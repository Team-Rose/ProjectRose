#include "rrpch.h"
#include "MonoGlue.h"

#include "MonoScriptEngine.h"
#include "RoseRoot/Scene/Scene.h"

#include "mono/metadata/object.h"
#include <glm/glm.hpp>

namespace Rose {

#define RR_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Rose.InternalCalls::" #Name, Name)

	static void NativeLog(MonoString* string, int parameter) {
		char* cStr = mono_string_to_utf8(string);
		RR_CORE_INFO("{}, {}", cStr, parameter);
		mono_free(cStr);
	}

	static void NativeLog_Vector3(MonoString* string, glm::vec3* parameter) {
		char* cStr = mono_string_to_utf8(string);
		RR_CORE_INFO("{}{}", cStr, *parameter);
	}

	static void CustomTypeReturnExample(glm::vec3* parameter, glm::vec3* outResult) {
		RR_CORE_INFO("Value: {}", *parameter);
		*outResult = glm::vec3(-parameter->x, -parameter->y, -parameter->z);
	}

	static void Entity_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();

		Entity entity = scene->GetEntityByUUID(entityID);
		*outTranslation = entity.GetComponent<TransformComponent>().Translation;
	}

	static void Entity_SetTranslation(UUID entityID, glm::vec3* translation)
	{
		Scene* scene = MonoScriptEngine::GetSceneContext();

		Entity entity = scene->GetEntityByUUID(entityID);
		entity.GetComponent<TransformComponent>().Translation = *translation;
	}

	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}


	void MonoGlue::RegisterFunctions()
	{
		RR_ADD_INTERNAL_CALL(NativeLog);
		RR_ADD_INTERNAL_CALL(NativeLog_Vector3);
		RR_ADD_INTERNAL_CALL(CustomTypeReturnExample);

		RR_ADD_INTERNAL_CALL(Entity_GetTranslation);
		RR_ADD_INTERNAL_CALL(Entity_SetTranslation);

		RR_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}
}

 