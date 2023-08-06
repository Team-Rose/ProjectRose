#include "rrpch.h"
#include "EditorAssetManager.h"
#ifdef RR_ASSET_MANAGER_BUILD_EDITOR
#include <yaml-cpp/yaml.h>

#include "../AssetImporter.h"

namespace Rose {
	static std::map<std::filesystem::path, AssetType> s_AssetExtensionMap = {
		{ ".rose", AssetType::Scene },
		{ ".png", AssetType::Texture2D },
		{ ".jpg", AssetType::Texture2D },
		{ ".jpeg", AssetType::Texture2D }
	};

	static AssetType GetTypeFromFileExtension(const std::filesystem::path& extension)
	{
		if (s_AssetExtensionMap.find(extension) == s_AssetExtensionMap.end())
		{
			RR_CORE_WARN("Could not find AssetTYpe for {}", extension);
			return AssetType::None;
		}
		return s_AssetExtensionMap.at(extension);
	}

	Ref<Asset> EditorAssetManager::GetAsset(AssetId id)
	{
		if (!IsAssetIdValid(id))
			return nullptr;

		if (IsAssetLoaded(id))
			return m_LoadedAssets.at(id);
	
		const AssetMetadata& metadata = GetMetadata(id);


		Ref<Asset> asset = AssetImporter::ImportAsset(id, metadata);
		if (asset) {
			m_LoadedAssets[id] = asset;
			
		} else
			RR_CORE_ERROR("EditorAssetManager::GetAsset - import failed!");

		return asset;
	}
	bool EditorAssetManager::IsAssetIdValid(AssetId id) const
	{
		//TODO C++20 switch to .contains
		return m_AssetRegistry.find(id) != m_AssetRegistry.end();
	}
	bool EditorAssetManager::IsAssetLoaded(AssetId id) const
	{
		return m_LoadedAssets.find(id) != m_LoadedAssets.end();
	}
	AssetType EditorAssetManager::GetAssetType(AssetId id) const
	{
		if (!IsAssetIdValid(id))
			return AssetType::None;
		return m_AssetRegistry.at(id).Type;
	}

	void EditorAssetManager::ImportAsset(const std::filesystem::path& path)
	{
		AssetId id;
		AssetMetadata metadata;
		metadata.FilePath = path;
		metadata.Type = GetTypeFromFileExtension(path.extension());

		Ref<Asset> asset = AssetImporter::ImportAsset(id, metadata);
		if (asset)
		{
			asset->Id = id;
			m_LoadedAssets[id] = asset;
			m_AssetRegistry[id] = metadata;
		}
	}
	const AssetMetadata& EditorAssetManager::GetMetadata(AssetId id) const
	{
		static AssetMetadata s_EmptyMetadata;
		auto it = m_AssetRegistry.find(id);
		if (it == m_AssetRegistry.end())
			return s_EmptyMetadata;
		return it->second;
	}

	void EditorAssetManager::SerializeAssetRegistry(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;

		out << YAML::BeginMap; // Root
		out << YAML::Key << "AssetRegistry" << YAML::Value;
		{
			out << YAML::BeginSeq;
			for (const auto& [id, metadata] : m_AssetRegistry)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Id" << YAML::Value << id;
				std::string filepathStr = metadata.FilePath.generic_string();
				out << YAML::Key << "FilePath" << YAML::Value << filepathStr;
				out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;

		}
		out << YAML::EndMap; // Root

		std::ofstream fout(filepath);
		fout << out.c_str();
	}
	bool EditorAssetManager::DeserializeAssetRegistry(const std::filesystem::path& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			RR_CORE_ERROR("Failed to load .rareg file '{0}'\n	{1}", filepath, e.what());
			return false;
		}

		auto rootNode = data["AssetRegistry"];

		if (!rootNode)
			return false;
		
		for (const auto& node : rootNode) {
			AssetId id = node["Id"].as<uint64_t>();
			auto& matadata = m_AssetRegistry[id];
			matadata.FilePath = node["FilePath"].as<std::string>();
			matadata.Type = AssetTypeFromString(node["Type"].as<std::string>());
		}
		return true;
	}
}
#endif