#include "rrpch.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "Renderer3D.h"
#include "UniformBuffer.h"
#include "RoseRoot/Math/Math.h"

namespace Rose
{
	struct QuadVertex
	{
		glm::vec2 Position;
		glm::vec2 TexCoord;
	};

	struct RendererData {
		Ref<Framebuffer> CompPass;
		Ref<Framebuffer> GeometryPass;
		Ref<Framebuffer> ShadowPass;

		Ref<Shader> CompositeShader;
		Ref<Shader> FinalPassShader;
		Ref<VertexArray> FrameBufferVertexArray;

		uint32_t* TextureData;
		bool CustomViewSize;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		RR_PROFILE_FUNCTION();
		{
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
			fbSpec.Width = 1600;
			fbSpec.Height = 900;
			fbSpec.Samples = 4;
			s_Data.GeometryPass = Framebuffer::Create(fbSpec);
		}
		{
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER };
			fbSpec.Width = 1600;
			fbSpec.Height = 900;
			fbSpec.Samples = 1;
			s_Data.CompPass = Framebuffer::Create(fbSpec);
		}

		{
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = { FramebufferTextureFormat::Depth };
			fbSpec.Width = 1024;
			fbSpec.Height = 1024;
			fbSpec.Samples = 1;
			s_Data.ShadowPass = Framebuffer::Create(fbSpec);
		}

		float frameBufferVertices[] = {
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};

		uint32_t frameBufferIndices[6] = { 0,1,2,3,4,5 };

		s_Data.FrameBufferVertexArray = VertexArray::Create();
		Ref<VertexBuffer> frameBufferVertexBuffer = VertexBuffer::Create(sizeof(frameBufferVertices));
		frameBufferVertexBuffer->SetData(frameBufferVertices, sizeof(frameBufferVertices));
		frameBufferVertexBuffer->SetLayout({
			{ ShaderDataType::Float2, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
			});
		s_Data.FrameBufferVertexArray->AddVertexBuffer(frameBufferVertexBuffer);

		Ref<IndexBuffer> frameBufferIndexBuffer;
		frameBufferIndexBuffer = IndexBuffer::Create(frameBufferIndices, sizeof(frameBufferIndices) / sizeof(uint32_t));
		s_Data.FrameBufferVertexArray->SetIndexBuffer(frameBufferIndexBuffer);

		s_Data.CompositeShader = Shader::Create("Resources/DefaultShaders/Composites.glsl"); 
		s_Data.FinalPassShader = Shader::Create("Resources/DefaultShaders/DrawTexture.glsl");

		RenderCommand::Init();
		Renderer2D::Init();
		Renderer3D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
		Renderer3D::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);

		if(!s_Data.CustomViewSize)
			OnViewResized(width, height);
	}

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		RR_PROFILE_FUNCTION();

		s_Data.GeometryPass->Bind();

		Renderer2D::BeginScene(camera, transform);
		Renderer3D::BeginScene(camera, transform);

	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		RR_PROFILE_FUNCTION();

		Renderer2D::BeginScene(camera);
		Renderer3D::BeginScene(camera);
		s_Data.GeometryPass->Bind();
	}


	void Renderer::EndScene()
	{
		Renderer2D::EndScene();
		Renderer3D::EndScene();
		s_Data.GeometryPass->Unbind();
		
		{
			s_Data.CompPass->Bind();

			struct ViewportProps {
				int samples = 4;
			};
			ViewportProps ViewportPropsBuffer;
			ViewportPropsBuffer.samples = s_Data.GeometryPass->GetSpecification().Samples;

			Ref<UniformBuffer> ViewportPropsUniformBuffer;
			ViewportPropsUniformBuffer = UniformBuffer::Create(sizeof(ViewportProps), 32);
			ViewportPropsUniformBuffer->SetData(&ViewportPropsBuffer, sizeof(ViewportProps));

			RenderCommand::DisableDepthTest();
			s_Data.GeometryPass->BindTexture(0, 0);
			s_Data.GeometryPass->BindTexture(1, 1);
			s_Data.CompositeShader->Bind();
			s_Data.FrameBufferVertexArray->Bind();
			RenderCommand::DrawIndexed(s_Data.FrameBufferVertexArray);
			RenderCommand::EnableDepthTest();

			s_Data.CompPass->Unbind();
		}
	}
	void Renderer::DrawFinalFrameBuffer()
	{
		RenderCommand::DisableDepthTest();
		s_Data.CompPass->BindTexture();
		s_Data.FinalPassShader->Bind();
		s_Data.FrameBufferVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.FrameBufferVertexArray);
		RenderCommand::EnableDepthTest();
	}
	Ref<Framebuffer> Renderer::GetFinalFrameBuffer()
	{
		return s_Data.CompPass;
	}
	Ref<Framebuffer> Renderer::GetGeometryFrameBuffer()
	{
		return s_Data.GeometryPass;
	}
	Ref<Framebuffer> Renderer::GetShadowFrameBuffer()
	{
		return s_Data.ShadowPass;
	}
	void Renderer::ResizeView(uint32_t width, uint32_t height)
	{
		s_Data.CustomViewSize = true;
		OnViewResized(width, height);

	}
	void Renderer::OnViewResized(uint32_t width, uint32_t height)
	{
		s_Data.GeometryPass->Resize(width, height);
		s_Data.CompPass->Resize(width, height);
	}
}