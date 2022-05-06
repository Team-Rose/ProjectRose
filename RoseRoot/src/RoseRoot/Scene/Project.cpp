#include "rrpch.h"
#include "Project.h"

#include <yaml-cpp/yaml.h>

namespace Rose
 {
	Project::Project(std::filesystem::path path)
		: m_Path(path)
	{
		std::filesystem::create_directory(m_Path);
		std::filesystem::path assetPath = m_Path / "assets";
		std::filesystem::create_directory(assetPath);

		bool couldLoad = false;
		for (auto& directoryEntry : std::filesystem::directory_iterator(m_Path))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, m_Path);
			std::string filenameString = relativePath.filename().string();

			size_t LastDot = filenameString.find_last_of(".");

			if (LastDot != std::string::npos) {
				if (filenameString.substr(LastDot) == ".rproj") {
					if(OpenProject(path))
						couldLoad = true;
				}
			}
		}
		if (!couldLoad)
			SaveProject();
	}

	Project::~Project()
	{
	}

	void Project::SetSceneToIndex(int index, std::filesystem::path path)
	{
		if (IsSceneAtIndex(index)) {
			m_ScenePaths.at(index) = path;
		}
		else {
			m_ScenePaths.insert(std::make_pair(index, path));
		}
	}

	bool Project::IsSceneAtIndex(int index)
	{
		if (m_ScenePaths.find(index) == m_ScenePaths.end()) {
			return false;
		}

		return true;
	}

	int Project::GetSizeOfSceneIndex()
	{
		int size = 0;
		std::for_each(m_ScenePaths.begin(), m_ScenePaths.end(), [&](std::pair<int, std::filesystem::path> elements) { size += 1; });
		return size;
	}
	void Project::SaveProject()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value << "UntitledProject";

		out << YAML::Key << "Scenes" << YAML::Value << YAML::BeginSeq;
		std::for_each(m_ScenePaths.begin(), m_ScenePaths.end(), [&](std::pair<int, std::filesystem::path> elements) {
			out << YAML::BeginMap;
			out << YAML::Key << "Scene" << YAML::Value << elements.first;
			out << YAML::Key << "Path" << YAML::Value << elements.second.string();
			out << YAML::EndMap;
		});
		out << YAML::EndSeq;

		out << YAML::EndMap;

		std::ofstream fout(m_Path / "UntitledProject.rproj");
		fout << out.c_str();
	}
	bool Project::OpenProject(std::filesystem::path path)
	{
		m_ScenePaths.clear();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException e)
		{
			return false;
		}

		if (!data["Project"])
			return false;

		std::string projectName = data["Project"].as<std::string>();
		RR_CORE_TRACE("Deserializing project '{0}'", projectName);
		m_Path = path.parent_path();

		RR_CORE_TRACE("Deserializing Scene Index");
		auto scenes = data["Scenes"];
		if (scenes)
		{
			for (auto scene : scenes)
			{
				SetSceneToIndex(scene["Scene"].as<int>(), scene["Path"].as<std::string>());
				RR_CORE_TRACE("Scene Index:'{0}' Path:'{1}'", scene["Scene"].as<int>(), scene["Path"].as<std::string>());
			}
		}
	}
}