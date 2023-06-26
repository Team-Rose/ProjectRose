#pragma once

#include "RoseRoot.h"

#include <string>
#include <RoseRoot/Asset/AssetManagers/EditorAssetManager.h>
#include <RoseRoot/Asset/AssetManagerHolder.h>

namespace Rose
 {
	struct ProjectConfig
	{
		std::string Name = "Untitled";

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path AssetRegistryPath = "AssetRegistry.rsr";
		std::filesystem::path ScriptModulePath;
	};

	class Project : public AssetManagerHolder {
	public:
		Project();
		~Project();
		
		ProjectConfig& GetConfig() { return m_Config; }
		virtual const std::filesystem::path GetAssetDirectory() override {
			return  m_ProjectDirectory / m_Config.AssetDirectory;
		}
		const std::filesystem::path GetAssetRegistryPath() {
			return  m_ProjectDirectory / m_Config.AssetRegistryPath;
		}

		const std::filesystem::path& GetProjectDirectory() { return m_ProjectDirectory; }
		virtual Ref<AssetManagerBase> GetAssetManager() override { return m_AssetManager; }

		//TODO(Sam) Maybe move this seems out of place
		void ImportAsset(const std::filesystem::path& path) {
			Project::GetActive()->GetEditorAssetManager()->ImportAsset(path);
			Project::GetActive()->GetEditorAssetManager()->SerializeAssetRegistry(GetAssetRegistryPath());
		}
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
		static const std::filesystem::path GetActiveAssetRegistryPath()
		{
			RR_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetAssetRegistryPath();
		}

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);
	private:
		ProjectConfig m_Config;

		std::filesystem::path m_ProjectDirectory;
		Ref<EditorAssetManager> m_AssetManager;

		inline static Ref<Project> s_ActiveProject;
	};
}