#pragma once

#include <filesystem>
#include <string>

#include "RoseRoot/Scene/Scene.h"
#include "RoseRoot/Scene/Entity.h"

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
}

namespace Rose {

	class MonoScriptClass
	{
	public:
		MonoScriptClass() = default;

		MonoScriptClass(const std::string& classNameSpace, const std::string& className);
		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);
	private:
		std::string m_ClassNameSpace;
		std::string m_ClassName;

		MonoClass* m_MonoClass = nullptr;
	};

	class MonoScriptInstance
	{
	public:
		MonoScriptInstance(Ref<MonoScriptClass> scriptClass, Entity entity);

		void InvokeOnCreate();
		void InvokeOnUpdate(float ts);
	private:
		Ref<MonoScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;
	};

	class MonoScriptEngine
	{
	public:

		static void Init();
		static void Shutdown();

		static void LoadAssembly(const std::filesystem::path& filepath);

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();
		static void OnCreateEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, float ts);

		static bool EntityClassExist(const std::string& fullClassName);

		static Scene* GetSceneContext();
		static std::unordered_map<std::string, Ref<MonoScriptClass>> GetEntityClasses();
	private:
		static void InitMono();
		static void ShutDownMono();

		static void LoadAssemblyClasses(MonoAssembly* assembly);
		static MonoObject* InstantiateClass(MonoClass* monoClass);

		friend class MonoScriptClass;
	};
}