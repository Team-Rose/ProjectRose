#pragma once
#include <RoseRoot/Renderer/Texture.h>
#include "AssetManagerHolder.h"

namespace Rose {
	class AssetManager {
	public:
		// static API class

		template<typename T>
		static Ref<T> GetAsset(AssetId id)
		{
			Ref<Asset> asset = AssetManagerHolder::GetActiveHolder()->GetAssetManager()->GetAsset(id);
			return std::static_pointer_cast<T>(asset);
		}

		static bool  IsAssetIdValid(AssetId id) {
			return AssetManagerHolder::GetActiveHolder()->GetAssetManager()->IsAssetIdValid(id);
		}
		static bool  IsAssetLoaded(AssetId id) {
			return AssetManagerHolder::GetActiveHolder()->GetAssetManager()->IsAssetLoaded(id);
		}
		static AssetType GetAssetType(AssetId id) {
			return AssetManagerHolder::GetActiveHolder()->GetAssetManager()->GetAssetType(id);
		}
	};
#if 0
	class AssetManager {
	public:
		static void Init();
		static void ShutDown();
		static void UnloadAssets();
		static void ReloadAssets();

		static void SetAssetPath(const std::string& path);
		static const std::string& GetAssetPath();

		static bool LoadTexture(const std::string& path);
		static Ref<Texture2D> GetTexture(const std::string& path);
		static Ref<Texture2D> GetOrLoadTexture(const std::string& path);
	};
#endif
}