#include "EditorLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RoseRoot/Utils/PlatformUtils.h"

#include "ImGuizmo.h"
#include "RoseRoot/Math/Math.h"
#include "Core/CommandHistory.h"
#include <Box2D/include/box2d/b2_body.h>
#include <RoseRoot/Scene/SceneSerializer.h>
#include "RoseRoot/Renderer/Font.h"

namespace Rose {
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
		Font font("C:\\Windows\\fonts\\Arial.ttf");
	}

	void EditorLayer::OnAttach()
	{
		RR_PROFILE_FUNCTION();
		Application::Get().GetWindow().SetWindowIcon("Resources/icon.png");
		m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.png");
		m_IconPause = Texture2D::Create("Resources/Icons/PauseButton.png");
		m_IconSimulate = Texture2D::Create("Resources/Icons/SimulateButton.png");
		m_IconStop = Texture2D::Create("Resources/Icons/StopButton.png");
		m_IconStep = Texture2D::Create("Resources/Icons/StepButton.png");

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		CommandHistory::Init();

		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			auto projectFilePath = commandLineArgs[1];
			OpenProject(projectFilePath);

			if (commandLineArgs.Count > 2) {
				auto sceneFilePath = commandLineArgs[2];
				OpenScene(sceneFilePath);
			}
		}
		else {
			std::filesystem::path path = "startup-project/startup-project.rproj";
			if (std::filesystem::exists(path)) {
				OpenProject(path);
			}
			else {
				NewProject();
			}

			NewScene();
		}
	}

	void EditorLayer::OnDetach()
	{
		RR_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		RR_PROFILE_FUNCTION();

		// Update scene
		// Resize
		if (FramebufferSpecification spec = Renderer::GetFinalFrameBuffer()->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			Renderer::ResizeView((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			if (m_SceneState != SceneState::Edit)
				m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Update
		if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
			m_EditorCamera.OnUpdate(ts);

		// Render
		Renderer2D::ResetStats();

		switch (m_SceneState) {
		case SceneState::Edit: m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera); break;
		case SceneState::Simulate: m_ActiveScene->OnUpdateSimulation(ts, m_EditorCamera, m_IsPaused); break;
		case SceneState::Play: m_ActiveScene->OnUpdateRuntime(ts, m_IsPaused); break;
		}


		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			Renderer::GetFinalFrameBuffer()->Bind();
			int pixelData = Renderer::GetFinalFrameBuffer()->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
			//RR_INFO(pixelData);
			Renderer::GetFinalFrameBuffer()->Unbind();
		}

		OnOverlayRender();
	}
	void EditorLayer::OnOverlayRender()
	{
		if (m_SceneState == SceneState::Play)
		{
			Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
			if (camera) {
				Renderer::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
			}

		}
		else
		{
			Renderer::BeginScene(m_EditorCamera);
		}

		if (m_ShowPhysicsColliders)
		{
			if (m_ShowPhysicsColliders)
			{
				// Box Colliders
				{
					auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
					for (auto entity : view)
					{
						auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

						glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);
						glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

						glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
							* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
							* glm::scale(glm::mat4(1.0f), scale);

						Renderer2D::DrawRect(transform, glm::vec4(0.2f, 0.2f, 1, 1));
					}
				}

				// Circle Colliders
				{
					auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
					for (auto entity : view)
					{
						auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

						glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, 0.001f);
						glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

						glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
							* glm::scale(glm::mat4(1.0f), scale);

						Renderer2D::DrawCircle(transform, glm::vec4(0.2f, 0.2f, 1, 1), 0.05f);
					}
				}
			}

		}
		if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) {
			// Draw selected entity outline 
			if (Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity())
			{
				const TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
				Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(0.65, 0.039, 0.576, 1.0f));
			}
		}

		Renderer::EndScene();
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
					OpenProjectDialog();

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
					NewScene();
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					SaveScene();

				if (ImGui::MenuItem("Save Scene As", "Ctrl+Shift+S"))
					SaveSceneAs();

				ImGui::Separator();

				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "Ctrl+Z"))
					CommandHistory::Undo();
				if (ImGui::MenuItem("Redo", "Ctrl+Y"))
					CommandHistory::Redo();

				if (ImGui::MenuItem("Reload App Assembly", "Ctrl+R")) {
					QuickReloadAppAssembly();
				}

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
		std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();

		float duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - m_LastTime).count() * 0.001f * 0.001f * 0.001f;;
		ImGui::Text("Frame Time: %.3fms", duration*1000.0f);
		m_LastTime = currentTime;

		SceneStats sceneStats = m_ActiveScene->GetSceneStats();
		ImGui::Text("Scene Stats:");
		ImGui::Text("Mono Script Time: %.3fms", sceneStats.MonoScriptTime);
		ImGui::Text("Lua Script Time: %.3fms", sceneStats.LuaSciptTime);
		ImGui::Text("Total Script Time: %.3fms", sceneStats.TotalScriptTime);

		ImGui::Text("Physics Time: %.3fms", sceneStats.PhysicsTime);
		ImGui::Text("Render Time: %.3fms", sceneStats.RenderTime);


		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();

		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel.OnImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = Renderer::GetFinalFrameBuffer()->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(Project::GetActiveAssetDirectory() / path);
			}
			ImGui::EndDragDropTarget();
		}


		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
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
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
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
					CommandHistory::ChangeVec3(CreateRef<ChangeValueCommand<glm::vec3>>(tc.Rotation,rotation));
				}

				if (scale != tc.Scale) {
					CommandHistory::ChangeVec3(CreateRef<ChangeValueCommand<glm::vec3>>(tc.Scale, scale));
				}

				if (!IsEditing()) {
					if (selectedEntity.HasComponent<Rigidbody2DComponent>()) {
						auto& rb2d = selectedEntity.GetComponent<Rigidbody2DComponent>();

						b2Body* body = (b2Body*)rb2d.RuntimeBody;
						body->SetAwake(false);
						body->SetTransform({tc.Translation.x, tc.Translation.y}, tc.Rotation.z);
						body->SetAwake(true);
					}
				}

				

			} else if (m_GizmoLastFrame) {
				m_GizmoLastFrame = false;
				CommandHistory::LockLastCommand();
			}
			
		}


		ImGui::End();
		ImGui::PopStyleVar();

		UI_Toolbar();
		if (m_SceneSettingsOpen)
			SceneSettingsWindow();
		if (m_ProjectSettingsOpen)
			ProjectSettingsWindow();

		ImGui::End();
	}


	void EditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);

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
				NewScene();

			break;
		}
		case Key::O:
		{
			if (control)
				OpenScene();

			break;
		}
		case Key::S:
		{
			if (control && alt)
				SaveSceneAs();
			if (control)
				SaveScene();

			break;
		}

		case Key::D:
		{
			if (control)
				OnDuplicateEntity();

			break;
		}
		case Key::Delete:
		{
			if (Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0) {
				Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
				if (selectedEntity) {
					m_SceneHierarchyPanel.SetSelectedEntity({});
					m_ActiveScene->DestroyEntity(selectedEntity);
				}
			}
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
			if (!ImGuizmo::IsUsing() && IsEditing() || IsSimulating())
				m_GizmoType = -1;
			break;
		}
		case Key::W:
		{
			if (!ImGuizmo::IsUsing() && IsEditing() || IsSimulating())
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		}
		case Key::E:
		{
			if (control) {
				
			} else {
				if (!ImGuizmo::IsUsing() && IsEditing() || IsSimulating())
					m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			}
			break;
		}
		case Key::R:
		{
			if(control)
				QuickReloadAppAssembly();
			else if (!ImGuizmo::IsUsing() && IsEditing() || IsSimulating())
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}
		
		}
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	void EditorLayer::ProjectSettingsWindow()
	{
		ImGui::Begin("Project Settings");

		{
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, m_ProjectConfigBuffer.Name.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
			{
				m_ProjectConfigBuffer.Name = std::string(buffer);
			}
		}
		ImGui::Separator();
		/*
		{
			ImGui::Text(m_ProjectConfigBuffer.AssetDirectory.string().c_str());
			if (ImGui::Button("Set Asset Parth")) {
				std::string filepath = FileDialogs::OpenFile("All Files\0*.*\0\0");
				if (!filepath.empty()) {
					RR_INFO(filepath.c_str());
					//OpenProject(filepath);
				}
			}
		}
		ImGui::Separator(); */

		ImGui::NewLine();
		if (ImGui::Button("Save Project Settings", { 200, 30 })) {
			Project::GetActive()->GetConfig() = m_ProjectConfigBuffer;
			SaveProject();
		}
			
		

		if (ImGui::Button("Reset To Project Settings", { 200, 21 }))
			m_ProjectConfigBuffer = Project::GetActive()->GetConfig();
		
		ImGui::End();
	}

	void EditorLayer::SceneSettingsWindow()
	{
		ImGui::Begin("Scene Settings");

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		std::strncpy(buffer, m_SceneName.c_str(), sizeof(buffer));
		if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
		{
			m_SceneName = std::string(buffer);
			m_EditorScene->SetName(m_SceneName);
		}

		if (ImGui::TreeNodeEx("Physics2D"))
		{


			if (ImGui::DragFloat2("Gravity 2D", glm::value_ptr(m_Gravity)))
			{
				if (m_SceneState == SceneState::Edit)
					m_EditorScene->SetGravity2D(m_Gravity);
			}
			ImGui::TreePop();
		}

		ImGui::Checkbox("Show Colliders", &m_ShowPhysicsColliders);
		ImGui::End();
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 4.0f;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		//Play Button
		if (m_SceneState != SceneState::Simulate)
		{
			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;
			if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
					OnScenePlay();
				else if (m_SceneState == SceneState::Play)
					OnSceneStop();
			}
		}

		//Simulate Button
		if (m_SceneState != SceneState::Play)
		{
			if (m_SceneState != SceneState::Simulate)
				ImGui::SameLine();

			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
			if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
					OnSceneSimululate();
				else if (m_SceneState == SceneState::Simulate)
					OnSceneStop();
			}
		}

		//Pause Button
		if (m_SceneState != SceneState::Edit)
		{
			ImGui::SameLine();

			Ref<Texture2D> icon = !m_IsPaused ? m_IconPause : m_IconPlay;
			if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				OnScenePause();
			}

			if (m_IsPaused) {
				ImGui::SameLine();

				Ref<Texture2D> icon = m_IconStep;
				if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				{
					m_ActiveScene->Step();
				}
			}
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}
	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		if (m_SceneHierarchyPanel.GetSelectedEntity())
			m_SceneHierarchyPanel.SetSelectedEntity(m_EditorScene->DuplicateEntity(m_SceneHierarchyPanel.GetSelectedEntity()));
	}

	void EditorLayer::OnScenePlay()
	{
		if (m_SceneState == SceneState::Simulate)
			OnSceneStop();
		m_IsPaused = false;
		/*if (std::filesystem::exists(m_AppAssemblyPath)) {
			MonoScriptEngine::ReloadAppAssembly(m_AppAssemblyPath);
		}
		else {
			MonoScriptEngine::UnloadAppAssembly();
		}*/
		m_GizmoType = -1;
		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart(Project::GetActiveAssetDirectory().string());

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_SceneState = SceneState::Play;
	}
	void EditorLayer::OnSceneSimululate()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();
		m_IsPaused = false;

		m_GizmoType = -1;
		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart(Project::GetActiveAssetDirectory().string());

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_SceneState = SceneState::Simulate;
	}

	void EditorLayer::OnSceneStop()
	{
		RR_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
			if (m_SceneState == SceneState::Play)
				m_ActiveScene->OnRuntimeStop();
			else
				m_ActiveScene->OnSimulationStop();

		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_SceneState = SceneState::Edit;
	}

	void EditorLayer::OnScenePause()
	{
		if (m_SceneState == SceneState::Edit)
			return;

		m_IsPaused = !m_IsPaused;
	}

	void EditorLayer::NewProject()
	{
		//TODO Saftey net for unsaved scenes.
		//SaveScene();
		NewScene();
		Project::New();
		m_ProjectConfigBuffer = Project::GetActive()->GetConfig();
	} 
	void EditorLayer::OpenProject(const std::filesystem::path& path)
	{
		//TODO Saftey net for unsaved scenes.
		//SaveScene();
		NewScene();
		if (!Project::Load(path)) {
			RR_ERROR("Failed to load project");
			NewProject();
			return;
		}
		AssetManager::SetAssetPath(Project::GetActiveAssetDirectory().string());
		m_ProjectConfigBuffer = Project::GetActive()->GetConfig();
		QuickReloadAppAssembly();
		const std::filesystem::path assetPath = Project::GetActiveAssetDirectory();
		auto startScenePath = assetPath / Project::GetActive()->GetConfig().StartScene;
		OpenScene(startScenePath);
	}
	void EditorLayer::OpenProjectDialog()
	{
		//TODO Saftey net for unsaved scenes.
		//SaveScene();
		NewScene();
		std::string filepath = FileDialogs::OpenFile("Rose Project (*.rproj)\0*.rproj\0");
		if (!filepath.empty()) {
			OpenProject(filepath);
		}
	}
	bool EditorLayer::SaveProjectDialog()
	{
		//TODO Saftey net for unsaved scenes.
		//SaveScene();
		NewScene();
		std::filesystem::path filepath = FileDialogs::SaveFile("Rose Project");
		if (!filepath.empty())
		{
			Project::SaveActive(filepath);
			AssetManager::SetAssetPath(Project::GetActiveAssetDirectory().string());
		}
		return true;
	}
	bool EditorLayer::SaveProject()
	{
		if (std::filesystem::exists(Project::GetActiveProjectDirectory())) {
			return SaveProjectDialog();
		}
		else {
			Project::SaveActive(Project::GetActiveProjectDirectory());
			return true;
		}
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_Gravity = m_ActiveScene->GetGravity2D();

		m_EditorScene = m_ActiveScene;
		m_EditorScenePath = std::filesystem::path();
	}
	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Rose Scene (*.rose)\0*.rose\0");
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();
		if (path.extension().string() != ".rose")
		{
			RR_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		Ref<Scene> newScene = CreateRef<Scene>();
		AssetManager::UnloadAssets();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string(), Project::GetActiveAssetDirectory().string()))
		{
			CommandHistory::Clear();
			m_EditorScene = newScene;
			m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneName = m_EditorScene->GetName();
			m_Gravity = m_EditorScene->GetGravity2D();
			m_SceneHierarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = path;
		}
		else {
			RR_WARN("Could not deserialize {}", path.string());
		}

	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Rose Scene (*.rose)\0*.rose\0");
		if (!filepath.empty())
		{
			SerializeScene(m_ActiveScene, filepath);
			m_EditorScenePath = filepath;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorScenePath.empty())
			SerializeScene(m_ActiveScene, m_EditorScenePath);
		else
			SaveSceneAs();
	}

	void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
			return;

		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	void EditorLayer::QuickReloadAppAssembly()
	{
		if (std::filesystem::exists(Project::GetActiveProjectDirectory() / Project::GetActive()->GetConfig().ScriptModulePath)) {
			MonoScriptEngine::ReloadAppAssembly(Project::GetActiveProjectDirectory() / Project::GetActive()->GetConfig().ScriptModulePath);
		}
		else {
			RR_WARN("(ignore this if you aren't using C#) No Script Module found! : {}", Project::GetActive()->GetConfig().ScriptModulePath);
			MonoScriptEngine::UnloadAppAssembly();
		}
	}
}