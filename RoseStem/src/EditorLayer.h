#pragma once
#include "RoseRoot.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include "RoseRoot/Renderer/EditorCamera.h"

#include <chrono>
#include "Project/Project.h"

namespace Rose {
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep ts) override;
		void OnOverlayRender();

		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		void ProjectSettingsWindow();

		void SceneSettingsWindow();
		void UI_Toolbar();
		void OnDuplicateEntity();

		void OnScenePlay();
		void OnSceneSimululate();
		void OnSceneStop();
		void OnScenePause();

		void NewProject();
		void OpenProjectDialog();
		void OpenProject(const std::filesystem::path& path);
		bool SaveProjectDialog();
		bool SaveProject();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void SaveScene();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		void QuickReloadAppAssembly();

		bool IsEditing() { return m_SceneState == SceneState::Edit; }
		bool IsSimulating() { return m_SceneState == SceneState::Simulate; }
	private:
		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;

		//Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		bool m_SceneSettingsOpen = true;
		bool m_ProjectSettingsOpen = true;

		//Gizmo
		int m_GizmoType = -1;
		bool m_GizmoLastFrame = false;

		EditorCamera m_EditorCamera;
		Entity m_HoveredEntity;

		std::chrono::time_point<std::chrono::high_resolution_clock> m_LastTime = std::chrono::high_resolution_clock::now();

		ProjectConfig m_ProjectConfigBuffer;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2] = {
			{ 0.0f, 0.0f },
			{ 0.0f, 0.0f }
		};

		std::string m_SceneName = "Untitled";
		glm::vec2 m_Gravity = { 0.0, -9.8 };

		bool m_ShowPhysicsColliders = false;
		bool m_IsPaused = false;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};

		SceneState m_SceneState = SceneState::Edit;

		//Editor Resources
		Ref<Texture2D> m_IconPlay, m_IconPause, m_IconSimulate, m_IconStop, m_IconStep;
	};
}
