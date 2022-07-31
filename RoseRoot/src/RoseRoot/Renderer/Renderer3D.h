#pragma once

#include "RenderCommand.h"
#include "RoseRoot/Renderer/Camera.h"
#include "RoseRoot/Renderer/EditorCamera.h"
#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace Rose {

	class Renderer3D {
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void EndScene();

		static void ClearLightData();
		static void DrawDirLight(const glm::vec3& direction, const glm::vec3& ambient = glm::vec3(0.05f, 0.05f, 0.05f), const glm::vec3& diffuse = glm::vec3(0.1f, 0.1f, 0.1f), const glm::vec3& specular = glm::vec3(0.1f, 0.1f, 0.1f));

		static void DrawCube(const glm::mat4& transform = glm::mat4(1.0f), const glm::vec4& color = glm::vec4(1.0f));

		static void DrawCube(const Ref<Texture2D>& texure, const glm::mat4& transform = glm::mat4(1.0f), const glm::vec4& color = glm::vec4(1.0f));
		static void DrawCube(const Ref<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f));
		static void DrawCube(const Ref<Texture2D>& texure, const Ref<Texture2D>& spec, const glm::mat4& transform = glm::mat4(1.0f), const float& tile = 1.0f, const glm::vec4& color = glm::vec4(1.0f));

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f), const glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
	};
}