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
	typedef struct _MonoString MonoString;
}

namespace Rose {

	class MonoScriptClass
	{
	public:
		MonoScriptClass() = default;

		MonoScriptClass(const std::string& classNameSpace, const std::string& className, bool isCore = false);
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

		void InvokeOnCollision2DBeginInternal(UUID id);
		void InvokeOnCollision2DEndInternal(UUID id);
	private:
		Ref<MonoScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;

		MonoMethod* m_OnCollision2DBeginInternalMethod = nullptr;
		MonoMethod* m_OnCollision2DEndInternalMethod = nullptr;
	};

	class MonoScriptEngine
	{
	public:

		static void Init();
		static void Shutdown();

		static void LoadCoreAssembly(const std::filesystem::path& filepath);

		static void LoadAppAssembly(const std::filesystem::path& filepath);
		static void ReloadAppAssembly(const std::filesystem::path& filepath);
		static void UnloadAppAssembly();

		static MonoString* CreateMonoString(const std::string string);

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();
		static void OnCreateEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, float ts);
		static void OnCollision2DBeginInternal(Entity entity, UUID id);
		static void OnCollision2DEndInternal(Entity entity, UUID id);

		static bool EntityClassExist(const std::string& fullClassName);

		static Scene* GetSceneContext();
		static std::unordered_map<std::string, Ref<MonoScriptClass>> GetEntityClasses();

		static MonoImage* GetCoreAssemblyImage();
	private:
		static void LoadAssemblyClasses(MonoAssembly* assembly);
		static MonoObject* InstantiateClass(MonoClass* monoClass);


		friend class MonoScriptClass;
		friend class MonoGlue;
	};
}