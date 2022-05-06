#include "rrpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Rose
 {

	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}