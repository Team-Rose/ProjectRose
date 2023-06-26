#include "rrpch.h"
#include "Asset.h"

namespace Rose {
    std::string AssetTypeToString(AssetType type)
    {
        switch (type) {
        case AssetType::None: return "None";
        case AssetType::Scene: return "Scene";
        case AssetType::Texture2D: return "Texture2D";
        }
        return "<Invalid>";
    }
    AssetType AssetTypeFromString(const std::string& assetType)
    {
        if (assetType == "None") return AssetType::None;
        if (assetType == "Scene") return AssetType::Scene;
        if (assetType == "Texture2D") return AssetType::Texture2D;

        return AssetType::None;
    }
}

