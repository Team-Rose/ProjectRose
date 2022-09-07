#pragma once
#include <RoseRoot/Renderer/Texture.h>

namespace Rose {
	class AssetManager {
	public:
		static void Init();
		static void ShutDown();
		static void CollectUnused();
		static void ReloadAssets();

		static bool LoadTexture(const std::string& path);
		static Ref<Texture2D> GetTexture(const std::string& path);
		static Ref<Texture2D> GetOrLoadTexture(const std::string& path);
	};
}