#pragma once
#include "Asset.h"
#include "AssetMetadata.h"

namespace Rose {

	class AssetImporter {
	public:
		static Ref<Asset> ImportAsset(AssetId id, const AssetMetadata& metadata);
	};
}