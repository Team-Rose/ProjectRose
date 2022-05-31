#include "MainLayer.h"

namespace Sandbox {
	MainLayer::MainLayer()
		: Layer("MainLayer")
	{

	}
	void MainLayer::OnAttach()
	{
		m_EditorCamera = 
			Rose::EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
	
		cursed = Rose::Texture2D::Create("assets/textures/Stone.png");
		frog = Rose::Texture2D::Create("assets/textures/GrassBlockTop.png");
		//m_Lua.Init();
		//m_Lua2.Init();
	}
	void MainLayer::OnDetach()
	{
	}
	void MainLayer::OnUpdate(Rose::Timestep ts)
	{
		m_EditorCamera.OnUpdate(ts);

		Rose::RenderCommand::SetClearColor({0.1, 0.1, 0.2, 1.0});
		Rose::RenderCommand::Clear();
		Rose::Renderer::BeginScene(m_EditorCamera);

		Rose::Renderer::DrawDirLight({-0.2f, -1.0f, -0.3f});

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 4.f, -1.f, 0.f }) * glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f,1.0f });
		Rose::Renderer::DrawCube(cursed, transform, { 1.0, 1.0, 1.0, 1.0 });

		glm::mat4 transform2 = glm::translate(glm::mat4(1.0f), { 0.f, 0.f, 0.f }) * glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f,1.0f });
		transform2 = glm::rotate(transform2, glm::radians(45.f), { 0.0f, 1.0f, 1.0f });
		Rose::Renderer::DrawCube(frog, transform2, { 1.0, 1.0, 1.0, 1.0 });

		Rose::Renderer2D::DrawQuad({ 0.0f, 2.0f }, { 1.0f, 1.0f }, frog);
		Rose::Renderer::EndScene();

	}
	void MainLayer::OnImGuiRender()
	{
	}
}


