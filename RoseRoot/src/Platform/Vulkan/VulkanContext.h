#pragma once

#include "RoseRoot/Renderer/GraphicsContexted.h"

struct GLFWwindow;

namespace Rose
{

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(GLFWwindow* windowHandle, const std::string& appName);
		~VulkanContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
		std::string m_AppName;
	};
}
