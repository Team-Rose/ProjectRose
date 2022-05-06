#pragma once
#include "RoseRoot/Scene/Scene.h"

namespace Rose
 {
	

	class Project {
	public:
		Project(std::filesystem::path path = "startup-project");
		~Project();
		
		void SetName(std::string name) { m_Name = name; }
		std::string GetName() { return m_Name; }
		std::filesystem::path GetPath() { return m_Path; }
		std::filesystem::path GetAssetPath() { std::filesystem::path path = m_Path / "assets"; return path; }

		std::unordered_map<int, std::filesystem::path>* GetSceneIndexPtr() { return &m_ScenePaths; }
		void SetSceneToIndex(int index, std::filesystem::path path);
		bool IsSceneAtIndex(int index);

		std::filesystem::path GetScenePathFromIndex(int index) { return m_ScenePaths.at(index); }
		void ClearSceneIndex() { m_ScenePaths.clear(); }
		int GetSizeOfSceneIndex();

		void SaveProject();
		bool OpenProject(std::filesystem::path path);
	private:
		std::string m_Name = "UntitledProject";
		//Absolute path of the project
		std::filesystem::path m_Path;

		//The relative path of all scenes in the build index
		std::unordered_map<int, std::filesystem::path> m_ScenePaths;
	};
}