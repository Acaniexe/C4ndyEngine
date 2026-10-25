#include "VulkanCommandBuffer.h"
#include <Core/Logger.h>

namespace C4ndy::RHI
{

    VulkanCommandBuffer::VulkanCommandBuffer() = default;
    
    void VulkanCommandBuffer::Init(VulkanContext& context)
    {
        m_Context = &context;
        CreateCommandPool();
        CreateCommandBuffer();
        CreateSyncObjects();
        C4NDY_INFO("Vulkan command buffer initalised!");
    }

    void VulkanCommandBuffer::Shutdown()
    {
        vkDestroySemaphore(m_Context->GetDevice(), m_RenderFinishedSemaphore, nullptr);
        vkDestroySemaphore(m_Context->GetDevice(), m_ImageAvailableSemaphore, nullptr);
        vkDestroyFence(m_Context->GetDevice(), m_InFlightFence, nullptr);
        vkDestroyCommandPool(m_Context->GetDevice(), m_CommandPool, nullptr);
        C4NDY_INFO("Vulkan command buffer destroyed!");
    }

    void VulkanCommandBuffer::Begin()
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
    }

    void VulkanCommandBuffer::End()
    {
        vkEndCommandBuffer(m_CommandBuffer);
    }

    void VulkanCommandBuffer::Reset()
    {
        vkResetCommandBuffer(m_CommandBuffer, 0);
    }

    void VulkanCommandBuffer::WaitForFence()
    {
        vkWaitForFences(m_Context->GetDevice(), 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(m_Context->GetDevice(), 1, &m_InFlightFence);
    }

    void VulkanCommandBuffer::Submit()
    {
        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &m_ImageAvailableSemaphore;
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_RenderFinishedSemaphore;

        if (vkQueueSubmit(m_Context->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFence) != VK_SUCCESS)
            C4NDY_FATAL("Failed to submit command buffer!");
    }

    void VulkanCommandBuffer::CreateCommandPool()
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = m_Context->GetGraphicsQueueFamily();
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(m_Context->GetDevice(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
            C4NDY_FATAL("Failed to create command pool!");
    }

    void VulkanCommandBuffer::CreateCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(m_Context->GetDevice(), &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
            C4NDY_FATAL("Failed to allocate command buffer!");
    }

    void VulkanCommandBuffer::CreateSyncObjects()
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateSemaphore(m_Context->GetDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(m_Context->GetDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(m_Context->GetDevice(), &fenceInfo, nullptr, &m_InFlightFence) != VK_SUCCESS)
            C4NDY_FATAL("Failed to create sync objects!");
    }
}