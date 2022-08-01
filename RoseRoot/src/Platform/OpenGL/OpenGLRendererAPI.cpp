#include "rrpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>


namespace Rose
 {

	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:        RR_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:      RR_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:         RR_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION:RR_CORE_TRACE(message); return;
		}

		RR_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init()
	{
		RR_PROFILE_FUNCTION();

#ifdef RR_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, NULL);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef RR_DEBUG
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			switch (error)
			{
			case(GL_INVALID_ENUM):
				RR_CORE_ERROR("GL_INVALID_ENUM : {}", error); break;
			case(GL_INVALID_VALUE):
				RR_CORE_ERROR("GL_INVALID_VALUE : {}", error); break;
			case(GL_STACK_OVERFLOW):
				RR_CORE_ERROR("GL_STACK_OVERFLOW : {}", error); break;
			case(GL_STACK_UNDERFLOW):
				RR_CORE_ERROR("GL_STACK_UNDERFLOW : {}", error); break;
			case(GL_OUT_OF_MEMORY):
				RR_CORE_ERROR("GL_OUT_OF_MEMORY : {}", error); break;
			case(GL_INVALID_FRAMEBUFFER_OPERATION):
				RR_CORE_ERROR("GL_INVALID_FRAMEBUFFER_OPERATION : {}", error); break;
			case(GL_CONTEXT_LOST):
				RR_CORE_ERROR("GL_CONTEXT_LOST : {}", error); break;
			default:
				RR_CORE_ERROR(error); break;
			}
		}
#endif
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRendererAPI::CullFaceDisabled()
	{
		glDisable(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::CullFaceFront()
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void OpenGLRendererAPI::CullFaceBack()
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}

	void OpenGLRendererAPI::DisableDepthTest()
	{
		glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::EnableDepthTest()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetLineWidth(float width)
	{
		glLineWidth(width);
	}

}