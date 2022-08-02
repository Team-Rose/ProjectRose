#include "rrpch.h"
#include "MonoGlue.h"

#include "MonoScriptEngine.h"
#include "RoseRoot/Scene/Scene.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"
#include <glm/glm.hpp>
#include <box2d/b2_body.h>

namespace Rose {

	static std::unordered_map <MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define RR_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Rose.InternalCalls::" #Name, Name)

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

	

	static void RigidBody2DComponeny_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}
	static void RigidBody2DComponeny_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();
		RR_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		RR_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

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
				RR_CORE_ERROR("Couldn't find component type {}", managedTypeName);
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
		RR_ADD_INTERNAL_CALL(Entity_HasComponent);

		RR_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		RR_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		RR_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		RR_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		RR_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		RR_ADD_INTERNAL_CALL(TransformComponent_SetScale);

		RR_ADD_INTERNAL_CALL(RigidBody2DComponeny_ApplyLinearImpulse);
		RR_ADD_INTERNAL_CALL(RigidBody2DComponeny_ApplyLinearImpulseToCenter);

		RR_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}
	
}

 