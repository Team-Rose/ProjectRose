#include "rrpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Rose
 {

	Ref<VertexArray> VertexArray::Create() 
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    RR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLVertexArray>();
		}

		RR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}