#pragma once
#include <vulkan/vulkan_core.h>

namespace Rose {
	static VkResult VKCheck(VkResult result) {
#ifdef RR_DIST
		//TODO (Sam) Maybe? idk if it is worth the extra ms to just skip this check might be useful for finding issues
		//return result;
#endif // RR_DIST

		switch (result)
		{
		case(VK_SUCCESS): break;
		case(VK_NOT_READY): RR_CORE_WARN("A vulkan call returned VK_NOT_READY"); break;
		case(VK_TIMEOUT): RR_CORE_WARN("A vulkan call returned VK_TIMEOUT"); break;
		case(VK_EVENT_SET): RR_CORE_WARN("A vulkan call returned VK_EVENT_SET"); break;
		case(VK_EVENT_RESET): RR_CORE_WARN("A vulkan call returned VK_EVENT_RESET"); break;
		case(VK_INCOMPLETE): RR_CORE_WARN("A vulkan call returned VK_INCOMPLETE"); break;

		case(VK_ERROR_OUT_OF_HOST_MEMORY): RR_CORE_ERROR("A vulkan call returned VK_ERROR_OUT_OF_HOST_MEMORY!"); break;
		case(VK_ERROR_OUT_OF_DEVICE_MEMORY): RR_CORE_ERROR("A vulkan call returned VK_ERROR_OUT_OF_DEVICE_MEMORY!"); break;
		case(VK_ERROR_INITIALIZATION_FAILED): RR_CORE_ERROR("A vulkan call returned VK_ERROR_INITIALIZATION_FAILED!"); break;
		case(VK_ERROR_DEVICE_LOST): RR_CORE_ERROR("A vulkan call returned VK_ERROR_DEVICE_LOST!"); break;
		case(VK_ERROR_MEMORY_MAP_FAILED): RR_CORE_ERROR("A vulkan call returned VK_ERROR_MEMORY_MAP_FAILED!"); break;
		case(VK_ERROR_LAYER_NOT_PRESENT): RR_CORE_ERROR("A vulkan call returned VK_ERROR_LAYER_NOT_PRESENT!"); break;
		case(VK_ERROR_EXTENSION_NOT_PRESENT): RR_CORE_ERROR("A vulkan call returned VK_ERROR_EXTENSION_NOT_PRESENT!"); break;
		case(VK_ERROR_FEATURE_NOT_PRESENT): RR_CORE_ERROR("A vulkan call returned VK_ERROR_FEATURE_NOT_PRESENT!"); break;
		case(VK_ERROR_INCOMPATIBLE_DRIVER): RR_CORE_ERROR("A vulkan call returned VK_ERROR_INCOMPATIBLE_DRIVER!"); break;
		case(VK_ERROR_TOO_MANY_OBJECTS): RR_CORE_ERROR("A vulkan call returned VK_ERROR_TOO_MANY_OBJECTS!"); break;
		case(VK_ERROR_FORMAT_NOT_SUPPORTED): RR_CORE_ERROR("A vulkan call returned VK_ERROR_FORMAT_NOT_SUPPORTED!"); break;
		case(VK_ERROR_FRAGMENTED_POOL): RR_CORE_ERROR("A vulkan call returned VK_ERROR_FRAGMENTED_POOL!"); break;
		case(VK_ERROR_UNKNOWN): RR_CORE_ERROR("A vulkan call returned VK_ERROR_OUT_OF_HOST_MEMORY!"); break;
		}

#ifdef RR_DEBUG
		RR_CORE_ASSERT(result == VK_SUCCESS, "VKCheck failed!")
#endif // RR_DEBUG

		return result;
	}
}