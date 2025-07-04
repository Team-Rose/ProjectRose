#pragma once

#include "RoseRoot/Renderer/RendererAPI.h"

namespace Rose
 {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;

		virtual void CullFaceDisabled() override;
		virtual void CullFaceFront() override;
		virtual void CullFaceBack() override;

		virtual void DisableDepthTest() override;
		virtual void EnableDepthTest() override;

		virtual void SetLineWidth(float width) override;
	};
}