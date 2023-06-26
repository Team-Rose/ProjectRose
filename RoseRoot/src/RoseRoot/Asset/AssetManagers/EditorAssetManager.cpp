#include "rrpch.h"
#include "EditorAssetManager.h"
#ifdef RR_ASSET_MANAGER_BUILD_EDITOR
#include <yaml-cpp/yaml.h>

#include "../AssetImporter.h"

namespace Rose {
	Ref<Asset> EditorAssetManager::GetAsset(AssetId id) const
	{
		if (!IsAssetIDValid(id))
			return nullptr;

		if (IsAssetLoaded(id))
			return m_LoadedAssets.at(id);
	
		const AssetMetadata& metadata = GetMetadata(id);


		Ref<Asset> asset = AssetImporter::ImportAsset(id, metadata);
		if (!asset) {
			// import failed
			RR_CORE_ERROR("EditorAssetManager::GetAsset - import failed!");
		}

		return asset;
	}
	bool EditorAssetManager::IsAssetIDValid(AssetId id) const
	{
		//TODO C++20 switch to .contains
		return m_AssetRegistry.find(id) != m_AssetRegistry.end();
	}
	bool EditorAssetManager::IsAssetLoaded(AssetId id) const
	{
		return m_LoadedAssets.find(id) != m_LoadedAssets.end();
	}
	void EditorAssetManager::ImportAsset(const std::filesystem::path& path)
	{
		AssetId id;
		AssetMetadata metadata;
		metadata.FilePath = path;
		metadata.Type = AssetType::Texture2D; // TODO(Sam): Grab this from extensions and try to load it
		Ref<Asset> asset = AssetImporter::ImportAsset(id, metadata);
		asset->Id = id;
		if (asset)
		{
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