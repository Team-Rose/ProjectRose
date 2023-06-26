#pragma once
#include <RoseRoot/Core/UUID.h>

namespace Rose {
	using AssetId = UUID;

	enum class AssetType : uint16_t
	{
		None = 0,
		Scene, Texture2D
	};

	std::string AssetTypeToString(AssetType type);
	AssetType AssetTypeFromString(const std::string& assetType);

	class Asset {
	public:
		AssetId Id;

		virtual AssetType GetType() const = 0;
	};
}