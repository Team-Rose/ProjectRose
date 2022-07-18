#pragma once

#include <filesystem>
#include <string>

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
}

namespace Rose {

	class MonoScriptEngine
	{
	public:

		static void Init();
		static void Shutdown();

		static void LoadAssembly(const std::filesystem::path& filepath);
	private:
		static void InitMono();
		static void ShutDownMono();

		static MonoObject* InstantiateClass(MonoClass* monoClass);

		friend class MonoScriptClass;
	};

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

}