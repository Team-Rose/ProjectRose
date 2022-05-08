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

		//m_Lua.Init();
		//m_Lua2.Init();
	}
	void MainLayer::OnDetach()
	{
	}
	void MainLayer::OnUpdate(Rose::Timestep ts)
	{
		m_EditorCamera.OnUpdate(ts);

		Rose::RenderCommand::SetClearColor({0.2, 0.0, 0.2, 1.0});
		Rose::RenderCommand::Clear();
		Rose::Renderer::BeginScene(m_EditorCamera);
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 0.f, 0.f, 0.f }) * glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f,1.0f });
		Rose::Renderer::DrawCube(transform);
		Rose::Renderer::EndScene();
	}
	void MainLayer::OnImGuiRender()
	{
	}
}


