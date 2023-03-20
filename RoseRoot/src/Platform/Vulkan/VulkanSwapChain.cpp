#include "rrpch.h"
#include "VulkanSwapChain.h"
#include "VulkanUtils.h"


namespace Rose {
	VulkanSwapChain::VulkanSwapChain(VkInstance instance)
	{
		m_Instance = instance;
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		if (m_Surface) {
			vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		}

		if(m_SwapChain)
			vkDestroySwapchainKHR(m_Device->Get(), m_SwapChain, nullptr);
	}

#ifdef RR_PLATFORM_WINDOWS
	void VulkanSwapChain::InitSurfaceGLFW(GLFWwindow* windowHandle)
	{
		VKCheck(glfwCreateWindowSurface(m_Instance, windowHandle, nullptr, &m_Surface));

	}
#endif

	void VulkanSwapChain::Init(Ref<VulkanLogicalDevice> device)
	{
		m_Device = device;
		VkPhysicalDevice physicalDevce = m_Device->GetPhysicalDevice()->Get();

		
		// Get capabilities //
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevce, m_Surface, &m_SurfaceCapabilities);

#pragma region Select Color Space and Format
		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevce, m_Surface, &formatCount, nullptr);
		RR_CORE_ASSERT(formatCount != 0);
		surfaceFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevce, m_Surface, &formatCount, surfaceFormats.data());

		// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
		// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
		if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
		{
			m_ColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
			m_ColorSpace = surfaceFormats[0].colorSpace;
		}
		else
		{
			// iterate over the list of available surface format and
			// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
			bool found_B8G8R8A8_UNORM = false;
			for (auto&& surfaceFormat : surfaceFormats)
			{
				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
				{
					m_ColorFormat = surfaceFormat.format;
					m_ColorSpace = surfaceFormat.colorSpace;
					found_B8G8R8A8_UNORM = true;
					break;
				}
			}

			// in case VK_FORMAT_B8G8R8A8_UNORM is not available
			// select the first available color format
			if (!found_B8G8R8A8_UNORM)
			{
				m_ColorFormat = surfaceFormats[0].format;
				m_ColorSpace = surfaceFormats[0].colorSpace;
			}
		}
#pragma endregion

		//Cache present mode info for creation
		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevce, m_Surface, &presentModeCount, nullptr);
		RR_CORE_ASSERT(presentModeCount != 0);
		m_PresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevce, m_Surface, &presentModeCount, m_PresentModes.data());

	}
	void VulkanSwapChain::Create(uint32_t* width, uint32_t* height, bool vsync)
	{
		m_VSync = vsync;

		VkDevice device = m_Device->Get();
		VkPhysicalDevice physicalDevice = m_Device->GetPhysicalDevice()->Get();

		VkSwapchainKHR oldSwapchain = m_SwapChain;

#pragma region Swap chain extent
		VkExtent2D swapchainExtent = {};
		// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
		if (m_SurfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
		{
			// If the surface size is undefined, the size is set to
			// the size of the images requested.
			swapchainExtent.width = *width;
			swapchainExtent.height = *height;
		}
		else
		{
			// If the surface size is defined, the swap chain size must match
			swapchainExtent = m_SurfaceCapabilities.currentExtent;
			*width = m_SurfaceCapabilities.currentExtent.width;
			*height = m_SurfaceCapabilities.currentExtent.height;
		}

		m_Width = *width;
		m_Height = *height;

		// Select a present mode for the swapchain

		// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
		// This mode waits for the vertical blank ("v-sync")
		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

		// If v-sync is not requested, try to find a mailbox mode
		// It's the lowest latency non-tearing present mode available
		if (!vsync)
		{
			for (size_t i = 0; i < m_PresentModes.size(); i++)
			{
				if (m_PresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				}
				if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (m_PresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
				{
					swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				}
			}
		}
#pragma endregion

		uint32_t desiredNumberOfSwapchainImages = m_SurfaceCapabilities.minImageCount + 1;
		if (m_SurfaceCapabilities.maxImageCount > 0 && desiredNumberOfSwapchainImages > m_SurfaceCapabilities.maxImageCount)
			desiredNumberOfSwapchainImages = m_SurfaceCapabilities.maxImageCount;

		// Find a supported composite alpha format (not all devices support alpha opaque)
		VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		// Simply select the first composite alpha format available
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		for (auto& compositeAlphaFlag : compositeAlphaFlags) {
			if (m_SurfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag) {
				compositeAlpha = compositeAlphaFlag;
				break;
			};
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;
		createInfo.minImageCount = desiredNumberOfSwapchainImages;
		createInfo.imageFormat = m_ColorFormat;
		createInfo.imageColorSpace = m_ColorSpace;
		createInfo.imageExtent = swapchainExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		auto queueFamilyIndices = m_Device->GetPhysicalDevice()->GetQueueFamilyIndices();
		if (queueFamilyIndices.Graphics == queueFamilyIndices.Present) {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;

			uint32_t queueFamilyIndicesArray[] = { queueFamilyIndices.Graphics, queueFamilyIndices.Present };
			createInfo.pQueueFamilyIndices = queueFamilyIndicesArray;
		}

		createInfo.preTransform = m_SurfaceCapabilities.currentTransform;
		createInfo.compositeAlpha = compositeAlpha;
		createInfo.presentMode = swapchainPresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = oldSwapchain;

		VKCheck(vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_SwapChain));

		//Post swap chain creation

		if (oldSwapchain)
			vkDestroySwapchainKHR(device, oldSwapchain, nullptr);

		for (auto& imageView : m_SwapChainImagesViews)
			vkDestroyImageView(device, imageView, nullptr);

		m_SwapChainImages.clear();
		m_SwapChainImagesViews.clear();
		
		uint32_t imageCount = 0;
		vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, nullptr);
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, m_SwapChainImages.data());
		

		m_SwapChainImagesViews.resize(imageCount);
		for (size_t i = 0; i < m_SwapChainImages.size(); i++) {
			VkImageViewCreateInfo colorAttachmentViewCI = {};
			colorAttachmentViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorAttachmentViewCI.image = m_SwapChainImages[i];
			colorAttachmentViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorAttachmentViewCI.format = m_ColorFormat;
			{
				colorAttachmentViewCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				colorAttachmentViewCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				colorAttachmentViewCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				colorAttachmentViewCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			}
			{
				colorAttachmentViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				colorAttachmentViewCI.subresourceRange.baseMipLevel = 0;
				colorAttachmentViewCI.subresourceRange.levelCount = 1;
				colorAttachmentViewCI.subresourceRange.baseArrayLayer = 0;
				colorAttachmentViewCI.subresourceRange.layerCount = 1;
			}

			VKCheck(vkCreateImageView(device, &colorAttachmentViewCI, nullptr, &m_SwapChainImagesViews[i]));
		}
		RR_CORE_ASSERT(false);
	}
}