#pragma once
#include "RoseRoot.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Core/SceneManager.h"

#include "RoseRoot/Renderer/EditorCamera.h"

namespace Rose {
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		void ProjectSettingsWindow();

		void ResetToProjectSettings();
		void SaveProjectSettings();

		void NewProject();
		void OpenProject();
	private:
		bool m_GizmoLastFrame;
		Project m_Project;
		SceneManger m_SceneManager;

		Entity m_HoveredEntity;
		bool m_SceneSettingsOpen = true;
		bool m_ProjectSettingsOpen = true;

		//Project Buffer
		int m_NumberOfScenes = 0;
		std::unordered_map<int, std::pair<int, std::filesystem::path>> m_ScenePathsBuffer;

		Ref<Texture2D> m_SpriteSheet, m_ViewTest;
		Ref<SubTexture2D> m_GrassTexture, m_StoneTexture, m_GlassTexture;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportBounds[2];
	};
}
