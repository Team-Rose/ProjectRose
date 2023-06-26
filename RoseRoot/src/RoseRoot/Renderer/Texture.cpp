#include "rrpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Rose
 {
	Ref<Texture2D> Texture2D::Create(const Texture2DSpecification& specification, Buffer data)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    RR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(specification, data);
		}

		RR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}