#include "rrpch.h"
#include "VulkanInstance.h"
#include "VulkanUtils.h"



#include <vulkan/vulkan.h>

#ifdef RR_PLATFORM_WINDOWS
#include <GLFW/glfw3.h>
#endif

#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include <RoseRoot/Core/Application.h>


namespace Rose {
#ifdef RR_DEBUG
#define ENABLE_VULKAN_DEBUG 1
#else
#define ENABLE_VULKAN_DEBUG 0
#endif

    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugUtilsMessengerCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, const VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        (void)messageType; (void)pCallbackData; (void)pUserData;
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            RR_CORE_ERROR("Vulkan Error: {}", pCallbackData->pMessage);
        }
        else if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            RR_CORE_ERROR("Vulkan Warning: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
            RR_CORE_INFO("Vulkan Info: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
            RR_CORE_TRACE("Vulkan Verbose: {}", pCallbackData->pMessage);
        }
        else
        {
            RR_ASSERT(false, "Invalid vulkan message severity");
        }

        return VK_FALSE;
    }
    struct VulkanInstanceData {
        VkInstance Instance = nullptr;
        VkDebugUtilsMessengerEXT DebugUtilsMessenger = VK_NULL_HANDLE;

        Ref<VulkanSwapChain> SwapChain = nullptr;
        Ref<VulkanPhysicalDevice> PhysicalDevice = nullptr;
        Ref<VulkanLogicalDevice> LogicalDevice = nullptr;
    };

    static VulkanInstanceData s_Data;
	void VulkanInstance::Init(const std::string& appName, void* nativeWindowHandle)
	{
        RR_PROFILE_FUNCTION();
        /////////////////////
        // Applcation Info //
        /////////////////////
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = appName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); //TODO
        appInfo.pEngineName = "Rose Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); //TODO
        appInfo.apiVersion = VK_API_VERSION_1_1;

        ///////////////////////////////
        // Extensions and Validation //
        ///////////////////////////////
        std::vector<const char*> instanceExtensions = {};
#ifdef RR_PLATFORM_WINDOWS
        instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        instanceExtensions.push_back("VK_KHR_win32_surface");
#endif // RR_PLATFORM_WINDOWS

#ifndef RR_DIST
        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // Very little performance hit, can be used in Release.
#endif // !RR_DIST


        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo;
        instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

#if ENABLE_VULKAN_DEBUG
        //Check for validation layer support
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        
        bool validationLayerPresent = false;
        const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
        RR_CORE_TRACE("Vulkan Instance Layers:");
        for (const VkLayerProperties& layer : availableLayers)
        {
            RR_CORE_TRACE("  {0}", layer.layerName);
            if (strcmp(layer.layerName, validationLayerName) == 0)
            {
                validationLayerPresent = true;
                break;
            }
        }
        if (validationLayerPresent)
        {
            instanceCreateInfo.ppEnabledLayerNames = &validationLayerName;
            instanceCreateInfo.enabledLayerCount = 1;
        }
        else
        {
            RR_CORE_ERROR("Validation layer VK_LAYER_KHRONOS_validation not present, validation is disabled");
        }
#endif
        ///////////////////////////////////
        // Instance And Surface Creation //
        ///////////////////////////////////
        VKCheck(vkCreateInstance(&instanceCreateInfo, nullptr, &s_Data.Instance));
#if ENABLE_VULKAN_DEBUG
        auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_Data.Instance, "vkCreateDebugUtilsMessengerEXT");
        RR_CORE_ASSERT(vkCreateDebugUtilsMessengerEXT != NULL);

        VkDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo{};
        debugUtilsCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugUtilsCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugUtilsCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
          //  | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
        debugUtilsCreateInfo.pfnUserCallback = VulkanDebugUtilsMessengerCallback;

        VKCheck(vkCreateDebugUtilsMessengerEXT(s_Data.Instance, &debugUtilsCreateInfo, nullptr, &s_Data.DebugUtilsMessenger));
#endif

        s_Data.SwapChain = CreateRef<VulkanSwapChain>(s_Data.Instance);

#ifdef RR_PLATFORM_WINDOWS
        //This is aweful don't do this. I am just lazy.
        s_Data.SwapChain->InitSurfaceGLFW((GLFWwindow*)nativeWindowHandle);
#endif // RR_PLATFORM_WINDOWS

        s_Data.PhysicalDevice = CreateRef<VulkanPhysicalDevice>();

        VkPhysicalDeviceFeatures enabledFeatures;
        memset(&enabledFeatures, VK_FALSE, sizeof(VkPhysicalDeviceFeatures));
        s_Data.LogicalDevice = CreateRef<VulkanLogicalDevice>(s_Data.PhysicalDevice, enabledFeatures);

        s_Data.SwapChain->Init(s_Data.LogicalDevice);
	}
    void VulkanInstance::ShutDown()
    {
        vkDestroyInstance(s_Data.Instance, nullptr);
        s_Data.Instance = nullptr;
    }
    VkInstance VulkanInstance::GetInstance()
    {
        return s_Data.Instance;
    }
    Ref<VulkanSwapChain> VulkanInstance::GetSwapChain()
    {
        return s_Data.SwapChain;
    }
}