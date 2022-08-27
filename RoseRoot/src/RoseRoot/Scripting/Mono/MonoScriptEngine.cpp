#include "rrpch.h"
#include "MonoScriptEngine.h"

#include "MonoGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include <mono/metadata/mono-gc.h>
#include "mono/metadata/object.h"
#include <glm/glm.hpp>

namespace Rose {

	namespace Utils {
		// TODO: Move to FileSystem class
		static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream)
			{
				// Failed to open the file
				return nullptr;
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint32_t size = end - stream.tellg();

			if (size == 0)
			{
				// File is empty
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = size;
			return buffer;
		}

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath, &fileSize);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			std::string string = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, string.c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the file data
			delete[] fileData;

			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
				RR_CORE_TRACE("{}.{}", nameSpace, name);
			}
		}
	}

	struct MonoScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;
		MonoDomain* OldAppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;

		MonoScriptClass EntityClass;

		std::unordered_map<std::string, Ref<MonoScriptClass>> EntityClasses;
		std::unordered_map<UUID, Ref<MonoScriptInstance>> EntityInstances;
		
		Scene* SceneContext;
	};

	static MonoScriptEngineData* s_MonoData = nullptr;

	void MonoScriptEngine::Init()
	{
		mono_set_assemblies_path("mono/lib");
		s_MonoData = new MonoScriptEngineData();

		MonoDomain* rootDomain = mono_jit_init("RoseJITRuntime");
		RR_CORE_ASSERT(rootDomain);

		// Store the root domain pointer
		s_MonoData->RootDomain = rootDomain;
	}

	void MonoScriptEngine::Shutdown()
	{
		s_MonoData->AppDomain = nullptr;
		s_MonoData->RootDomain = nullptr;

		delete s_MonoData;
	}

	void MonoScriptEngine::LoadCoreAssembly(const std::filesystem::path& filepath)
	{
		s_MonoData->CoreAssembly = Utils::LoadMonoAssembly(filepath);
		s_MonoData->CoreAssemblyImage = mono_assembly_get_image(s_MonoData->CoreAssembly);
	}

	void MonoScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
	{
		s_MonoData->AppAssembly = Utils::LoadMonoAssembly(filepath);
		s_MonoData->AppAssemblyImage = mono_assembly_get_image(s_MonoData->AppAssembly);
		LoadAssemblyClasses(s_MonoData->AppAssembly);
	}

	void MonoScriptEngine::ReloadAppAssembly(const std::filesystem::path& filepath)
	{
		UnloadAppAssembly();

		LoadCoreAssembly("Resources/Scripts/Rose-ScriptCore.dll");
		LoadAppAssembly(filepath);

		MonoGlue::RegisterComponents();
		MonoGlue::RegisterFunctions();
		s_MonoData->EntityClass = MonoScriptClass("Rose", "Entity", true);

		mono_gc_collect(mono_gc_max_generation());
	}

	void MonoScriptEngine::UnloadAppAssembly()
	{
		s_MonoData->OldAppDomain = s_MonoData->AppDomain;
		s_MonoData->AppDomain = mono_domain_create_appdomain("RoseScriptRuntime", nullptr);
		mono_domain_set(s_MonoData->AppDomain, true);
		if(s_MonoData->OldAppDomain != nullptr)
			mono_domain_unload(s_MonoData->OldAppDomain);

		s_MonoData->EntityInstances.clear();
		s_MonoData->EntityClasses.clear();

		s_MonoData->AppAssembly = nullptr;
		s_MonoData->AppAssemblyImage = nullptr;
	}

	void MonoScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_MonoData->SceneContext = scene;
	}

	void MonoScriptEngine::OnRuntimeStop()
	{
		s_MonoData->SceneContext = nullptr;
		s_MonoData->EntityInstances.clear();
	}

	void MonoScriptEngine::OnCreateEntity(Entity entity)
	{
		const MonoScriptComponent& msc = entity.GetComponent<MonoScriptComponent>();
		if (MonoScriptEngine::EntityClassExist(msc.ClassName)) {
			Ref<MonoScriptInstance> instance = CreateRef<MonoScriptInstance>(s_MonoData->EntityClasses[msc.ClassName], entity);
			s_MonoData->EntityInstances[entity.GetUUID()] = instance;

			instance->InvokeOnCreate();
		}
	}

	void MonoScriptEngine::OnUpdateEntity(Entity entity, float ts)
	{
		if (!s_MonoData->AppAssembly)
			return;

		UUID entityUUID = entity.GetUUID();
		RR_CORE_ASSERT(s_MonoData->EntityInstances.find(entityUUID) != s_MonoData->EntityInstances.end());

		s_MonoData->EntityInstances[entityUUID]->InvokeOnUpdate(ts);
	}

	Scene* MonoScriptEngine::GetSceneContext()
	{
		return s_MonoData->SceneContext;
	}

	bool MonoScriptEngine::EntityClassExist(const std::string& fullClassName)
	{
		return s_MonoData->EntityClasses.find(fullClassName) != s_MonoData->EntityClasses.end();
	}

	std::unordered_map<std::string, Ref<MonoScriptClass>> MonoScriptEngine::GetEntityClasses()
	{
		return s_MonoData->EntityClasses;
	}

	void MonoScriptEngine::LoadAssemblyClasses(MonoAssembly* assembly)
	{
		s_MonoData->EntityClasses.clear();

		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* entityClass = mono_class_from_name(s_MonoData->CoreAssemblyImage, "Rose", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			std::string fullName;
			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, name);
			else
				fullName = name;
			MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);
			if (monoClass == entityClass)
				continue;
			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if (isEntity)
				s_MonoData->EntityClasses[fullName] = CreateRef<MonoScriptClass>(nameSpace, name);
		}
	}
	MonoObject* MonoScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_MonoData->AppDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	MonoImage* MonoScriptEngine::GetCoreAssemblyImage()
	{
		return s_MonoData->CoreAssemblyImage;
	}

	//-----------------------

	MonoScriptClass::MonoScriptClass(const std::string& classNameSpace, const std::string& className, bool isCore)
		: m_ClassNameSpace(classNameSpace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(isCore ? s_MonoData->CoreAssemblyImage : s_MonoData->AppAssemblyImage, classNameSpace.c_str(), className.c_str());
	}
	MonoObject* MonoScriptClass::Instantiate()
	{
		return MonoScriptEngine::InstantiateClass(m_MonoClass);
	}
	MonoMethod* MonoScriptClass::GetMethod(const std::string& name, int parameterCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
	}
	MonoObject* MonoScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		return mono_runtime_invoke(method, instance, params, nullptr);
	}



	MonoScriptInstance::MonoScriptInstance(Ref<MonoScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = scriptClass->Instantiate();

		m_Constructor = s_MonoData->EntityClass.GetMethod(".ctor", 1);
		m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);
		m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);
		
		// Call Entity Constructor
		{
			UUID entityID = entity.GetUUID();
			void* param = &entityID;
			m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
		}
		
	}
	void MonoScriptInstance::InvokeOnCreate()
	{
		m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
	}
	void MonoScriptInstance::InvokeOnUpdate(float ts)
	{
		void* param = &ts;
		m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
	}
}