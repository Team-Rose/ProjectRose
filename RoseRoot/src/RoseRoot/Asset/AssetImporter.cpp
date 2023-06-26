#include "rrpch.h"
#include "AssetImporter.h"

#include "Importers/TextureImporter.h"

namespace Rose {
	Ref<Asset> AssetImporter::ImportAsset(AssetId id, const AssetMetadata& metadata)
	{
		switch (metadata.Type)
		{
		case AssetType::Texture2D: return Import::Texture2D(id, metadata);
		}

		RR_CORE_ERROR("No importer found for asset type!: {}", AssetTypeToString(metadata.Type));
		return nullptr;
	}
}