#pragma once

//#include <memory.h>
#include <map>
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
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoDomain MonoDomain;
}

namespace Rose {

	enum class MonoScriptFieldType
	{
		None = 0,

		Float, Double,
		Bool, Char, Int16, Int32, Int64,
		Byte, UInt16, UInt32, UInt64,

		Vector2, Vector3, Vector4,
		Entity
	};

	struct MonoScriptField
	{
		MonoScriptFieldType Type;
		std::string Name;

		MonoClassField* ClassField;
	};

	//MonoScriptField + data storage
	struct MonoScriptFieldInstance
	{
		MonoScriptField Field;

		MonoScriptFieldInstance()
		{
			memset(m_Buffer, 0, sizeof(m_Buffer));
		}
		//TODO Allow for larger data types!
		template<typename T>
		T GetValue() {
			static_assert(sizeof(T) <= 8, "Type to large!");

			return *(T*)m_Buffer;
		}

		template<typename T>
		void SetValue(T value) {
			static_assert(sizeof(T) <= 8, "Type to large!");
			memcpy(m_Buffer, &value, sizeof(T));
		}
	private:
		uint8_t m_Buffer[8];

		friend class MonoScriptEngine;
	};
	using MonoScriptFieldMap = std::unordered_map<std::string, MonoScriptFieldInstance>;

	class MonoScriptClass
	{
	public:
		MonoScriptClass() = default;

		MonoScriptClass(const std::string& classNameSpace, const std::string& className, bool isCore = false);
		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		const std::map<std::string, MonoScriptField> GetFields() const { return m_Fields; }
	private:
		std::string m_ClassNameSpace;
		std::string m_ClassName;

		std::map<std::string, MonoScriptField> m_Fields;
		MonoClass* m_MonoClass = nullptr;

		friend class MonoScriptEngine;
	};

	class MonoScriptInstance
	{
	public:
		MonoScriptInstance(Ref<MonoScriptClass> scriptClass, Entity entity);

		void InvokeOnCreate();
		void InvokeOnUpdate(float ts);

		void InvokeOnCollision2DBeginInternal(UUID id);
		void InvokeOnCollision2DEndInternal(UUID id);

		Ref<MonoScriptClass> GetMonoScriptClass() { return m_ScriptClass; }
		
		template<typename T>
		T GetFieldValue(const std::string& name) {
			if (!GetFieldValueInternal(name, s_FieldValueBuffer)) {
				return T();
			}
			return *(T*)s_FieldValueBuffer;
		}

		template<typename T>
		void SetFieldValue(const std::string& name, const T& value) {
			SetFieldValueInternal(name, &value);
		}
	private:
		bool GetFieldValueInternal(const std::string& name, void* buffer);
		bool SetFieldValueInternal(const std::string& name, const void* value);
	private:
		Ref<MonoScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;

		MonoMethod* m_OnCollision2DBeginInternalMethod = nullptr;
		MonoMethod* m_OnCollision2DEndInternalMethod = nullptr;

		inline static char s_FieldValueBuffer[8];

		friend class MonoScriptEngine;
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

		static MonoDomain* GetAppDomain();
		static MonoString* CreateMonoString(const std::string string);

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();
		static void OnCreateEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, float ts);
		static void OnCollision2DBeginInternal(Entity entity, UUID id);
		static void OnCollision2DEndInternal(Entity entity, UUID id);

		static bool EntityClassExist(const std::string& fullClassName);

		static Scene* GetSceneContext();
		static Ref<MonoScriptInstance> GetEntityMonoScriptInstance(UUID entityID);

		static Ref<MonoScriptClass> GetEntityClass(const std::string& name);
		static std::unordered_map<std::string, Ref<MonoScriptClass>> GetEntityClasses();
		static MonoScriptFieldMap& GetScriptFieldMap(UUID entityID);

		static MonoImage* GetCoreAssemblyImage();
	private:
		static void LoadAssemblyClasses(MonoAssembly* assembly);
		static MonoObject* InstantiateClass(MonoClass* monoClass);

		friend struct MonoScriptField;
		friend struct MonoScriptFieldInstance;
		friend class MonoScriptClass;
		friend class MonoGlue;
	};
}