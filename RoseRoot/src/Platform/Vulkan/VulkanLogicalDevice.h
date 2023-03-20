#pragma once
#include "VulkanPhysicalDevice.h"

#include <vulkan/vulkan.h>

namespace Rose {
	class VulkanLogicalDevice
	{
	public:
		VulkanLogicalDevice(Ref<VulkanPhysicalDevice> physicalDevice, VkPhysicalDeviceFeatures enabledFeatures);
		~VulkanLogicalDevice();

		VkDevice Get() { return m_Device; }
		Ref<VulkanPhysicalDevice> GetPhysicalDevice() { return m_PhysicalDevice; }
	private:
		VkDevice m_Device = nullptr;
		Ref<VulkanPhysicalDevice> m_PhysicalDevice;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
	};
}