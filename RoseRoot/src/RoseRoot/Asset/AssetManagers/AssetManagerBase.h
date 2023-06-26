#pragma once

//TODO (Sam): Allow App to define these
#define RR_ASSET_MANAGER_BUILD_EDITOR
#define RR_ASSET_MANAGER_BUILD_RUNTIME

#include "../Asset.h"

#include <map>

namespace Rose {
	using AssetMap = std::map<AssetId, Ref<Asset>>;

	class AssetManagerBase {
	public:
		virtual Ref<Asset> GetAsset(AssetId id) const = 0;
		virtual bool  IsAssetIDValid(AssetId id) const = 0;
		virtual bool  IsAssetLoaded(AssetId id) const = 0;
	};
}