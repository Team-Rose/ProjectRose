#include "MainLayer.h"

namespace Sandbox {
	MainLayer::MainLayer()
		: Layer("MainLayer"), m_TestMesh("Resources\\american_house.glb")
	{

	}
	void MainLayer::OnAttach()
	{
		m_EditorCamera = 
			Rose::EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
	
		wood = Rose::Texture2D::Create("assets/textures/WoodFloor/WoodFloor051_1K_Color.png");
		
		crate = Rose::Texture2D::Create("assets/textures/Crate/container2.png");
		cratespec = Rose::Texture2D::Create("assets/textures/Crate/container2_specular.png");
	}
	void MainLayer::OnDetach()
	{
	}
	void MainLayer::OnUpdate(Rose::Timestep ts)
	{
		m_EditorCamera.SetViewportSize(Rose::Application::Get().GetWindow().GetWidth(), Rose::Application::Get().GetWindow().GetHeight());
		m_EditorCamera.OnUpdate(ts);
	

		Rose::Renderer2D::ResetStats();

		Rose::Renderer::BeginScene(m_EditorCamera);
		Rose::RenderCommand::SetClearColor({0.3, 0.3, 0.7, 1.0});
		Rose::RenderCommand::Clear();

		Rose::Renderer3D::DrawDirLight({-0.2f, -1.0f, -0.3f});
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 0.f, 0.0f, 0.f }) * glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f, 1.0f });
			Rose::Renderer3D::DrawMesh(m_TestMesh, transform);
		}
		/*
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 0.f, -1.0f, 0.f }) * glm::scale(glm::mat4(1.0f), { 40.0f, 1.0f,40.0f });
			Rose::Renderer3D::DrawCube(wood, transform, 20.0f, { 1.0, 1.0, 1.0, 1.0 });
		}
		

		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 0.f, 0.0f, -1.f }) * glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f,1.0f });
			transform = glm::rotate(transform, glm::radians(90.f), { 1.0f, 0.1f, 0.0f });
			Rose::Renderer3D::DrawCube(crate, cratespec, transform, 1.0f, { 1.0, 1.0, 1.0, 1.0 });
		}
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 2.f, 0.0f, 1.f }) * glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f,1.0f });
			transform = glm::rotate(transform, glm::radians(90.f), { 1.0f, 0.0f, 0.0f });
			Rose::Renderer3D::DrawCube(crate, cratespec, transform, 1.0f, { 1.0, 1.0, 1.0, 1.0 });
		}

		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 1.0f,3.0f,0.0f }) * glm::scale(glm::mat4(1.0f), { 0.25f, 0.25f,0.25f });
			Rose::Renderer3D::DrawCube(transform, { 1.0f, 1.0f, 0.4f, 0.6f });
		} */
		//Rose::Renderer2D::DrawQuad({ 0.0f, 2.0f }, { 1.0f, 1.0f }, crate);
		Rose::Renderer::EndScene();
		Rose::Renderer::DrawFinalFrameBuffer();
	}
	void MainLayer::OnImGuiRender()
	{
	}
}


