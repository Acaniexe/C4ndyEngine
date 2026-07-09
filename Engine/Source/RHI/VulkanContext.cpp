#include <Core/Core.h>
#include "VulkanContext.h"
#include <Core/Logger.h>
#include <set>
#if defined(C4NDY_PLATFORM_WINDOWS)
    #include <Windows.h>
    #include <vulkan/vulkan_win32.h>
#elif defined(C4NDY_PLATFORM_LINUX)
    #include <xcb/xcb.h>
    #include <vulkan/vulkan_xcb.h>
#endif

namespace C4ndy::RHI
{
    void VulkanContext::Init(void* windowHandle)
    {
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface(windowHandle);
        PickPhysicalDevice();
        CreateLogicalDevice();

        C4NDY_INFO("Vulkan context initialized.");
    }

    void VulkanContext::Shutdown()
    {
        vkDestroyDevice(m_Device, nullptr);

        if (k_EnableValidation)
            DestroyDebugMessenger();

        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkDestroyInstance(m_Instance, nullptr);

        C4NDY_INFO("Vulkan context destroyed.");
    }

    void VulkanContext::CreateInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "C4ndy Engine";
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        appInfo.pEngineName = "C4ndy";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        std::vector<const char*> extensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,
    #if defined(C4NDY_PLATFORM_WINDOWS)
            "VK_KHR_win32_surface",
    #elif defined(C4NDY_PLATFORM_LINUX)
            "VK_KHR_xcb_surface",
    #elif defined(C4NDY_PLATFORM_MACOS)
            "VK_MVK_macos_surface",
    #endif
        };

        if (k_EnableValidation)
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = (u32)extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        if (k_EnableValidation)
        {
            createInfo.enabledLayerCount = 1;
            createInfo.ppEnabledLayerNames = &k_ValidationLayer;
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
            C4NDY_FATAL("Failed to create Vulkan instance.");
        else
            C4NDY_INFO("Vulkan instance created successfully.");
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* data,
        void* userData)
    {
         if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            C4NDY_ERROR(std::string("[VULKAN] ") + data->pMessage);
         else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
             C4NDY_WARN(std::string("[VULKAN] ") + data->pMessage);
        else
             C4NDY_INFO(std::string("[VULKAN] ") + data->pMessage);

        return VK_FALSE;
    }

    void VulkanContext::SetupDebugMessenger()
    {
        if (!k_EnableValidation) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            createInfo.pfnUserCallback = DebugCallback;

            auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
                vkGetInstanceProcAddr (m_Instance, "vkCreateDebugUtilsMessengerEXT");

            if (func && func(m_Instance, &createInfo, nullptr, &m_DebugMessenger) == VK_SUCCESS)
                C4NDY_INFO("Vulkan validation layers enabled.");
            else
                C4NDY_WARN("Failed to setup Vulkan debug messenger.");
    }     
    
    void VulkanContext::DestroyDebugMessenger()
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr (m_Instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func)
            func(m_Instance, m_DebugMessenger, nullptr);
    }

    void VulkanContext::CreateSurface(void* windowHandle)
    {
    #if defined(C4NDY_PLATFORM_WINDOWS)
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = (HWND)windowHandle;
        createInfo.hinstance = GetModuleHandle(nullptr);

        if (vkCreateWin32SurfaceKHR(m_Instance, &createInfo, nullptr, &m_Surface) != VK_SUCCESS)
            C4NDY_FATAL("Failed to create Win32 Vulkan surface!");
    #elif defined(C4NDY_PLATFORM_LINUX)
        struct LinuxWindowHandle { void* display; unsigned long window; };
        auto* handle = (LinuxWindowHandle*)windowHandle;

        VkXcbSurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        createInfo.connection = (xcb_connection_t*)handle->display;
        createInfo.window = (xcb_window_t)handle->window;

        if (vkCreateXcbSurfaceKHR(m_Instance, &createInfo, nullptr, &m_Surface) != VK_SUCCESS)
            C4NDY_FATAL("Failed to create linux Vulkan Surface!");
    #endif

        C4NDY_INFO("Vulkan surface created.");
    }

    void VulkanContext::PickPhysicalDevice()
    {
        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if (deviceCount == 0)
            C4NDY_FATAL("No Vulkan-capable GPU found!");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        for (const auto& device : devices)
        {
            u32 queueCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);
            std::vector<VkQueueFamilyProperties> queues(queueCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queues.data());

            bool hasGraphics = false;
            bool hasPresent = false;

            for (u32 i = 0; i < queueCount; i++)
            {
                if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    m_GraphicsQueueFamily = i;
                    hasGraphics = true;
                }

                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);
                if (presentSupport)
                {
                    m_PresentQueueFamily = i;
                    hasPresent = true;
                }
            }

            if (hasGraphics && hasPresent)
            {
                m_PhysicalDevice = device;

                VkPhysicalDeviceProperties props;
                vkGetPhysicalDeviceProperties(device, &props);
                C4NDY_INFO(std::string("GPU selected: ") + props.deviceName);
                return;
            }
        }

        C4NDY_FATAL("No suitable GPU found!");
    }

    void VulkanContext::CreateLogicalDevice()
    {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<u32> uniqueQueueFamilies = {
            m_GraphicsQueueFamily,
            m_PresentQueueFamily
        };

        f32 queuePriority = 1.0f;
        for (u32 queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = (u32)queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = (u32)ArrayCount(k_DeviceExtensions);
        createInfo.ppEnabledExtensionNames = k_DeviceExtensions;

        if (k_EnableValidation)
        {
            createInfo.enabledLayerCount = 1;
            createInfo.ppEnabledLayerNames = &k_ValidationLayer;
        }

        if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
            C4NDY_FATAL("Failed to create Vulkan logical device!");

        vkGetDeviceQueue(m_Device, m_GraphicsQueueFamily, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, m_PresentQueueFamily, 0, &m_PresentQueue);

        C4NDY_INFO("Vulkan logical device created.");
    }
}