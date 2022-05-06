#include "MainLayer.h"

namespace Sandbox {
	MainLayer::MainLayer()
		: Layer("MainLayer")
	{

	}
	void MainLayer::OnAttach()
	{
		m_EditorCamera = 
			EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

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
		Rose::Renderer2D::BeginScene(m_EditorCamera);
		//m_Lua.Update(ts);
		//m_Lua2.Update(ts);
		Rose::Renderer2D::EndScene();
	}
	void MainLayer::OnImGuiRender()
	{
	}
}


