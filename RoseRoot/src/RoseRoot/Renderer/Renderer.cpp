#include "rrpch.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "UniformBuffer.h"
#include "RoseRoot/Math/Math.h"
namespace Rose
 {
	struct CubeVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		//float TexIndex; This may be added back to optimize in the future
		float TilingFactor;

		// Editor-only
		int EntityID;
	};

	struct RendererData
	{
		Ref<VertexArray> CubeVertexArray;
		Ref<Shader> StandardShader;
		Ref<Texture2D> WhiteTexture;

		struct CameraData
		{
			glm::mat4 ViewProjection;
			glm::vec3 ViewPos;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;

		struct ObjectAndSceneData {
			glm::mat4 Transform;
			//TODO find a better way of doing this wasted memory allocation
			//glm::vec3 PointLightsInRange[10];
			int EntityID;
		};
		ObjectAndSceneData ObjectAndSceneDataBuffer;
		Ref<UniformBuffer> ObjectAndSceneDataUniformBuffer;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		RR_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();

		s_Data.CubeVertexArray = VertexArray::Create();

		float cubeVertices[] = {
			//Position				  //Normal				 //Texcoord
			-0.5f, -0.5f, -0.5f,	  0.0f,  0.0f, -1.0f,	 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,	  0.0f,  0.0f, -1.0f,	 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,	  0.0f,  0.0f, -1.0f,	 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,	  0.0f,  0.0f, -1.0f,	 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,	  0.0f,  0.0f, -1.0f,	 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,	  0.0f,  0.0f, -1.0f,	 0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,	  0.0f,  0.0f,  1.0f,	 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,	  0.0f,  0.0f,  1.0f,	 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,	  0.0f,  0.0f,  1.0f,	 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,	  0.0f,  0.0f,  1.0f,	 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,	  0.0f,  0.0f,  1.0f,	 0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,	  0.0f,  0.0f,  1.0f,	 0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,	 -1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,	 -1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,	 -1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,	 -1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,	 -1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,	 -1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,	  1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,	  1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,	  1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,	  1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,	  1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,	  1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,	  0.0f, -1.0f,  0.0f,	 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,	  0.0f, -1.0f,  0.0f,	 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,	  0.0f, -1.0f,  0.0f,	 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,	  0.0f, -1.0f,  0.0f,	 0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,	  0.0f, -1.0f,  0.0f,	 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,	  0.0f, -1.0f,  0.0f,	 0.0f, 0.0f,

			-0.5f,  0.5f, -0.5f,	  0.0f,  1.0f,  0.0f,	 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,	  0.0f,  1.0f,  0.0f,	 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,	  0.0f,  1.0f,  0.0f,	 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,	  0.0f,  1.0f,  0.0f,	 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,	  0.0f,  1.0f,  0.0f,	 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,	  0.0f,  1.0f,  0.0f,	 0.0f, 0.0f
		};

		Ref<VertexBuffer> cubeVertexBuffer;
		cubeVertexBuffer = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
		cubeVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" }
			});
		s_Data.CubeVertexArray->AddVertexBuffer(cubeVertexBuffer);

		uint32_t cubeIndices[36] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35};
		Ref<IndexBuffer> cubeIndexBuffer;
		cubeIndexBuffer = IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(uint32_t));
		s_Data.CubeVertexArray->SetIndexBuffer(cubeIndexBuffer);

		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		s_Data.StandardShader = Shader::Create("Resources/DefaultShaders/Standard.glsl");

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(RendererData::CameraData), 0);
		s_Data.ObjectAndSceneDataUniformBuffer = UniformBuffer::Create(sizeof(RendererData::ObjectAndSceneData), 1);
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		RR_PROFILE_FUNCTION();
		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);

		glm::vec3 translation, rotation, scale;
		Math::DecomposeTransform(transform, translation, rotation, scale);
		s_Data.CameraBuffer.ViewPos = translation;

		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RendererData::CameraData));

		Renderer2D::BeginScene(camera, transform);

	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		RR_PROFILE_FUNCTION();
		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraBuffer.ViewPos = camera.GetPosition();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RendererData::CameraData));

		Renderer2D::BeginScene(camera);
	}


	void Renderer::EndScene()
	{
		Renderer2D::EndScene();
	}

	void Renderer::DrawCube(const glm::mat4& transform)
	{
		DrawCube(s_Data.StandardShader, transform);
	}

	void Renderer::DrawCube(const Ref<Shader>& shader, const glm::mat4& transform)
	{
		Submit(shader, s_Data.CubeVertexArray, transform);
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		shader->Bind();
		s_Data.WhiteTexture->Bind(0);
		s_Data.ObjectAndSceneDataBuffer.Transform = transform;
		s_Data.ObjectAndSceneDataBuffer.EntityID = entityID;
		s_Data.ObjectAndSceneDataUniformBuffer->SetData(&s_Data.ObjectAndSceneDataBuffer, sizeof(RendererData::ObjectAndSceneData));

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}