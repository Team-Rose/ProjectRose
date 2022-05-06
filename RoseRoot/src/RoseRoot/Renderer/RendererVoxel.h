#pragma once

#include "Camera.h"
#include "Texture.h"
#include "RoseRoot/Renderer/SubTexture2D.h"

namespace Rose
 {

	class RendererVoxel
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const PerspectiveCamera& camera);
		static void EndScene();
		static void Flush();

		// Primitives
		static void DrawQuad(const glm::vec3& position, const glm::vec3& scale, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec3& scale, const glm::vec4& color, const Ref<Texture2D>& texture, float tiles = 1);
		static void DrawQuad(const glm::vec3& position, const glm::vec3& scale, const glm::vec4& color, const Ref<SubTexture2D>& subTexture, float tiles = 1);

		static void DrawQuadRotated(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation, const glm::vec4& color, const Ref<Texture2D>& texture, float tiles = 1.f);
		static void DrawQuadRotated(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation, const glm::vec4& color, const Ref<SubTexture2D>& subTexture, float tiles = 1.f);

		//stats
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 8; }\
		};
		static void ResetStats();
		static Statistics GetStats();
	private:
		static void FlushAndReset();
	};
}