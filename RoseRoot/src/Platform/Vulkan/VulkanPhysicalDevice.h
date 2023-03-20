#pragma once
#include <vulkan/vulkan.h>

namespace Rose {
	class VulkanPhysicalDevice
	{
	public:
		struct QueueFamilyIndices
		{
			int32_t Graphics = -1;
			int32_t Present = -1;
			//int32_t Compute = -1;
			//int32_t Transfer = -1;
		};
	public:
		VulkanPhysicalDevice();
		~VulkanPhysicalDevice();

		bool IsExtensionSupported(const std::string& extensionName) const;

		inline const QueueFamilyIndices& GetQueueFamilyIndices() { return m_QueueFamilyIndices; }
		inline const VkPhysicalDevice& Get() { return m_PhysicalDevice; }
	private:
		QueueFamilyIndices m_QueueFamilyIndices;

		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkPhysicalDeviceProperties m_Properties;
		VkPhysicalDeviceMemoryProperties m_MemoryProperties;
		VkPhysicalDeviceFeatures m_Features;

		//TODO Could we maybe delete this after startup sense it is only needed in startup?
		std::unordered_set<std::string> m_SupportedExtensions;
	};
}