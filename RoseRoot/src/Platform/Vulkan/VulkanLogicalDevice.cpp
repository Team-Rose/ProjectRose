#include "rrpch.h"
#include "VulkanLogicalDevice.h"

#include "VulkanUtils.h"

namespace Rose {
	VulkanLogicalDevice::VulkanLogicalDevice(Ref<VulkanPhysicalDevice> physicalDevice, VkPhysicalDeviceFeatures enabledFeatures)
		: m_PhysicalDevice(physicalDevice)
	{
		// Setup queue create infos
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		
		auto queueFamilyIndices = m_PhysicalDevice->GetQueueFamilyIndices();
		static const float queuePriority = 1.0f;
		//Graphics
		VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
		graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		graphicsQueueCreateInfo.queueFamilyIndex = queueFamilyIndices.Graphics;
		graphicsQueueCreateInfo.queueCount = 1;
		graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(graphicsQueueCreateInfo);

		//Optional Present Queue
		if (queueFamilyIndices.Graphics != queueFamilyIndices.Present) {
			VkDeviceQueueCreateInfo presentQueueCreateInfo{};
			presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			presentQueueCreateInfo.queueFamilyIndex = queueFamilyIndices.Graphics;
			presentQueueCreateInfo.queueCount = 1;
			presentQueueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(presentQueueCreateInfo);
		}

		//Enable extensions
		std::vector<const char*> deviceExtensions;
		RR_CORE_ASSERT(m_PhysicalDevice->IsExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME));
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);


		//Assemble create info and create device
		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();

		RR_CORE_ASSERT(deviceExtensions.size() > 0)
		deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

		deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

		//Set these if there is compatibility issues
		//These don't need to be set on new implmentation of vulkan but may be needed on older devices
		//deviceCreateInfo.enabledLayerCount = 
		//deviceCreateInfo.ppEnabledLayerNames
		VKCheck(vkCreateDevice(m_PhysicalDevice->Get(), &deviceCreateInfo, nullptr, &m_Device));

		vkGetDeviceQueue(m_Device, queueFamilyIndices.Graphics, 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, queueFamilyIndices.Present, 0, &m_PresentQueue);
	}
	VulkanLogicalDevice::~VulkanLogicalDevice()
	{
		vkDestroyDevice(m_Device, nullptr);
	}
}