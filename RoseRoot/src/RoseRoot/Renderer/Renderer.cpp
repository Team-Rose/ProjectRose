#include "rrpch.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "RendererVoxel.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Rose
 {

	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

	void Renderer::Init()
	{
		RR_PROFILE_FUNCTION();

		RenderCommand::Init();
		RendererVoxel::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
		RendererVoxel::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(PerspectiveCamera& camera)
	{
		RR_CORE_WARN("Beginning a scene in default renderer is unsuppported please use Renderer2D!");

		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->SetMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}