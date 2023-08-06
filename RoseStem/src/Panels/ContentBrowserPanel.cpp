#include "rrpch.h"
#include "ContentBrowserPanel.h"
#include "../Project/Project.h"
#include "RoseRoot/Asset/Importers/TextureImporter.h"

#include <imgui/imgui.h>

namespace Rose {
	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_AssetDirectory = Project::GetActiveAssetDirectory();
		m_CurrentDirectory = Project::GetActiveAssetDirectory();

		m_TreeNodes.push_back(TreeNode("." , 0));

		m_DirectoryIcon = Import::Texture2D("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Import::Texture2D("Resources/Icons/ContentBrowser/FileIcon.png");
		m_LuaIcon = Import::Texture2D("Resources/Icons/ContentBrowser/LuaIcon.png");
		m_CSIcon = Import::Texture2D("Resources/Icons/ContentBrowser/C#Icon.png");
		m_SceneIcon = Import::Texture2D("Resources/Icons/ContentBrowser/SceneIcon.png");
		m_PictureIcon = Import::Texture2D("Resources/Icons/ContentBrowser/PictureIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");
		
		if (m_AssetDirectory != Project::GetActiveAssetDirectory()) {
			m_AssetDirectory = Project::GetActiveAssetDirectory();
			m_CurrentDirectory = m_AssetDirectory;
			RefreshAssetTree();
		}
		if (!std::filesystem::exists(m_AssetDirectory))
		{
			ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "No Valid Asset Directory Found.");
			ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "	Set a valid asset directory in project settings");
			ImGui::NewLine();
			ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), m_AssetDirectory.string().c_str());
			ImGui::End();
			return;
		}

		const char* label = m_Mode == Mode::Asset ? "Asset" : "File";
		if (ImGui::Button(label))
			m_Mode = m_Mode == Mode::Asset ? Mode::FileSystem : Mode::Asset;
		if (m_CurrentDirectory != Project::GetActiveAssetDirectory())
		{
			ImGui::SameLine();
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

		if (m_Mode == Mode::Asset) {
			TreeNode* node = &m_TreeNodes[0];

			auto currentDir = std::filesystem::relative(m_CurrentDirectory, Project::GetActiveAssetDirectory());
			for (const auto& p : currentDir)
			{
				if (node->Path == currentDir)
					break;

				if (node->Children.find(p) != node->Children.end())
				{
					node = &m_TreeNodes[node->Children[p]];
					continue;
				}
				else
				{
					m_CurrentDirectory = m_CurrentDirectory.parent_path();
					//RR_ASSERT(false)
				}


			}

			for (const auto& [item, index] : node->Children)
			{
				DrawFileThumbnail(Project::GetActiveAssetDirectory() / item, std::filesystem::is_directory(Project::GetActiveAssetDirectory() / item), thumbnailSize, true, index);
			}
		}
		else {

			TreeNode* directoryNode = &m_TreeNodes[0];
			bool directoryNodeFound = false;

			auto currentDir = std::filesystem::relative(m_CurrentDirectory, Project::GetActiveAssetDirectory());
			for (const auto& p : currentDir)
			{
				if (directoryNode->Path == currentDir)
				{
					directoryNodeFound = true;
					break;
				}
				if (directoryNode->Children.find(p) != directoryNode->Children.end())
				{
					directoryNodeFound = true;
					directoryNode = &m_TreeNodes[directoryNode->Children[p]];
					continue;
				}
			}

			for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				auto path = directoryEntry.path();

				bool imported = false;
				uint32_t nodeIndex = 0;
				if (directoryNodeFound)
				{
					for (const auto& [item, index] : directoryNode->Children)
					{
						if (path == m_CurrentDirectory / item) {
							nodeIndex = index;
							imported = true;
							break;
						}
					}
				}

				DrawFileThumbnail (path, directoryEntry.is_directory(), thumbnailSize, imported, nodeIndex);

			}
		}
		ImGui::Columns(1);

		//ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		//ImGui::SliderFloat("Padding", &padding, 0, 32);

		// TODO: status bar
		ImGui::End();
	}
	void ContentBrowserPanel::DrawFileThumbnail(const std::filesystem::path& path, bool isDirectory, float size, bool imported, uint32_t index)
	{
		// TODO move this somewhere more central

		auto relativePath = std::filesystem::relative(path, Project::GetActiveAssetDirectory());
		std::string filenameString = relativePath.filename().string();
		auto extension = relativePath.extension();

		bool isAsset = false;
		if (extension == ".png" || extension == ".jpeg" || extension == ".rose") {
			isAsset = true;
		}


		ImGui::PushID(filenameString.c_str());
		Ref<Texture2D> icon;

		size_t LastDot = filenameString.find_last_of(".");
		ImVec4 color = ImVec4(1, 1, 1, 1);
		if (isAsset && !imported)
		{
			color = ImVec4(1, 0.7, 0.7, 1);
		}

		if (isDirectory)
			icon = m_DirectoryIcon;
		else
		{
			if (LastDot != std::string::npos) {
				if (extension == ".lua")
					icon = m_LuaIcon;
				else if (extension == ".cs")
					icon = m_CSIcon;
				else if (extension == ".rose")
					icon = m_SceneIcon;
				else if (extension == ".png" || extension == ".jpeg")
					icon = m_PictureIcon;
				else
					icon = m_FileIcon;
			}
			else
				icon = m_FileIcon;
		}

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

		ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { size, size }, { 0, 1 }, { 1, 0 }, -1, ImVec4(0, 0, 0, 0), color);

		if (ImGui::BeginPopupContextItem())
		{
			if (isAsset) {
				if (!imported) {
					if (ImGui::MenuItem("Import"))
					{
						Project::GetActive()->ImportAsset(relativePath);
						RefreshAssetTree();
					}
				}
				else {
					if (ImGui::MenuItem("Unimport"))
					{
						RR_WARN("Unimporting assets is not implmented!");
						RR_ASSERT(false);
					}
				}
				
			}

			if (ImGui::MenuItem("Delete"))
			{
				RR_WARN("Deleting assets is not implmented!");
				RR_ASSERT(false);
			}

			ImGui::EndPopup();
		}

		if (!isDirectory && imported) {
			if (ImGui::BeginDragDropSource())
			{
				AssetId id = m_TreeNodes.at(index).Id;
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ASSET", &id, sizeof(AssetId));
				ImGui::EndDragDropSource();
			}
		}
		else {
			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();;
			}
		}

		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (isDirectory)
				m_CurrentDirectory /= path.filename();

		}
		ImGui::TextWrapped(filenameString.c_str());

		ImGui::NextColumn();

		ImGui::PopID();
	}

	void ContentBrowserPanel::RefreshAssetTree()
	{
		const auto& assetRegistry = Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();
		for (const auto& [id, metadata] : assetRegistry)
		{
			uint32_t currentNodeIndex = 0;

			for (const auto& p : metadata.FilePath)
			{
				auto it = m_TreeNodes[currentNodeIndex].Children.find(p.generic_string());
				if (it != m_TreeNodes[currentNodeIndex].Children.end())
				{
					currentNodeIndex = it->second;
				}
				else
				{
					// add node
					TreeNode newNode(p, id);
					newNode.Parent = currentNodeIndex;
					m_TreeNodes.push_back(newNode);

					m_TreeNodes[currentNodeIndex].Children[p] = m_TreeNodes.size() - 1;
					currentNodeIndex = m_TreeNodes.size() - 1;
				}
			}
		}
	}

}