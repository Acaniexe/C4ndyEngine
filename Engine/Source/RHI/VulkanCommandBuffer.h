#pragma once
#include <Core/Core.h>
#include "VulkanContext.h"

namespace C4ndy::RHI
{
    class VulkanCommandBuffer
    {
    public:
        VulkanCommandBuffer();
        C4NDY_NON_COPYABLE_MOVEABLE(VulkanCommandBuffer);

        void Init(VulkanContext& context);
        void Shutdown();

        void Begin();
        void End();
        void Submit();
        void Reset();
        void WaitForFence();

        VkCommandBuffer GetCommandBuffer() const { return m_CommandBuffer; }
        VkSemaphore GetImageAvailableSemaphore() const { return m_ImageAvailableSemaphore; }
        VkSemaphore GetRenderFinishedSemaphore() const { return m_RenderFinishedSemaphore; }

    private:
        VulkanContext* m_Context = nullptr;
        VkCommandPool m_CommandPool = VK_NULL_HANDLE;
        VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;

        VkSemaphore m_ImageAvailableSemaphore = VK_NULL_HANDLE;
        VkSemaphore m_RenderFinishedSemaphore = VK_NULL_HANDLE;
        VkFence m_InFlightFence = VK_NULL_HANDLE;

        void CreateCommandPool();
        void CreateCommandBuffer();
        void CreateSyncObjects();
    };
}

