#include "rrpch.h"
#include "AssetManager.h"
#include <RoseRoot/Scene/Scene.h>

namespace Rose {
	struct AssetManagerData
	{
		std::string AssetPath;
		std::unordered_map<std::string, Ref<Texture2D>> Textures;

		std::unordered_map<std::filesystem::path, Ref<Model>> Models;
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
		ReloadAssets();
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
	bool AssetManager::LoadModel(const std::filesystem::path& path)
	{
		Ref<Model> model = CreateRef<Model>();
		if (!model->ReadFile(path))
			return false;
		s_Data.Models[path] = model;
		return true;
	}
	Ref<Model> AssetManager::GetModel(const std::filesystem::path& path)
	{
		auto& it = s_Data.Models.find(path);
		if (it != s_Data.Models.end())
			return it->second;
		return nullptr;
	}
	Ref<Model> AssetManager::GetOrLoadModel(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
			return nullptr;

		Ref<Model> model = GetModel(path);
		if (!model) {
			bool load = LoadModel(path);
			if (!load)
				return nullptr;
			return GetModel(path);
		}

		return model;
	}
}