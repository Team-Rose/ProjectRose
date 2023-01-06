#pragma once
#include "RoseRoot.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Core/SceneManager.h"

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
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		void ProjectSettingsWindow();

		void NewProject();
		void OpenProjectDialog();
		void OpenProject(const std::filesystem::path& path);
		bool SaveProjectDialog();
		bool SaveProject();

		void QuickReloadAppAssembly();
	private:
		bool m_GizmoLastFrame;
		std::string m_ProjectName;
		SceneManger m_SceneManager;

		Entity m_HoveredEntity;
		bool m_SceneSettingsOpen = true;
		bool m_ProjectSettingsOpen = true;

		std::chrono::time_point<std::chrono::high_resolution_clock> m_LastTime = std::chrono::high_resolution_clock::now();

		Ref<Texture2D> m_SpriteSheet, m_ViewTest;
		Ref<SubTexture2D> m_GrassTexture, m_StoneTexture, m_GlassTexture;
		ProjectConfig m_ProjectSettingsBuffer;
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportBounds[2];
	};
}
