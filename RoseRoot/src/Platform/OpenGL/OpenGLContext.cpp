#include "rrpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include <gl/GL.h>

namespace Rose
 {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_windowHandle(windowHandle)
	{
		RR_CORE_ASSERT(windowHandle, "Window handle is null!");
	}
	void OpenGLContext::Init()
	{
		RR_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_windowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		RR_CORE_ASSERT(status, "Failed to initailize Glad!");

		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		RR_CORE_INFO("OpenGL Info:");
		RR_CORE_INFO("Vendor: {0}", glGetString(GL_VENDOR));
		RR_CORE_INFO("Renderer: {0}", glGetString(GL_RENDERER));
		RR_CORE_INFO("Version: {0}", glGetString(GL_VERSION));
	}
	void OpenGLContext::SwapBuffers()
	{
		RR_PROFILE_FUNCTION();

		glfwSwapBuffers(m_windowHandle);
	}
}