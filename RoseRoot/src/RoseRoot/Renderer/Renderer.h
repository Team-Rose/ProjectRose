#pragma once

#include "RenderCommand.h"
#include "RoseRoot/Renderer/Camera.h"
#include "RoseRoot/Renderer/EditorCamera.h"
#include "RoseRoot/Renderer/Framebuffer.h"

namespace Rose
 {

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void EndScene();

		//TODO Find a better way to handle framebuffers
		static void DrawFinalFrameBuffer();
		static Ref<Framebuffer> GetFinalFrameBuffer();

		static Ref<Framebuffer> GetShadowFrameBuffer();

		static void ResizeView(uint32_t width, uint32_t height);

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		static void OnViewResized(uint32_t width, uint32_t height);
	private:
		
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static Scope<SceneData> s_SceneData;
	};
}