#include "EditorLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RoseRoot/Utils/PlatformUtils.h"

#include "ImGuizmo.h"
#include "RoseRoot/Math/Math.h"
#include "Core/CommandHistory.h"

namespace Rose {
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		RR_PROFILE_FUNCTION();
		Application::Get().GetWindow().SetWindowIcon("Resources/icon.png");
		CommandHistory::Init();

		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			auto projectFilePath = commandLineArgs[1];
			m_Project = Project(projectFilePath);
		}

		m_SceneManager.m_ContentBrowserPanel.SetAssetPath(m_Project.GetAssetPath());
		m_SceneManager.m_SceneHierarchyPanel.SetAssetPath(m_Project.GetAssetPath());
		m_SceneManager.SetAssetPath(m_Project.GetAssetPath());

		ResetToProjectSettings();
		m_SceneManager.NewScene();
	}

	void EditorLayer::OnDetach()
	{
		RR_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		RR_PROFILE_FUNCTION();

		// Update scene
		m_SceneManager.OnUpdate(ts);

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_SceneManager.m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_SceneManager.getActiveScene().get());
		}

		m_SceneManager.OnOverlayRender();

		m_SceneManager.m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		RR_PROFILE_FUNCTION();

		// Note: Switch this to true to enable dockspace
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 320.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1
				if (ImGui::MenuItem("New Project"))
					NewProject();
				if (ImGui::MenuItem("Open Project"))
					OpenProject();

				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
					m_SceneManager.NewScene();

				if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
					m_SceneManager.OpenScene();

				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					m_SceneManager.SaveScene();

				if (ImGui::MenuItem("Save Scene As", "Ctrl+Shift+S"))
					m_SceneManager.SaveSceneAs();

				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "Ctrl+Z"))
					CommandHistory::Undo();
				if (ImGui::MenuItem("Redo", "Ctrl+Y"))
					CommandHistory::Redo();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Scene Settings", "Ctrl+L"))
					m_SceneSettingsOpen = !m_SceneSettingsOpen;
				if (ImGui::MenuItem("Project Settings", "Ctrl+P"))
					m_ProjectSettingsOpen = !m_ProjectSettingsOpen;
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		ImGui::Begin("Stats");

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();

		m_SceneManager.m_SceneHierarchyPanel.OnImGuiRender();
		m_SceneManager.m_ContentBrowserPanel.OnImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_SceneManager.m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = m_SceneManager.m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_SceneManager.m_ViewportSize.x, m_SceneManager.m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				m_SceneManager.OpenScene(m_Project.GetAssetPath() / path);
			}
			ImGui::EndDragDropTarget();
		}


		// Gizmos
		Entity selectedEntity = m_SceneManager.m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_SceneManager.m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			// Camera

			// Runtime camera from entity
			// auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			// const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			// const glm::mat4& cameraProjection = camera.GetProjection();
			// glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

			// Editor camera
			const glm::mat4& cameraProjection = m_SceneManager.m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_SceneManager.m_EditorCamera.GetViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_SceneManager.m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_SceneManager.m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				if (!m_GizmoLastFrame) {
					m_GizmoLastFrame = true;
					CommandHistory::LockLastCommand();
				}
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				if (translation != tc.Translation) {
					CommandHistory::ChangeVec3(CreateRef<ChangeValueCommand<glm::vec3>>(tc.Translation, translation));
				}
				if (rotation != tc.Rotation) {
					CommandHistory::ChangeVec3(CreateRef < ChangeValueCommand<glm::vec3>>(tc.Rotation,rotation));
				}

				if (scale != tc.Scale) {
					CommandHistory::ChangeVec3(CreateRef<ChangeValueCommand<glm::vec3>>(tc.Scale, scale));
				}
			} else if (m_GizmoLastFrame) {
				m_GizmoLastFrame = false;
				CommandHistory::LockLastCommand();
			}
			
		}


		ImGui::End();
		ImGui::PopStyleVar();

		m_SceneManager.UI_Toolbar();
		if (m_SceneSettingsOpen)
			m_SceneManager.SceneSettingsWindow();
		if (m_ProjectSettingsOpen)
			ProjectSettingsWindow();

		ImGui::End();
	}


	void EditorLayer::OnEvent(Event& e)
	{
		m_SceneManager.m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(RR_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(RR_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool alt = Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::RightAlt);

		switch (e.GetKeyCode())
		{
		case Key::N:
		{
			if (control)
				m_SceneManager.NewScene();

			break;
		}
		case Key::O:
		{
			if (control)
				m_SceneManager.OpenScene();

			break;
		}
		case Key::S:
		{
			if (control && alt)
				m_SceneManager.SaveSceneAs();
			if (control)
				m_SceneManager.SaveScene();

			break;
		}

		case Key::D:
		{
			if (control)
				m_SceneManager.OnDuplicateEntity();

			break;
		}
		case Key::L:
		{
			m_SceneSettingsOpen = !m_SceneSettingsOpen;
			break;
		}
		case Key::P:
		{
			m_ProjectSettingsOpen = !m_ProjectSettingsOpen;
			break;
		}
		case Key::Z:
		{
			if (control)
				CommandHistory::Undo();

			break;
		}
		case Key::Y:
		{
			if (control)
				CommandHistory::Redo();

			break;
		}
		// Gizmos
		case Key::Q:
		{
			if (!ImGuizmo::IsUsing() && m_SceneManager.isEditing())
				m_SceneManager.m_GizmoType = -1;
			break;
		}
		case Key::W:
		{
			if (!ImGuizmo::IsUsing() && m_SceneManager.isEditing())
				m_SceneManager.m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		}
		case Key::E:
		{
			if (!ImGuizmo::IsUsing() && m_SceneManager.isEditing())
				m_SceneManager.m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		}
		case Key::R:
		{
			if (!ImGuizmo::IsUsing() && m_SceneManager.isEditing())
				m_SceneManager.m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}
		
		}
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_SceneManager.m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	

	void EditorLayer::ProjectSettingsWindow()
	{
		ImGui::Begin("Project Settings");

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		std::strncpy(buffer, m_Project.GetName().c_str(), sizeof(buffer));
		if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
		{
			m_Project.SetName(std::string(buffer));
		}

		ImGui::PushItemWidth(70);
		ImGui::InputInt("Number of Scenes", &m_NumberOfScenes, 0, 100);
		ImGui::PopItemWidth();

		if (ImGui::TreeNodeEx("Scene Index"))
		{
			for (int i = 0; i < m_NumberOfScenes; i++) {
				if (m_ScenePathsBuffer.find(i) == m_ScenePathsBuffer.end()) {
					m_ScenePathsBuffer.insert(std::make_pair(i, std::pair<int, std::filesystem::path>(i, "no-scene")));
				}

				ImGui::PushItemWidth(70);
				std::string index = "##Index ";
				index += std::to_string(i);
				ImGui::InputInt(index.c_str(), &m_ScenePathsBuffer.at(i).first, 0, 100);
				ImGui::PopItemWidth();

				ImGui::SameLine();

				std::string  scene = m_ScenePathsBuffer.at(i).second.filename().string();
				ImGui::Button(scene.c_str(), ImVec2(150.0f, 0.0f));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path scenePath = m_Project.GetAssetPath() / path;
						m_ScenePathsBuffer.at(i).second = scenePath;
					}
					ImGui::EndDragDropTarget();
				}
			}

			ImGui::TreePop();
		}

		ImGui::Text("");

		if (ImGui::Button("Save Project Settings", { 200, 30 }))
			SaveProjectSettings();
		

		if (ImGui::Button("Reset To Project Settings", { 200, 21 }))
			ResetToProjectSettings();
		
		ImGui::End();
	}

	void EditorLayer::ResetToProjectSettings()
	{
		m_ScenePathsBuffer.clear();

		m_NumberOfScenes = m_Project.GetSizeOfSceneIndex();

		auto sceneIndexPtr = m_Project.GetSceneIndexPtr();

		int i = 0;
		std::for_each(sceneIndexPtr->begin() , sceneIndexPtr->end(), [&](std::pair<int, std::filesystem::path> elements) {
			m_ScenePathsBuffer.insert(std::make_pair(i, elements));
			i++;
		});
	}

	void EditorLayer::SaveProjectSettings()
	{
		for (int i = 0; i < m_NumberOfScenes; i++) {
			m_Project.SetSceneToIndex(m_ScenePathsBuffer.at(i).first, m_ScenePathsBuffer.at(i).second);
		}

		m_Project.SaveProject();
	}

	void EditorLayer::NewProject()
	{
		//TODO Saftey net for unsaved scenes.
		//SaveScene();
		m_SceneManager.NewScene();
		std::filesystem::path filepath = FileDialogs::SaveFile("Rose Project");
		if (!filepath.empty())
		{
			m_Project = Project(filepath);
			m_SceneManager.m_ContentBrowserPanel.SetAssetPath(m_Project.GetAssetPath());
			m_SceneManager.m_SceneHierarchyPanel.SetAssetPath(m_Project.GetAssetPath());
			m_SceneManager.SetAssetPath(m_Project.GetAssetPath());
		}	
	} 
	void EditorLayer::OpenProject()
	{
		//TODO Saftey net for unsaved scenes.
		//SaveScene();
		m_SceneManager.NewScene();
		std::string filepath = FileDialogs::OpenFile("Rose Project (*.rproj)\0*.rproj\0");
		if (!filepath.empty()) {
			m_Project.OpenProject(filepath);
			m_SceneManager.m_ContentBrowserPanel.SetAssetPath(m_Project.GetAssetPath());
			m_SceneManager.m_SceneHierarchyPanel.SetAssetPath(m_Project.GetAssetPath());
			m_SceneManager.SetAssetPath(m_Project.GetAssetPath());
			ResetToProjectSettings();
		}
	}
}