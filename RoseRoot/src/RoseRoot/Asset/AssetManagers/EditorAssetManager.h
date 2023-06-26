#pragma once
#include "AssetManagerBase.h"

#ifdef RR_ASSET_MANAGER_BUILD_EDITOR

#include <map>
#include "../AssetMetadata.h"
namespace Rose {

	using AssetRegistry = std::map<AssetId, AssetMetadata>;

	class EditorAssetManager : public AssetManagerBase {
	public:
		virtual Ref<Asset> GetAsset(AssetId id) const override;

		virtual bool  IsAssetIDValid(AssetId id) const override;
		virtual bool  IsAssetLoaded(AssetId id) const override;

		void ImportAsset(const std::filesystem::path& path);

		const AssetMetadata& GetMetadata(AssetId id) const;
		const AssetRegistry& GetAssetRegistry() const { return m_AssetRegistry; }
		void SerializeAssetRegistry(const std::filesystem::path& filepath);
		bool DeserializeAssetRegistry(const std::filesystem::path& filepath);
	private:
		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;
	};
}
#endif