#include "SceneManager.h"

#include "RoseRoot/Scene/SceneSerializer.h"
#include "RoseRoot/Utils/PlatformUtils.h"
#include "CommandHistory.h"

#include "imgui/imgui.h"
namespace Rose {
	Rose::SceneManger::SceneManger()
	{
		m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.png");
		m_IconSimulate = Texture2D::Create("Resources/Icons/SimulateButton.png");
		m_IconStop = Texture2D::Create("Resources/Icons/StopButton.png");

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
	}

	Rose::SceneManger::~SceneManger()
	{
	}
	void SceneManger::OnUpdate(Timestep ts)
	{
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
		if(m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
			m_EditorCamera.OnUpdate(ts);

		// Render
		Renderer2D::ResetStats();

		switch (m_SceneState) {
		case SceneState::Edit: m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera); break;
		case SceneState::Simulate: m_ActiveScene->OnUpdateSimulation(ts, m_EditorCamera); break;
		case SceneState::Play: m_ActiveScene->OnUpdateRuntime(ts); break;
		}
	}

	void SceneManger::OnOverlayRender()
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
		Renderer::EndScene();
	}

	void SceneManger::SceneSettingsWindow()
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
	void SceneManger::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_Gravity = m_ActiveScene->GetGravity2D();

		m_EditorScene = m_ActiveScene;
		m_EditorScenePath = std::filesystem::path();
	}

	void SceneManger::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Rose Scene (*.rose)\0*.rose\0");
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void SceneManger::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();
		if (path.extension().string() != ".rose")
		{
			RR_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string(), m_AssetPath.string()))
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

	}

	void SceneManger::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Rose Scene (*.rose)\0*.rose\0");
		if (!filepath.empty())
		{
			SerializeScene(m_ActiveScene, filepath);
			m_EditorScenePath = filepath;
		}
	}

	void SceneManger::SaveScene()
	{
		if (!m_EditorScenePath.empty())
			SerializeScene(m_ActiveScene, m_EditorScenePath);
		else
			SaveSceneAs();
	}

	void SceneManger::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
			return;

		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	void SceneManger::OnScenePlay()
	{
		if (m_SceneState == SceneState::Simulate)
			OnSceneStop();

		if (std::filesystem::exists(m_AppAssemblyPath)) {
			MonoScriptEngine::ReloadAppAssembly(m_AppAssemblyPath);
		}
		else {
			MonoScriptEngine::UnloadAppAssembly();
		}
		m_GizmoType = -1;
		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart(m_AssetPath.string());

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_SceneState = SceneState::Play;
	}
	void SceneManger::OnSceneSimululate()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_GizmoType = -1;
		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart(m_AssetPath.string());

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_SceneState = SceneState::Simulate;
	}

	void SceneManger::OnSceneStop()
	{
		RR_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
		if(m_SceneState == SceneState::Play)
			m_ActiveScene->OnRuntimeStop();
		else
			m_ActiveScene->OnSimulationStop();

		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_SceneState = SceneState::Edit;
	}

	void SceneManger::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 4.0f;
		{
			Ref<Texture2D> icon = (m_SceneState ==  SceneState::Edit || m_SceneState ==  SceneState::Simulate) ? m_IconPlay : m_IconStop;
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
					OnScenePlay();
				else if (m_SceneState == SceneState::Play)
					OnSceneStop();
			}
		}
		ImGui::SameLine();
		{
			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
			//ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
					OnSceneSimululate();
				else if (m_SceneState == SceneState::Simulate)
					OnSceneStop();
			}
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}
	void SceneManger::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		if (m_SceneHierarchyPanel.GetSelectedEntity())
			m_SceneHierarchyPanel.SetSelectedEntity(m_EditorScene->DuplicateEntity(m_SceneHierarchyPanel.GetSelectedEntity()));
	}
}
