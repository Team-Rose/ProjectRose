#include "ProjectSerializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Rose {
	ProjectSerializer::ProjectSerializer(Ref<Project> project)
		: m_Project(project)
	{

	}
	bool ProjectSerializer::Serialize(const std::filesystem::path& filepath)
	{
		const auto& config = m_Project->GetConfig();

		YAML::Emitter out;
		out << YAML::BeginMap;

		//everytime the project format changes in a breaking way update V#!
		out << YAML::Key << "ProjectV2" << YAML::Value;
		{
			out << YAML::BeginMap;

			out << YAML::Key << "Name" << YAML::Value << config.Name;
			out << YAML::Key << "StartScene" << YAML::Value << config.StartScene.string();
			out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
			out << YAML::Key << "CacheDirectory" << YAML::Value << config.CacheDirectory.string();
			out << YAML::Key << "AssetRegistryPath" << YAML::Value << config.AssetRegistryPath.string();
			out << YAML::Key << "ScriptModulePath" << YAML::Value << config.ScriptModulePath.string();

			out << YAML::EndMap;
		}

		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}
	bool ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		auto& config = m_Project->GetConfig();
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			RR_CORE_ERROR("Failed to load .rproj file'{0}'\n	{1}", filepath, e.what());
			return false;
		}

		if (!data["ProjectV2"])
		{
			if (data["Project"])
				return DeserializeV1(filepath);
		}

		auto projectNode = data["ProjectV2"];

		config.Name = projectNode["Name"].as<std::string>();
		config.StartScene = projectNode["StartScene"].as<std::string>();
		config.AssetDirectory = projectNode["AssetDirectory"].as<std::string>();
		if (projectNode["CacheDirectory"])
			config.CacheDirectory = projectNode["CacheDirectory"].as<std::string>();
		if(projectNode["AssetRegistryPath"])
			config.AssetDirectory = projectNode["AssetRegistryPath"].as<std::string>();

		config.ScriptModulePath = projectNode["ScriptModulePath"].as<std::string>();

		return true;
	}
	bool ProjectSerializer::DeserializeV1(const std::filesystem::path& filepath)
	{
		RR_WARN("Attempting to import V1 project");
		RR_WARN("V1 projects are deprecated you may experience bugs while trying to update");

		auto& config = m_Project->GetConfig();
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			return false;
		}

		if (!data["Project"])
			return false;

		std::string projectName = data["Project"].as<std::string>();
		RR_CORE_TRACE("Deserializing project '{0}'", projectName);
		config.Name = projectName;

		config.AssetDirectory = std::filesystem::path("assets");
		config.CacheDirectory = std::filesystem::path("cache");
		std::string nameCopy = projectName;
		config.ScriptModulePath = std::filesystem::path("Binaries") / nameCopy.append(".dll");

		/*
		RR_CORE_TRACE("Deserializing Scene Index");
		auto scenes = data["Scenes"];
		if (scenes)
		{
			for (auto scene : scenes)
			{
				SetSceneToIndex(scene["Scene"].as<int>(), scene["Path"].as<std::string>());
				RR_CORE_TRACE("Scene Index:'{0}' Path:'{1}'", scene["Scene"].as<int>(), scene["Path"].as<std::string>());
			}
		} */

		return true;
	}
}