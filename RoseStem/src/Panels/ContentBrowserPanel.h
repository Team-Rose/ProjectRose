#pragma once

#include <filesystem>

#include "RoseRoot/Renderer/Texture.h"

namespace Rose {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel(std::filesystem::path path = "startup-project");

		void OnImGuiRender();
		
		void SetAssetPath(std::filesystem::path path) { m_AssetPath = path; m_CurrentDirectory = m_AssetPath; }
	private:
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_AssetPath;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
		Ref<Texture2D> m_LuaIcon;
		Ref<Texture2D> m_SceneIcon;
		Ref<Texture2D> m_PictureIcon;
	};
}