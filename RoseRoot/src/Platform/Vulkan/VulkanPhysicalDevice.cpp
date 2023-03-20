#include "rrpch.h"
#include "VulkanPhysicalDevice.h"

#include "VulkanUtils.h"
#include "VulkanInstance.h"

namespace Rose {
	VulkanPhysicalDevice::VulkanPhysicalDevice()
	{
		VkInstance vkInstance = VulkanInstance::GetInstance();

		//Can we do this without calling the same function twice??
		uint32_t gpuCount = 0;
		// Get number of available physical devices
		vkEnumeratePhysicalDevices(vkInstance, &gpuCount, nullptr);
		RR_CORE_ASSERT(gpuCount > 0, "");
		// Enumerate devices
		std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
		VKCheck(vkEnumeratePhysicalDevices(vkInstance, &gpuCount, physicalDevices.data()));

		VkSurfaceKHR surface = VulkanInstance::GetSwapChain()->GetSurface();
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		int highScore = INT_MIN;
		for (VkPhysicalDevice physicalDevice : physicalDevices)
		{
			int score = 0;
			vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
			vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

			bool deviceSuitable = deviceFeatures.geometryShader; //TODO (Sam) check for features we actually used just put a random one to test
			if (!deviceSuitable)
				score -= 100000;

			if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				score += 100000;
			
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
			uint32_t i = 0;

			QueueFamilyIndices queueFamilyIndices;
			bool foundShared = false;
			for (const auto& queueFamily : queueFamilies) {
				if (!foundShared) {
					if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
						queueFamilyIndices.Graphics = i;
					}
					VkBool32 presentSupport = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
					if (presentSupport) {
						queueFamilyIndices.Present = i;
					}

					if (queueFamilyIndices.Present > -1 && queueFamilyIndices.Graphics > -1)
						foundShared = true;
				}
				i++;
			}

			if (foundShared)
				score += 100;

			if (score > highScore) {
				highScore = score;
				m_PhysicalDevice = physicalDevice;
				m_Properties = deviceProperties;
				m_Features = deviceFeatures;
				m_QueueFamilyIndices = queueFamilyIndices;
			}
		}
		if (highScore < 0) {
			RR_CORE_WARN("Vulkan the selected physical device has a suitability score below zero! You may experience crashes or undefined behavior.");
		}
		RR_CORE_ASSERT(m_PhysicalDevice, "Could not find any physical devices!");

		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_MemoryProperties);
		RR_CORE_ASSERT(m_QueueFamilyIndices.Graphics > -1);
		RR_CORE_ASSERT(m_QueueFamilyIndices.Present > -1);


		uint32_t extCount = 0;
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extCount, nullptr);
		if (extCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extCount);
			if (vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
			{
#ifdef RR_DEBUG
				RR_CORE_TRACE("Selected physical device has {0} extensions", extensions.size());
#endif // RR_DEBUG

				for (const auto& ext : extensions)
				{
					m_SupportedExtensions.emplace(ext.extensionName);
#ifdef RR_DEBUG
					RR_CORE_TRACE("  {0}", ext.extensionName);
#endif // RR_DEBUG
				}
			}
		}
	}
	VulkanPhysicalDevice::~VulkanPhysicalDevice()
	{
	}
	bool VulkanPhysicalDevice::IsExtensionSupported(const std::string& extensionName) const
	{
		return m_SupportedExtensions.find(extensionName) != m_SupportedExtensions.end();
	}
}