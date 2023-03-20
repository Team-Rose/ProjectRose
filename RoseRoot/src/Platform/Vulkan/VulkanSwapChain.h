#pragma once

#include "VulkanLogicalDevice.h"

#include <vulkan/vulkan.h>

#ifdef RR_PLATFORM_WINDOWS
#include <GLFW/glfw3.h>
#endif // RR_PLATFORM_WINDOWS


namespace Rose {
	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VkInstance instance);
		~VulkanSwapChain();

#ifdef RR_PLATFORM_WINDOWS
		void InitSurfaceGLFW(GLFWwindow* windowHandle);
#endif // RR_PLATFORM_WINDOWS

		//Make sure to call InitSurface[Platform]() before init!
		void Init(Ref<VulkanLogicalDevice> device);

		void Create(uint32_t* width, uint32_t* height, bool vsync);
		VkSurfaceKHR GetSurface() { return m_Surface; }
	private:
		VkSwapchainKHR m_SwapChain = nullptr;

		VkInstance m_Instance = nullptr;
		VkSurfaceKHR m_Surface = nullptr;
		bool m_VSync = false;
		uint32_t m_Width = 0, m_Height = 0;
		
		VkFormat m_ColorFormat;
		VkColorSpaceKHR m_ColorSpace;

		Ref<VulkanLogicalDevice> m_Device = nullptr;

		VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;
		std::vector<VkPresentModeKHR> m_PresentModes;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImagesViews;
	};
}