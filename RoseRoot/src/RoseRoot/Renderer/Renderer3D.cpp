#include "rrpch.h"
#include "Renderer3D.h"

#include "UniformBuffer.h"
#include "RoseRoot/Math/Math.h"

namespace Rose
{
	struct DirLight {
		glm::vec3 direction;
		float pad;

		glm::vec3 ambient;
		float pad1;
		glm::vec3 diffuse;
		float pad2;
		glm::vec3 specular;
		float pad3;
	};

	struct PointLight {
		glm::vec3 position;

		float constant;
		float linear;
		float quadratic;
		float pad0;
		float pad1;

		glm::vec3 ambient;
		float pad2;
		glm::vec3 diffuse;
		float pad3;
		glm::vec3 specular;
		float pad4;
	};

	struct SpotLight {
		glm::vec3 position;
		float pad;
		glm::vec3 direction;
		float pad2;

		float cutOff;
		float outerCutOff;

		float constant;
		float linear;

		float quadratic;

		glm::vec3 ambient;
		float pad3;
		glm::vec3 diffuse;
		float pad4;
		glm::vec3 specular;
		float pad5;
	};

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
	};

	struct RendererData
	{
		Ref<VertexArray> CubeVertexArray;
		Ref<Shader> StandardShader;
		Ref<VertexArray> FrameBufferVertexArray;
		Ref<Shader> FrameBufferShader;
		Ref<Texture2D> WhiteTexture;

		struct CameraData
		{
			glm::mat4 ViewProjection;
			glm::vec3 ViewPos;
			float pad;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;

		struct ObjectAndSceneData {
			glm::mat4 Transform;
			glm::vec4 Color;

			float Tile;
			int EntityID;
		};

		ObjectAndSceneData ObjectAndSceneDataBuffer;
		Ref<UniformBuffer> ObjectAndSceneDataUniformBuffer;

		struct LightData {
			DirLight DirLight;
			PointLight PointLight;
			SpotLight SpotLight;
		};
		LightData LightDataBuffer;
		Ref<UniformBuffer> LightDataUniformBuffer;
	};

	static RendererData s_Data;

	void Renderer3D::Init()
	{
		RR_PROFILE_FUNCTION();

		RenderCommand::Init();
		{
			Vertex cubeVertices[] = {
				//Position				  //Normal				 //Texcoord

				// Back face
				{{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f},  {0.0f, 0.0f}},
				{{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f},  {1.0f, 0.0f}},
				{{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f},  {1.0f, 1.0f}},
				{{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f},  {1.0f, 1.0f}},
				{{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f},  {0.0f, 1.0f}},
				{{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f},  {0.0f, 0.0f}},

				// Front face
				{{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f},  { 0.0f, 0.0f}},
				{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f},  { 1.0f, 1.0f}},
				{{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f},  { 1.0f, 0.0f}},
				{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f},  { 1.0f, 1.0f}},
				{{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f},  { 0.0f, 0.0f}},
				{{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f},  { 0.0f, 1.0f}},

				// Left face
				{{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}},
				{{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}},
				{{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f},  {1.0f, 1.0f}},
				{{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}},
				{{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}},
				{{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f},  {0.0f, 0.0f}},

				// Right face
				{{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}},
				{{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f},  {1.0f, 1.0f}},
				{{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}},
				{{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}},
				{{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f},  {0.0f, 0.0f}},
				{{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}},

				// Bottom face 
				{{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f},  {0.0f, 1.0f}},
				{{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f},  {1.0f, 0.0f}},
				{{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f},  {1.0f, 1.0f}},
				{{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f},  {1.0f, 0.0f}},
				{{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f},  {0.0f, 1.0f}},
				{{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f},  {0.0f, 0.0f}},

				// Top face
				{{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f},  {0.0f, 1.0f}},
				{{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f},  {1.0f, 1.0f}},
				{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f},  {1.0f, 0.0f}},
				{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f},  {1.0f, 0.0f}},
				{{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f},  {0.0f, 0.0f}},
				{{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f},  {0.0f, 1.0f}},
			};

			uint32_t cubeIndices[36] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35 };

			s_Data.CubeVertexArray = VertexArray::Create();
			Ref<VertexBuffer> cubeVertexBuffer = VertexBuffer::Create(sizeof(cubeVertices));
			cubeVertexBuffer->SetData(cubeVertices, sizeof(cubeVertices));
			cubeVertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float3, "a_Normal" },
				{ ShaderDataType::Float2, "a_TexCoord" }
				});
			s_Data.CubeVertexArray->AddVertexBuffer(cubeVertexBuffer);

			Ref<IndexBuffer> cubeIndexBuffer;
			cubeIndexBuffer = IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(uint32_t));
			s_Data.CubeVertexArray->SetIndexBuffer(cubeIndexBuffer);
		}
		
		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		s_Data.StandardShader = Shader::Create("Resources/DefaultShaders/Standard.glsl");

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(RendererData::CameraData), 0);
		s_Data.ObjectAndSceneDataUniformBuffer = UniformBuffer::Create(sizeof(RendererData::ObjectAndSceneData), 1);
		s_Data.LightDataUniformBuffer = UniformBuffer::Create(sizeof(RendererData::LightData), 2);
	}

	void Renderer3D::Shutdown()
	{
	}

	void Renderer3D::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		RR_PROFILE_FUNCTION();
		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);

		glm::vec3 translation, rotation, scale;
		Math::DecomposeTransform(transform, translation, rotation, scale);
		s_Data.CameraBuffer.ViewPos = translation;

		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RendererData::CameraData));

		//RenderCommand::CullFaceDisabled();

	}

	void Renderer3D::BeginScene(const EditorCamera& camera)
	{
		RR_PROFILE_FUNCTION();
		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraBuffer.ViewPos = camera.GetPosition();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RendererData::CameraData));
	}


	void Renderer3D::EndScene()
	{
		ClearLightData();
	}

	void Renderer3D::ClearLightData()
	{
		s_Data.LightDataBuffer.DirLight.direction = { 0.0f,0.0f,0.0f };
		s_Data.LightDataBuffer.DirLight.ambient = { 0.0f,0.0f,0.0f };
		s_Data.LightDataBuffer.DirLight.diffuse = { 0.0f,0.0f,0.0f };
		s_Data.LightDataBuffer.DirLight.specular = { 0.0f,0.0f,0.0f };

		s_Data.LightDataBuffer.PointLight.position = { 1.0f,4.0f,0.0f };


		s_Data.LightDataBuffer.PointLight.constant = 1.0f;
		s_Data.LightDataBuffer.PointLight.linear = 0.09f;
		s_Data.LightDataBuffer.PointLight.quadratic = 0.032f;

		s_Data.LightDataBuffer.PointLight.ambient = { 0.05f, 0.05f, 0.05f };
		s_Data.LightDataBuffer.PointLight.diffuse = { 0.2f, 0.2f, 0.6f };
		s_Data.LightDataBuffer.PointLight.specular = { 0.2f, 0.2f, 0.6f };



		s_Data.LightDataBuffer.SpotLight.position = { 0.0f,0.0f,0.0f };
		s_Data.LightDataBuffer.SpotLight.direction = { 0.0f,0.0f,0.0f };

		s_Data.LightDataBuffer.SpotLight.cutOff = 0.0f;
		s_Data.LightDataBuffer.SpotLight.outerCutOff = 0.0f;

		s_Data.LightDataBuffer.SpotLight.constant = 1.0f;
		s_Data.LightDataBuffer.SpotLight.linear = 0.0f;
		s_Data.LightDataBuffer.SpotLight.quadratic = 0.0f;

		s_Data.LightDataBuffer.SpotLight.ambient = { 0.0f,0.0f,0.0f };
		s_Data.LightDataBuffer.SpotLight.diffuse = { 0.0f,0.0f,0.0f };
		s_Data.LightDataBuffer.SpotLight.specular = { 0.0f,0.0f,0.0f };
	}

	void Renderer3D::DrawDirLight(const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
	{
		s_Data.LightDataBuffer.DirLight.direction = direction;
		s_Data.LightDataBuffer.DirLight.ambient = ambient;
		s_Data.LightDataBuffer.DirLight.diffuse = diffuse;
		s_Data.LightDataBuffer.DirLight.specular = specular;
	}

	void Renderer3D::DrawCube(const glm::mat4& transform, const glm::vec4& color)
	{
		s_Data.WhiteTexture->Bind(0);
		s_Data.WhiteTexture->Bind(1);
		Submit(s_Data.StandardShader, s_Data.CubeVertexArray, transform, color);
	}

	void Renderer3D::DrawCube(const Ref<Texture2D>& texure, const glm::mat4& transform, const glm::vec4& color)
	{
		texure->Bind(0);
		s_Data.WhiteTexture->Bind(1);
		s_Data.ObjectAndSceneDataBuffer.Tile = 1.0f;
		Submit(s_Data.StandardShader, s_Data.CubeVertexArray, transform, color);
	}

	void Renderer3D::DrawCube(const Ref<Texture2D>& texure, const Ref<Texture2D>& spec, const glm::mat4& transform, const float& tile, const glm::vec4& color)
	{
		texure->Bind(0);
		spec->Bind(1);
		s_Data.ObjectAndSceneDataBuffer.Tile = tile;
		Submit(s_Data.StandardShader, s_Data.CubeVertexArray, transform, color);
	}

	void Renderer3D::DrawCube(const Ref<Shader>& shader, const glm::mat4& transform)
	{
		s_Data.WhiteTexture->Bind(0);
		s_Data.WhiteTexture->Bind(1);
		Submit(shader, s_Data.CubeVertexArray, transform);
	}

	void Renderer3D::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		RenderCommand::CullFaceBack();
		shader->Bind();


		s_Data.ObjectAndSceneDataBuffer.Transform = transform;
		s_Data.ObjectAndSceneDataBuffer.EntityID = entityID;
		s_Data.ObjectAndSceneDataBuffer.Color = color;
		s_Data.ObjectAndSceneDataUniformBuffer->SetData(&s_Data.ObjectAndSceneDataBuffer, sizeof(RendererData::ObjectAndSceneData));
		s_Data.LightDataUniformBuffer->SetData(&s_Data.LightDataBuffer, sizeof(RendererData::LightData));

		struct ShaderProps {
			float shininess = 32.0f;
			float gamma = 1.8f;
		};
		ShaderProps ShaderPropsBuffer;
		Ref<UniformBuffer> ShaderPropsUniformBuffer;
		ShaderPropsUniformBuffer = UniformBuffer::Create(sizeof(ShaderProps), 3);
		ShaderPropsUniformBuffer->SetData(&ShaderPropsBuffer, sizeof(ShaderProps));

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}