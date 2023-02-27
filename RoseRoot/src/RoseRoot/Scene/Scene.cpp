#include "rrpch.h"
#include "Scene.h"

#include "Components.h"
#include "ScriptableEntity.h"
#include "RoseRoot/Scripting/LUA/Lua.h"
#include "RoseRoot/Scripting/Mono/MonoScriptEngine.h"
#include "RoseRoot/Assets/AssetManager.h"

#include "RoseRoot/Renderer/Renderer.h"
#include "RoseRoot/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

#include "Entity.h"

#include <RoseRoot/Physics/2D/ContactListener.h>

// Box 2D
#include "Box2D/b2_world.h"
#include "Box2D/b2_body.h"
#include "Box2D/b2_fixture.h"
#include "Box2D/b2_polygon_shape.h"
#include "Box2D/b2_circle_shape.h"
#include <RoseRoot/Core/Timer.h>
#include <RoseRoot/Renderer/Renderer3D.h>

namespace Rose
 {
	namespace Utils {
		static bool EntityIsAncestorOfOther(Entity entity, Entity other) {
			if (!entity.GetScene())
				return false;

			if (other.HasComponent<RelationshipComponent>() 
				&& entity.HasComponent<RelationshipComponent>()) {

				for (auto &childID : other.GetComponent<RelationshipComponent>().Children) {
					if (childID == entity.GetUUID())
						return true;

					Entity child = entity.GetScene()->GetEntityByUUID(childID);

					//TODO Maybe make this go one layer at at time to avoid getting stuck in complex child structures
					if (EntityIsAncestorOfOther(entity, child))
						return true;
				}
			}

			return false;
		}
	}
	static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType) 
	{
		switch (bodyType)
		{
		case Rose::Rigidbody2DComponent::BodyType::Static:   return b2_staticBody;
		case Rose::Rigidbody2DComponent::BodyType::Dynamic:  return b2_dynamicBody;
		case Rose::Rigidbody2DComponent::BodyType::Kinematic:return b2_kinematicBody;
		}

		RR_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	Scene::Scene()
		: m_Contactlistener()
	{
	}

	Scene::~Scene()
	{
		delete m_PhysicsWorld;
	}

	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		([&]()
			{
				auto view = src.view<Component>();
				for (auto srcEntity : view)
				{
					entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

					auto& srcComponent = src.get<Component>(srcEntity);
					dst.emplace_or_replace<Component>(dstEntity, srcComponent);
				}
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		CopyComponent<Component...>(dst, src, enttMap);
	}


	template<typename Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		if (src.HasComponent<Component>())
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		newScene->m_SceneSettings = other->m_SceneSettings;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}
		//Not included in all components sense you are not aloud to access directly from the C# or LUA api
		CopyComponent(ComponentGroup<RelationshipComponent>{}, dstSceneRegistry, srcSceneRegistry, enttMap);
		// Copy components (except IDComponent and TagComponent)
		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Rose::Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		auto& rc = entity.AddComponent <RelationshipComponent>();
		rc.Children.clear();

		m_EntityMap[uuid] = entity;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		UnparentEntity(entity);

		auto& children = entity.GetComponent<RelationshipComponent>().Children;
		for (auto& childID : children) {
			if (Entity child = GetEntityByUUID(childID)) {
				DestroyEntity(child);
			}
		}

		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
	}

	void Scene::ParentEntity(Entity entity, Entity parent)
	{
		if (!GetEntityByUUID(entity.GetUUID()))
			return;
		if (!GetEntityByUUID(parent.GetUUID()))
			return;

		if (!entity.HasComponent<RelationshipComponent>() || !parent.HasComponent<RelationshipComponent>())
			return;	

		if (entity == parent) {
			UnparentEntity(entity);
			return;
		}
		
		if(Utils::EntityIsAncestorOfOther(parent, entity))
			return;

		RelationshipComponent& rc = entity.GetComponent<RelationshipComponent>();
		if (parent.GetUUID() == rc.ParentHandle) {
			UnparentEntity(entity);
			return;
		}
		if(rc.ParentHandle != 0)
			UnparentEntity(entity);

		RelationshipComponent& parentRc = parent.GetComponent<RelationshipComponent>();
		parentRc.Children.push_back(entity.GetUUID());

		rc.ParentHandle = parent.GetUUID();
	}

	void Scene::UnparentEntity(Entity entity)
	{
		RelationshipComponent& rc = entity.GetComponent<RelationshipComponent>();
		if (rc.ParentHandle == 0)
			return;

		Entity parent = GetEntityByUUID(rc.ParentHandle);
		if (!parent)
			return;

		std::vector<UUID>& parentChildren = parent.GetComponent<RelationshipComponent>().Children;
		parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), entity.GetUUID()), parentChildren.end());
		rc.ParentHandle = 0;
	}

	void Scene::OnRuntimeStart(const std::string& assetPath)
	{
		RR_PROFILE_FUNCTION();
		RR_CORE_TRACE("-----Runtime Scene Started-----");
		m_IsRunning = true;
		AssetManager::ReloadAssets();
		OnPhysics2DStart();

		m_Registry.view<SpriteRendererComponent>().each([=](auto entity, SpriteRendererComponent& src)
			{
				if (Ref<Texture2D> texture = AssetManager::GetOrLoadTexture(src.Path))
					src.Texture = texture;
			});

		

		MonoScriptEngine::OnRuntimeStart(this); 
		m_Registry.view<MonoScriptComponent>().each([=](auto entity, MonoScriptComponent& msc)
			{
				MonoScriptEngine::OnCreateEntity(Entity{ entity, this });
			});

		m_Registry.view<LuaScriptComponent>().each([=](auto entity, LuaScriptComponent& lsc)
			{
				lsc.Script = CreateRef<LuaScript>(Entity{ entity, this }, assetPath + "\\" + lsc.Path);
				lsc.Script->Init();
			});
	}

	void Scene::OnRuntimeStop()
	{
		OnPhysics2DStop();
		MonoScriptEngine::OnRuntimeStop();
		m_SceneStats = {};
		m_IsRunning = false;
		RR_CORE_TRACE("-----Runtime Scene Stopped-----");
	}

	void Scene::OnSimulationStart(const std::string& assetPath)
	{
		RR_PROFILE_FUNCTION();
		RR_CORE_TRACE("-----Simulation Scene Started-----");
		AssetManager::ReloadAssets();
		m_Registry.view<SpriteRendererComponent>().each([=](auto entity, SpriteRendererComponent& src)
			{
				if (Ref<Texture2D> texture = AssetManager::GetOrLoadTexture(src.Path))
					src.Texture = texture;
			});

		OnPhysics2DStart();
	}

	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
		m_SceneStats = {};
		RR_CORE_TRACE("-----Simulation Scene Stopped-----");
	}

	void Scene::Step(int step)
	{
		m_SteppedFrames += step;
	}

	void Scene::OnUpdateRuntime(Timestep ts, bool pause)
	{
		RR_PROFILE_FUNCTION();
		if (!pause || m_SteppedFrames > 0) {
			if (m_SteppedFrames > 0)
				m_SteppedFrames--;
			{
				RR_PROFILE_SCOPE("Update Scripts");
				Timer scriptTimer;

				m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
					{
						// TODO: Move to Scene::OnScenePlay
						if (!nsc.Instance)
						{
							nsc.Instance = nsc.InstantiateScript();
							nsc.Instance->m_Entity = Entity{ entity, this };
							nsc.Instance->OnCreate();
						}

						nsc.Instance->OnUpdate(ts);
					});

				//C# OnUpdate
				Timer monoScriptTimer;
				m_Registry.view<MonoScriptComponent>().each([=](auto entity, MonoScriptComponent& msc)
					{
						Entity rEntity = { entity, this };
						MonoScriptEngine::OnUpdateEntity(rEntity, (float)ts);
					});
				m_SceneStats.MonoScriptTime = monoScriptTimer.ElapsedMillis();

				//Lua OnUpdate
				Timer luaScriptTimer;
				m_Registry.view<LuaScriptComponent>().each([=](auto entity, auto& lsc)
					{
						lsc.Script->Update(ts);
					});
				m_SceneStats.LuaSciptTime = luaScriptTimer.ElapsedMillis();

				m_SceneStats.TotalScriptTime = scriptTimer.ElapsedMillis();
			}

			Timer physicsTimer;	
			//  Physics 2D
			{
				RR_PROFILE_SCOPE("Update Physics2D");

				const int32_t velocityIterations = 6;
				const int32_t positionIterations = 2;
				m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

				// Retrieve transform from Box2D
				auto view = m_Registry.view<Rigidbody2DComponent>();
				for (auto e : view)
				{
					Entity entity = { e, this };
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

					b2Body* body = (b2Body*)rb2d.RuntimeBody;
					const auto& position = body->GetPosition();
					transform.Translation.x = position.x;
					transform.Translation.y = position.y;
					transform.Rotation.z = body->GetAngle();
					//body->SetAwake(true);
				}
			}
			m_SceneStats.PhysicsTime = physicsTimer.ElapsedMillis();
		}

		Timer renderTimer;
		// Render
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer::BeginScene(*mainCamera, cameraTransform);
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();

			// Clear our entity ID attachment to -1
			Renderer::GetGeometryFrameBuffer()->ClearAttachment(1, -1);

			// Draw sprites
			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

					Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
				}
			}

			// Draw circles
			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

					Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
				}
			}

			// Draw text
			{
				auto view = m_Registry.view<TransformComponent, TextRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, text] = view.get<TransformComponent, TextRendererComponent>(entity);
					Renderer2D::DrawTextComponent(transform.GetTransform(), text, (int)entity);
				}
			}

			// Draw Meshes
			{
				auto view = m_Registry.view<TransformComponent, MeshRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, mrc] = view.get<TransformComponent, MeshRendererComponent>(entity);

					if (mrc.Model != nullptr && mrc.Model->GetMeshes().size() > 0) {
						Renderer3D::DrawMesh(transform.GetTransform(), mrc, (int)entity);
					}
				}
			}
			Renderer::EndScene();
		}
		m_SceneStats.RenderTime = renderTimer.ElapsedMillis();
	}

	void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& camera,bool pause)
	{
		RR_PROFILE_FUNCTION();
		if (!pause || m_SteppedFrames > 0) {
			if (m_SteppedFrames > 0)
				m_SteppedFrames--;

			Timer physicsTimer;
			//  Physics 2D
			{
				RR_PROFILE_SCOPE("Update Physics2D");

				const int32_t velocityIterations = 6;
				const int32_t positionIterations = 2;
				m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

				// Retrieve transform from Box2D
				auto view = m_Registry.view<Rigidbody2DComponent>();
				for (auto e : view)
				{
					Entity entity = { e, this };
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

					b2Body* body = (b2Body*)rb2d.RuntimeBody;
					const auto& position = body->GetPosition();
					transform.Translation.x = position.x;
					transform.Translation.y = position.y;
					transform.Rotation.z = body->GetAngle();
					//body->SetAwake(true);
				}
			}
			m_SceneStats.PhysicsTime = physicsTimer.ElapsedMillis();
		}
		Timer renderTimer;
		RenderScene(camera);
		m_SceneStats.RenderTime = renderTimer.ElapsedMillis();
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Timer renderTimer;
		RenderScene(camera);
		m_SceneStats.RenderTime = renderTimer.ElapsedMillis();
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}

	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);

		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CircleRendererComponent>(newEntity, entity);
		CopyComponentIfExists<TextRendererComponent>(newEntity, entity);
		CopyComponentIfExists<MeshRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<LuaScriptComponent>(newEntity, entity);

		if (entity.HasComponent<RelationshipComponent>()) {
			RelationshipComponent rc = entity.GetComponent<RelationshipComponent>();
			
			if (newEntity.GetScene() == entity.GetScene() && newEntity.GetScene()->GetEntityByUUID(rc.ParentHandle))
				newEntity.GetScene()->ParentEntity(newEntity, newEntity.GetScene()->GetEntityByUUID(rc.ParentHandle));
			
			for (auto& childId : rc.Children) {
				if (entity.GetScene()) {
					Entity childEntity = entity.GetScene()->GetEntityByUUID(childId);

					Entity newChildEntity = DuplicateEntity(childEntity);
					newEntity.GetScene()->ParentEntity(newChildEntity, newEntity);
				}
			}
		}

		return newEntity;
	}

	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
			return { m_EntityMap.at(uuid), this };

		RR_CORE_WARN("Entity with UUID {} was not found!", uuid);
		return {};
	} 

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}

	void Scene::OnPhysics2DStart()
	{
		m_PhysicsWorld = new b2World({ m_SceneSettings.Gravity2D.x, m_SceneSettings.Gravity2D.y });
		m_Contactlistener = new ContactListener2D();
		m_PhysicsWorld->SetContactListener(m_Contactlistener);

		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			body->GetUserData().pointer = (uintptr_t)entity.GetUUID();
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = transform.Scale.x * cc2d.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnPhysics2DStop()
	{
		delete m_PhysicsWorld;
		delete m_Contactlistener;
		m_PhysicsWorld = nullptr;
	}

	void Scene::RenderScene(EditorCamera& camera)
	{
		RR_PROFILE_FUNCTION();
		Renderer::BeginScene(camera);
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();

		Renderer::GetGeometryFrameBuffer()->ClearAttachment(1, -1);

		Renderer3D::DrawDirLight({ -0.2f, -1.0f, -0.3f });
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}
		}

		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}

		{
			auto view = m_Registry.view<TransformComponent, TextRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, text] = view.get<TransformComponent, TextRendererComponent>(entity);
				Renderer2D::DrawTextComponent(transform.GetTransform() ,text ,(int)entity);
		// Draw Meshes
		{
			auto view = m_Registry.view<TransformComponent, MeshRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, mrc] = view.get<TransformComponent, MeshRendererComponent>(entity);

				if (mrc.Model != nullptr && mrc.Model->GetMeshes().size() > 0) {
					Renderer3D::DrawMesh(transform.GetTransform(), mrc, (int)entity);
				}
			}
		}

		Renderer::EndScene();
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		RR_CORE_ERROR("Attempted to add unknown commponent ({}) to entity ({})", typeid(T)::name(), entity.GetUUID());
		RR_CORE_ASSERT();
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<RelationshipComponent>(Entity entity, RelationshipComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<TextRendererComponent>(Entity entity, TextRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<MeshRendererComponent>(Entity entity, MeshRendererComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<LuaScriptComponent>(Entity entity, LuaScriptComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<MonoScriptComponent>(Entity entity, MonoScriptComponent& component)
	{
	}
}