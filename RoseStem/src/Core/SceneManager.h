#pragma once
#include "RoseRoot.h"
#include "../Panels/SceneHierarchyPanel.h"
#include "../Panels/ContentBrowserPanel.h"

namespace Rose {
	

	class SceneManger
	{
	public:
		SceneManger();
		~SceneManger();

		void OnUpdate(Timestep ts);
		void OnOverlayRender();
		void SceneSettingsWindow();
		
		Ref<Scene> getActiveScene() { return m_ActiveScene; }
		bool isEditing() { return m_SceneState == SceneState::Edit; }

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void SaveScene();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnSceneStop();

		void UI_Toolbar();
		void OnDuplicateEntity();

		void SetAssetPath(const std::filesystem::path& path) { m_AssetPath = path; }
	public:
		
		//Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		Ref<Framebuffer> m_Framebuffer;
		EditorCamera m_EditorCamera;

		int m_GizmoType = -1;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	private:
		std::filesystem::path m_AssetPath;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;

		std::string m_SceneName = "Untitled";
		glm::vec2 m_Gravity = { 0.0, -9.8 };

		bool m_ShowPhysicsColliders = false;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};

		SceneState m_SceneState = SceneState::Edit;

		//Editor Resources
		Ref<Texture2D> m_IconPlay, m_IconStop;
	};
}