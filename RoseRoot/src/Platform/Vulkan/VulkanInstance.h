#pragma once

#include "VulkanSwapChain.h"

typedef struct VkInstance_T* VkInstance;


namespace Rose {
	//The name is a little misleading this acts more as a place to store things that the vulkan renderer will need in many places.
	class VulkanInstance {
	public:
		static void Init(const std::string& appName, void* nativeWindowHandle);
		static void ShutDown();

		static VkInstance GetInstance();
		static Ref<VulkanSwapChain> GetSwapChain();
	};
}