#pragma once
#include <RoseRoot/Renderer/Texture.h>
#include <RoseRoot/Renderer/Mesh.h>
#include "Model.h"

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

		static bool LoadModel(const std::filesystem::path& path);
		static Ref<Model> GetModel(const std::filesystem::path& path);
		static Ref<Model> GetOrLoadModel(const std::filesystem::path& path);
	};
}