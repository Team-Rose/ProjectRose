#include "rrpch.h"
#include "MonoScriptEngine.h"

#include "MonoGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include <glm/glm.hpp>

namespace Rose {

	namespace Utils {
		// TODO: Move to FIleSystem class
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

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		MonoScriptClass EntityClass;
	};

	static MonoScriptEngineData* s_MonoData = nullptr;

	void MonoScriptEngine::Init()
	{
		InitMono();
		LoadAssembly("Resources/Scripts/Rose-ScriptCore.dll");

		MonoGlue::RegisterFunctions();

		// Retrieve and instantiate class
		s_MonoData->EntityClass = MonoScriptClass("Rose", "Entity");
		MonoObject* instance = s_MonoData->EntityClass.Instantiate();

		// call method
		MonoMethod* printMessageMethod = s_MonoData->EntityClass.GetMethod("PrintMessage", 0);
		s_MonoData->EntityClass.InvokeMethod(instance ,printMessageMethod, nullptr);

		// call method with param

		int value = 5;
		int value2 = 123;
		void* params[2] = {
			&value,
			&value2
		};

		MonoMethod* printIntsMethod = s_MonoData->EntityClass.GetMethod("PrintInts", 2);
		s_MonoData->EntityClass.InvokeMethod(instance, printIntsMethod, params);


		MonoString* monoString = mono_string_new(s_MonoData->AppDomain, "Hello Worlds from C++!!!! :D");
		MonoMethod* printStringMethod = s_MonoData->EntityClass.GetMethod("PrintString", 1);
		void* param = monoString;

		s_MonoData->EntityClass.InvokeMethod(instance, printStringMethod, &param);
	}

	void MonoScriptEngine::Shutdown()
	{
		ShutDownMono();
	}

	void MonoScriptEngine::InitMono()
	{
		s_MonoData = new MonoScriptEngineData();

		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("RoseJITRuntime");
		RR_CORE_ASSERT(rootDomain);

		// Store the root domain pointer
		s_MonoData->RootDomain = rootDomain;
	}

	void MonoScriptEngine::ShutDownMono()
	{
		s_MonoData->AppDomain = nullptr;
		s_MonoData->RootDomain = nullptr;

		delete s_MonoData;
	}

	void MonoScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
	{
		// Create an App Domain
		s_MonoData->AppDomain = mono_domain_create_appdomain("RoseScriptRuntime", nullptr);
		mono_domain_set(s_MonoData->AppDomain, true);

		//Move this maybe
		s_MonoData->CoreAssembly = Utils::LoadMonoAssembly(filepath);
		//PrintAssemblyTypes(s_MonoData->CoreAssembly);

		s_MonoData->CoreAssemblyImage = mono_assembly_get_image(s_MonoData->CoreAssembly);
	}

	MonoObject* MonoScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_MonoData->AppDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}


	MonoScriptClass::MonoScriptClass(const std::string& classNameSpace, const std::string& className)
		: m_ClassNameSpace(classNameSpace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(s_MonoData->CoreAssemblyImage, classNameSpace.c_str(), className.c_str());
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
}