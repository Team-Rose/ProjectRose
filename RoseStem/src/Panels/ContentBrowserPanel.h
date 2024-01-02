#pragma once

#include <filesystem>

#include "RoseRoot/Renderer/Texture.h"
#include "Core/ThumbnailCache.h"

#include <set>
#include <map>

namespace Rose {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		void DrawFileThumbnail(const std::filesystem::path& path, bool isDirectory, float size = 100.0f, bool imported = true, uint32_t index = 0);
		void RefreshAssetTree();
	private:
		std::filesystem::path m_AssetDirectory;
		std::filesystem::path m_CurrentDirectory;
		Ref<ThumbnailCache> m_ThumbnailCache;

		struct TreeNode
		{
			std::filesystem::path Path;
			AssetId Id = 0;

			uint32_t Parent = (uint32_t) - 1;
			std::map<std::filesystem::path, uint32_t> Children;

			TreeNode(const std::filesystem::path& path, AssetId id)
				: Path(path), Id(id) {}
		};
		std::vector<TreeNode> m_TreeNodes;

		enum class Mode
		{
			FileSystem = 0, Asset = 1
		};
		Mode m_Mode = Mode::FileSystem;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
		Ref<Texture2D> m_LuaIcon;
		Ref<Texture2D> m_CSIcon;
		Ref<Texture2D> m_SceneIcon;
		Ref<Texture2D> m_PictureIcon;
	};
}