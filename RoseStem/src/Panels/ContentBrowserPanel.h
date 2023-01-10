#pragma once

#include <filesystem>

#include "RoseRoot/Renderer/Texture.h"

namespace Rose {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path m_AssetDirectory;
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
		Ref<Texture2D> m_LuaIcon;
		Ref<Texture2D> m_CSIcon;
		Ref<Texture2D> m_SceneIcon;
		Ref<Texture2D> m_PictureIcon;
	};
}