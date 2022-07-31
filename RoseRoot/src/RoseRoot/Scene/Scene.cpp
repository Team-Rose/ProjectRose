#include "rrpch.h"
#include "Scene.h"

#include "Components.h"
#include "ScriptableEntity.h"
#include "RoseRoot/Scripting/LUA/Lua.h"
#include "RoseRoot/Scripting/Mono/MonoScriptEngine.h"

#include "RoseRoot/Renderer/Renderer.h"
#include "RoseRoot/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

#include "Entity.h"


// Box 2D
#include "Box2D/b2_world.h"
#include "Box2D/b2_body.h"
#include "Box2D/b2_fixture.h"
#include "Box2D/b2_polygon_shape.h"
#include "Box2D/b2_circle_shape.h"

namespace Rose
 {

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

		m_EntityMap[uuid] = entity;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
		m_EntityMap.erase(entity.GetUUID());
	}

	void Scene::OnRuntimeStart(const std::string& assetPath)
	{
		RR_PROFILE_FUNCTION();
		RR_CORE_TRACE("-----Runtime Scene Started-----");
		OnPhysics2DStart();


		MonoScriptEngine::OnRuntimeStart(this);
		m_Registry.view<MonoScriptComponent>().each([=](auto entity, MonoScriptComponent& msc)
			{
				if (MonoScriptEngine::EntityClassExist(msc.ClassName)) {
					MonoScriptEngine::OnCreateEntity(Entity{ entity, this });
				}
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
	}

	void Scene::OnSimulationStart(const std::string& assetPath)
	{
		RR_PROFILE_FUNCTION();
		RR_CORE_TRACE("-----Simulation Scene Started-----");

		OnPhysics2DStart();
	}

	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
	}

	void Scene::OnUpdateRuntime(Timestep ts, bool pause)
	{
		RR_PROFILE_FUNCTION();
		if (!pause) {
			// Update scripts
			{
				RR_PROFILE_SCOPE("Update Scripts");

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
				m_Registry.view<MonoScriptComponent>().each([=](auto entity, MonoScriptComponent& msc)
					{
						Entity rEntity = { entity, this };
						MonoScriptEngine::OnUpdateEntity(rEntity, (float)ts);
					});

				//Lua OnUpdate
				m_Registry.view<LuaScriptComponent>().each([=](auto entity, auto& lsc)
					{
						lsc.Script->Update(ts);
					});
			}
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
		}
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
			Renderer::GetFinalFrameBuffer()->ClearAttachment(1, -1);

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

			Renderer::EndScene();
		}

	}

	void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& camera,bool pause)
	{
		RR_PROFILE_FUNCTION();

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

		RenderScene(camera);
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		RenderScene(camera);
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

	void Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);

		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CircleRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<LuaScriptComponent>(newEntity, entity);
	}

	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
			return { m_EntityMap.at(uuid), this };

		RR_CORE_WARN("Entity with UUID {} was not found!");
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
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);

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
		m_PhysicsWorld = nullptr;
	}

	void Scene::RenderScene(EditorCamera& camera)
	{
		RR_PROFILE_FUNCTION();
		Renderer::BeginScene(camera);
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();

		Renderer::GetFinalFrameBuffer()->ClearAttachment(1, -1);

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
		Renderer::EndScene();
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
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