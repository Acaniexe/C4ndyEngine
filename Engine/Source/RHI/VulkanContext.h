#pragma once
#include <Core/Core.h>
#include <vulkan/vulkan.h>

namespace C4ndy::RHI
{
    class VulkanContext
    {
    public:
        VulkanContext();
        C4NDY_NON_COPYABLE_MOVEABLE(VulkanContext);

        void Init(void* windowHandle);
        void Shutdown();

        VkInstance GetInstance() const { return m_Instance; }
        VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
        VkDevice GetDevice() const { return m_Device; }
        VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
        VkQueue GetPresentQueue() const { return m_PresentQueue; }
        VkSurfaceKHR GetSurface() const { return m_Surface; }
        u32 GetGraphicsQueueFamily() const { return m_GraphicsQueueFamily; }
        u32 GetPresentQueueFamily() const { return m_PresentQueueFamily; }

    private:
        #ifdef _DEBUG
            static constexpr bool k_EnableValidation = true;
        #else
            static constexpr bool k_EnableValidation = false;
        #endif

        void CreateInstance();
        void SetupDebugMessenger();
        void CreateSurface(void* windowHandle);
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void DestroyDebugMessenger();

        VkInstance m_Instance = VK_NULL_HANDLE;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_PresentQueue = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
        u32 m_GraphicsQueueFamily = 0;
        u32 m_PresentQueueFamily = 0;

        static constexpr const char* k_ValidationLayer = "VK_LAYER_KHRONOS_validation";
        static constexpr const char* k_DeviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
    };
}