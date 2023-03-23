#include "rrpch.h"-
#include "VulkanContext.h"
#include "VulkanInstance.h"

namespace Rose
{
	VulkanContext::VulkanContext(GLFWwindow* windowHandle, const std::string& appName)
		: m_WindowHandle(windowHandle), m_AppName(appName)
	{
		RR_CORE_ASSERT(windowHandle, "Window handle is null!");
	}
	VulkanContext::~VulkanContext()
	{
		VulkanInstance::ShutDown();
	}
	void VulkanContext::Init()
	{
		RR_PROFILE_FUNCTION();
		VulkanInstance::Init(m_AppName, m_WindowHandle);

	}
	void VulkanContext::SwapBuffers()
	{
		RR_PROFILE_FUNCTION();
	}
}
