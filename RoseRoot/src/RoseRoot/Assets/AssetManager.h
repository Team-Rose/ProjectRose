#pragma once
#include <RoseRoot/Renderer/Texture.h>

namespace Rose {
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
}