#include "rrpch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

namespace Rose {

	ContentBrowserPanel::ContentBrowserPanel(std::filesystem::path path)
		: m_AssetPath(path)
	{
		m_CurrentDirectory = m_AssetPath;
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
		m_LuaIcon = Texture2D::Create("Resources/Icons/ContentBrowser/LuaIcon.png");
		m_SceneIcon = Texture2D::Create("Resources/Icons/ContentBrowser/SceneIcon.png");
		m_PictureIcon = Texture2D::Create("Resources/Icons/ContentBrowser/PictureIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != m_AssetPath)
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 8.0f;
		static float thumbnailSize = 100.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, m_AssetPath);
			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());
			Ref<Texture2D> icon;
	
			size_t LastDot = filenameString.find_last_of(".");

			if (LastDot != std::string::npos) {
				if (filenameString.substr(LastDot) == ".lua")
					icon = m_LuaIcon;
				else if (filenameString.substr(LastDot) == ".rose")
					icon = m_SceneIcon;
				else if (filenameString.substr(LastDot) == ".png" || filenameString.substr(LastDot) == ".jpeg")
					icon = m_PictureIcon;
				else
					icon = m_FileIcon;
			} else
				icon = m_FileIcon;
			if (directoryEntry.is_directory())
				icon = m_DirectoryIcon;

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();

			}
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();

			ImGui::PopID();

		}

		ImGui::Columns(1);

		//ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		//ImGui::SliderFloat("Padding", &padding, 0, 32);

		// TODO: status bar
		ImGui::End();
	}

}