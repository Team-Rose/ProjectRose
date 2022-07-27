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
	
		Rose::FramebufferSpecification fbSpec;
		fbSpec.Attachments = { Rose::FramebufferTextureFormat::RGBA8, Rose::FramebufferTextureFormat::RED_INTEGER, Rose::FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		fbSpec.Samples = 4;
		m_Framebuffer = Rose::Framebuffer::Create(fbSpec);

		wood = Rose::Texture2D::Create("assets/textures/WoodFloor/WoodFloor051_1K_Color.png");
		//woodspec = Rose::Texture2D::Create("assets/textures/WoodFloor/WoodFloor051_1K_Displacement.png");
		
		crate = Rose::Texture2D::Create("assets/textures/Crate/container2.png");
		cratespec = Rose::Texture2D::Create("assets/textures/Crate/container2_specular.png");
		// 
		//m_Lua.Init();
		//m_Lua2.Init();
	}
	void MainLayer::OnDetach()
	{
	}
	void MainLayer::OnUpdate(Rose::Timestep ts)
	{
		m_EditorCamera.OnUpdate(ts);
	

		Rose::Renderer2D::ResetStats();

		Rose::RenderCommand::SetClearColor({0.1, 0.1, 0.2, 1.0});
		Rose::RenderCommand::Clear();
		Rose::Renderer::BeginScene(m_EditorCamera);

		Rose::Renderer::DrawDirLight({-0.2f, -1.0f, -0.3f});

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), {0.f, -1.0f, 0.f}) * glm::scale(glm::mat4(1.0f), {40.0f, 1.0f,40.0f});
		Rose::Renderer::DrawCube(wood, transform,{ 1.0, 1.0, 1.0, 1.0 });
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 0.f, 0.0f, -1.f }) * glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f,1.0f });
			transform = glm::rotate(transform, glm::radians(90.f), { 1.0f, 0.0f, 0.0f });
			Rose::Renderer::DrawCube(crate, cratespec, transform, 1.0f, { 1.0, 1.0, 1.0, 1.0 });
		}
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 2.f, 0.0f, 1.f }) * glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f,1.0f });
			transform = glm::rotate(transform, glm::radians(90.f), { 1.0f, 0.0f, 0.0f });
			Rose::Renderer::DrawCube(crate, cratespec, transform, 1.0f, { 1.0, 1.0, 1.0, 1.0 });
		}
		//Rose::Renderer2D::DrawQuad({ 0.0f, 2.0f }, { 1.0f, 1.0f }, grass);
		Rose::Renderer::EndScene();
	}
	void MainLayer::OnImGuiRender()
	{
	}
}


