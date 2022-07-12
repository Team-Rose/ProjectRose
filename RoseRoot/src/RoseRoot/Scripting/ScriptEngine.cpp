#include "rrpch.h"
#include "ScriptEngine.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

namespace Rose {
	struct MonoScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
	};

	static MonoScriptEngineData* s_MonoData = nullptr;

	void ScriptEngine::Init()
	{
		InitMono();
		InitLua();
	}

	void ScriptEngine::Shutdown()
	{
		ShutDownMono();
	}

	char* ReadBytes(const std::string& filepath, uint32_t* outSize)
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

	MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
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

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
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

	void ScriptEngine::InitMono()
	{
		s_MonoData = new MonoScriptEngineData();

		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("RoseJITRuntime");
		RR_CORE_ASSERT(rootDomain);

		// Store the root domain pointer
		s_MonoData->RootDomain = rootDomain;

		// Create an App Domain
		s_MonoData->AppDomain = mono_domain_create_appdomain("RoseScriptRuntime", nullptr);
		mono_domain_set(s_MonoData->AppDomain, true);

		//Move this maybe
		s_MonoData->CoreAssembly = LoadCSharpAssembly("Resources/Scripts/Rose-ScriptCore.dll");
		PrintAssemblyTypes(s_MonoData->CoreAssembly);

		// create an object
		MonoImage* assemblyImage = mono_assembly_get_image(s_MonoData->CoreAssembly);
		MonoClass* monoClasss = mono_class_from_name(assemblyImage, "Rose", "Main");

		MonoObject* instance = mono_object_new(s_MonoData->AppDomain, monoClasss);
		mono_runtime_object_init(instance);
		
		// call function
		MonoMethod* printMessageMethod = mono_class_get_method_from_name(monoClasss, "PrintMessage", 0);
		mono_runtime_invoke(printMessageMethod, instance, nullptr, nullptr);

		// call function with param
		MonoMethod* printIntsMethod = mono_class_get_method_from_name(monoClasss, "PrintInts", 2);

		int value = 5;
		int value2 = 123;
		void* params[2] = {
			&value,
			&value2
		};

		mono_runtime_invoke(printIntsMethod, instance, params, nullptr);

		MonoString* monoString = mono_string_new(s_MonoData->AppDomain, "Hello Worlds from C++!!!! :D");
		MonoMethod* printStringMethod = mono_class_get_method_from_name(monoClasss, "PrintString", 1);
		void* param = monoString;

		mono_runtime_invoke(printStringMethod, instance, &param, nullptr);
	}


	void ScriptEngine::ShutDownMono()
	{
		s_MonoData->AppDomain = nullptr;
		s_MonoData->RootDomain = nullptr;

		delete s_MonoData;
	}

	void ScriptEngine::InitLua()
	{
	}


}