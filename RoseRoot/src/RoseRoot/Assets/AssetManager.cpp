#include "rrpch.h"
#include "AssetManager.h"

namespace Rose {
	struct AssetManagerData
	{
		std::string AssetPath;
		std::unordered_map<std::string, Ref<Texture2D>> Textures;
	};

	static AssetManagerData s_Data;

	void AssetManager::Init()
	{
		
	}
	void AssetManager::ShutDown()
	{
		s_Data = AssetManagerData{};
	}
	void AssetManager::UnloadAssets()
	{
		s_Data.Textures.clear();
	}
	void AssetManager::ReloadAssets()
	{
		for (auto& [path, texture] : s_Data.Textures) {
			if(!LoadTexture(path))
				s_Data.Textures.erase(path);
		}
	}
	void AssetManager::SetAssetPath(const std::string& path)
	{
		s_Data.AssetPath = path;
	}
	const std::string& AssetManager::GetAssetPath()
	{
		return s_Data.AssetPath;
	}

	bool AssetManager::LoadTexture(const std::string& path)
	{
		Ref<Texture2D> loadedtexture = Texture2D::Create(path);
		if (!loadedtexture->IsLoaded())
			return false;

		s_Data.Textures[path] = loadedtexture;
		return true;
	}
	Ref<Texture2D> AssetManager::GetTexture(const std::string& path)
	{
		auto& it = s_Data.Textures.find(path);
		if (it != s_Data.Textures.end())
			return it->second;
		return nullptr;
	}
	Ref<Texture2D> AssetManager::GetOrLoadTexture(const std::string& path)
	{
		if (!std::filesystem::exists(path))
			return nullptr;

		Ref<Texture2D> loadedtexture = GetTexture(path);
		if (!loadedtexture) {
			bool load = LoadTexture(path);
			if (!load)
				return nullptr;
			return GetTexture(path);
		}

		return loadedtexture;
	}
}