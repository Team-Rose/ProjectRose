#pragma once
#include <RoseRoot/Renderer/Texture.h>

//TODO (Sam): Allow App to define these
#ifndef RR_DIST
#define RR_ASSET_MANAGER_BUILD_EDITOR
#endif // !RR_DIST
#ifdef RR_DIST
#define RR_ASSET_MANAGER_BUILD_RUNTIME
#endif // RR_DIST

namespace Rose {
	

	class AssetManager {
	public:
		// IMPORTANT Must be called in app construction in order to use most features!
		static void SetAssetManager(const Ref<AssetManager>& assetManager);
		static Ref<AssetManager> GetAssetManager();

		// Only should be called during shutdown
		static void DeleteAssetManagerRef();
	public:
		AssetManager() = default;
		virtual ~AssetManager() = default;


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