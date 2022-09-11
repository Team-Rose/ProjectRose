#include "rrpch.h"
#include "MonoGlue.h"

#include "MonoScriptEngine.h"
#include "RoseRoot/Scene/Scene.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"
#include <glm/glm.hpp>
#include <box2d/b2_body.h>
#include <mono/metadata/appdomain.h>
#include <RoseRoot/Assets/AssetManager.h>

namespace Rose {

	static std::unordered_map <MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define RR_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Rose.InternalCalls::" #Name, Name)

#pragma region  Input
	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}
#pragma endregion

#pragma region  Asset
	static bool Asset_GetOrLoadTexture2D(MonoString* path)
	{
		char* utf8 = mono_string_to_utf8(path);
		std::string string = std::string(utf8);

		if (AssetManager::GetOrLoadTexture(AssetManager::GetAssetPath() + "\\" + string))
			return true;
		return false;
	}
	static bool Asset_GetTexture2D(MonoString* path)
	{
		char* utf8 = mono_string_to_utf8(path);
		std::string string = std::string(utf8);

		if (AssetManager::GetTexture(AssetManager::GetAssetPath() + "\\" + string))
			return true;
		return false;
	}
#pragma endregion

#pragma region  Scene
	static uint64_t Scene_FindEntityByTag(MonoString* tag)
	{
		auto view = MonoScriptEngine::GetSceneContext()->GetAllEntitiesWith<TagComponent>();
		char* utf8 = mono_string_to_utf8(tag);
		std::string string = std::string(utf8);
		for (auto e : view)
		{
			auto tc = view.get<TagComponent>(e);
			if (tc.Tag == string) {
				Entity entity = Entity{ e, MonoScriptEngine::GetSceneContext() };
				return entity.GetUUID();
			}
		}
		return 0;
	}
#pragma endregion

#pragma region  Entity
	static uint64_t Entity_GetParent(UUID entityID) {
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		if (entity.HasComponent<RelationshipComponent>())
			return entity.GetComponent<RelationshipComponent>().ParentHandle;

		return 0;
	}

	static void Entity_SetParent(UUID entityID, UUID parentID) {
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);
		Entity parent = scene->GetEntityByUUID(parentID);
		RR_CORE_ASSERT(parent);

		scene->ParentEntity(entity, parent);
	}

	static MonoArray* Entity_GetChildren(UUID entityID) {
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		MonoClass* uint64class = mono_class_from_name(mono_get_corlib(), "System", "UInt64");
		RR_CORE_ASSERT(uint64class);

		if (!entity.HasComponent<RelationshipComponent>())
			return mono_array_new(MonoScriptEngine::GetAppDomain(), uint64class, 0);

		const auto& children = entity.GetComponent<RelationshipComponent>().Children;
		MonoArray* arr = mono_array_new(MonoScriptEngine::GetAppDomain(), uint64class, children.size());

		for (uint32_t i = 0; i < children.size(); i++) {
			mono_array_set(arr, int64_t, i, children[i]);
		}
		return arr;
	}


	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		RR_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());

		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}
	static MonoString* Entity_GetTag(UUID entityID, MonoString* outTag)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		std::string tag = entity.GetComponent<TagComponent>().Tag;
		return(MonoScriptEngine::CreateMonoString(tag));

	}
	static void Entity_SetTag(UUID entityID, MonoString* tag)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		char* utf8 = mono_string_to_utf8(tag);
		entity.GetComponent<TagComponent>().Tag = std::string(utf8);
		mono_free(utf8);
	}
#pragma endregion

#pragma region  TranformComponent
	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		*outTranslation = entity.GetComponent<TransformComponent>().Translation;
	}
	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		entity.GetComponent<TransformComponent>().Translation = *translation;
	}

	static void TransformComponent_GetRotation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		*outTranslation = entity.GetComponent<TransformComponent>().Rotation;
	}
	static void TransformComponent_SetRotation(UUID entityID, glm::vec3* rotation)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		entity.GetComponent<TransformComponent>().Rotation = *rotation;
	}

	static void TransformComponent_GetScale(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		*outTranslation = entity.GetComponent<TransformComponent>().Scale;
	}
	static void TransformComponent_SetScale(UUID entityID, glm::vec3* scale)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		entity.GetComponent<TransformComponent>().Scale = *scale;
	}
#pragma endregion

#pragma region  SpriteRendererComponent
	static void SpriteRendererComponent_GetColor(UUID entityID, glm::vec4* outColor)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		*outColor = entity.GetComponent<SpriteRendererComponent>().Color;
	}
	static void SpriteRendererComponent_SetColor(UUID entityID, glm::vec4* color)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		entity.GetComponent<SpriteRendererComponent>().Color = *color;
	}

	static float SpriteRendererComponent_GetTileFactor(UUID entityID) {
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		return entity.GetComponent<SpriteRendererComponent>().TilingFactor;
	}
	static void SpriteRendererComponent_SetTileFactor(UUID entityID, float tileFactor) {
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		entity.GetComponent<SpriteRendererComponent>().TilingFactor = tileFactor;
	}

	static void SpriteRendererComponent_SetTexture2D(UUID entityID, MonoString* path)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);
		auto& src = entity.GetComponent<SpriteRendererComponent>();
		char* utf8 = mono_string_to_utf8(path);
		RR_CORE_ASSERT(utf8 != nullptr);
		std::string string = std::string(utf8);
		src.Path = (AssetManager::GetAssetPath() + "/" + string);
		src.Texture = AssetManager::GetOrLoadTexture(src.Path);
	}
	static MonoString* SpriteRendererComponent_GetTexture2D(UUID entityID)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);
		auto& src = entity.GetComponent<SpriteRendererComponent>();
		std::string copy = src.Path;
		size_t pos = copy.find_last_of("\\");
		RR_CORE_TRACE(copy.substr(pos + 1));
		return mono_string_new(MonoScriptEngine::GetAppDomain(), copy.substr(pos + 1).c_str());
	}
#pragma endregion

#pragma region  RigidBody2DComponent
	static void RigidBody2DComponent_GetPosition(UUID entityID, glm::vec2* outPosition)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		*outPosition = { body->GetPosition().x, body->GetPosition().y };
	}
	static void RigidBody2DComponent_SetPosition(UUID entityID, glm::vec2* position)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		//TODO fix crash if you call in rigidbody2D collide callback
		body->SetTransform(b2Vec2(position->x, position->y), body->GetAngle());
	}

	static float RigidBody2DComponent_GetRotation(UUID entityID)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		return body->GetAngle();
	}
	static void RigidBody2DComponent_SetRotation(UUID entityID, float rotation)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		//TODO fix crash if you call in rigidbody2D collide callback
		body->SetTransform(body->GetPosition(), rotation);
	}

	static void RigidBody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}
	static void RigidBody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}
#pragma endregion

	template<typename ... Component>
	static void RegisterComponent()
	{
		([]()
		{
			std::string_view typeName = typeid(Component).name();
			size_t pos = typeName.find_last_of(':');
			std::string_view structName = typeName.substr(pos + 1);
			std::string managedTypeName = fmt::format("Rose.{}", structName);

			MonoType* managedType = mono_reflection_type_from_name(managedTypeName.data(), MonoScriptEngine::GetCoreAssemblyImage());
			if (!managedType)
			{
				RR_CORE_ERROR("Couldn't find C# component type {}", managedTypeName);
				return;
			}
			s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
		}(), ...);
		
	}

	template<typename ... Component>
	static void RegisterComponent(ComponentGroup<Component ...>)
	{
		RegisterComponent<Component ...>();
	}

	void MonoGlue::RegisterComponents()
	{
		RegisterComponent(AllComponents{});
	}

	void MonoGlue::RegisterFunctions()
	{
		RR_ADD_INTERNAL_CALL(Asset_GetOrLoadTexture2D);
		RR_ADD_INTERNAL_CALL(Asset_GetTexture2D);

		RR_ADD_INTERNAL_CALL(Scene_FindEntityByTag);

		RR_ADD_INTERNAL_CALL(Entity_GetParent);
		RR_ADD_INTERNAL_CALL(Entity_SetParent);
		RR_ADD_INTERNAL_CALL(Entity_GetChildren);
		RR_ADD_INTERNAL_CALL(Entity_HasComponent);
		RR_ADD_INTERNAL_CALL(Entity_GetTag);
		RR_ADD_INTERNAL_CALL(Entity_SetTag);

		RR_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		RR_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		RR_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		RR_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		RR_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		RR_ADD_INTERNAL_CALL(TransformComponent_SetScale);

		RR_ADD_INTERNAL_CALL(SpriteRendererComponent_GetColor);
		RR_ADD_INTERNAL_CALL(SpriteRendererComponent_SetColor);
		RR_ADD_INTERNAL_CALL(SpriteRendererComponent_GetTileFactor);
		RR_ADD_INTERNAL_CALL(SpriteRendererComponent_SetTileFactor);
		RR_ADD_INTERNAL_CALL(SpriteRendererComponent_SetTexture2D);
		RR_ADD_INTERNAL_CALL(SpriteRendererComponent_GetTexture2D);

		RR_ADD_INTERNAL_CALL(RigidBody2DComponent_GetPosition);
		RR_ADD_INTERNAL_CALL(RigidBody2DComponent_SetPosition);
		RR_ADD_INTERNAL_CALL(RigidBody2DComponent_GetRotation);
		RR_ADD_INTERNAL_CALL(RigidBody2DComponent_SetRotation);
		RR_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulse);
		RR_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulseToCenter);

		RR_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}
	
}

 