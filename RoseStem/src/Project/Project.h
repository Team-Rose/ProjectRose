#pragma once

#include "RoseRoot.h"

#include <string>

namespace Rose
 {
	struct ProjectConfig
	{
		std::string Name = "Untitled";

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path ScriptModulePath;
	};

	class Project {
	public:
		Project();
		~Project();
		
		static Ref<Project> GetActive() { return s_ActiveProject; };
		static const std::filesystem::path& GetActiveProjectDirectory()
		{
			RR_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->m_ProjectDirectory;
		}

		static const std::filesystem::path GetActiveAssetDirectory()
		{
			RR_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetAssetDirectory();
		}

		ProjectConfig& GetConfig() { return m_Config; }
		const std::filesystem::path GetAssetDirectory() {
			return  m_ProjectDirectory / m_Config.AssetDirectory;
		}
		const std::filesystem::path& GetProjectDirectory() { return m_ProjectDirectory; }

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);
	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;

		inline static Ref<Project> s_ActiveProject;
	};
}