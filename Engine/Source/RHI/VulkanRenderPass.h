#pragma once
#include <Core/Core.h>
#include "VulkanContext.h"

namespace C4ndy::RHI
{
    class VulkanRenderPass
    {
    public:    
        VulkanRenderPass();
        C4NDY_NON_COPYABLE_MOVEABLE(VulkanRenderPass);

        void Init(VulkanContext&, VkFormat colorFormat, VkFormat depthFormat);
        void Shutdown();
        void Begin(VkCommandBuffer, VkFramebuffer, VkExtent2D);
        void End(VkCommandBuffer);

        VkRenderPass GetRenderPass() const { return m_RenderPass; }

    private:
        VulkanContext* m_Context = nullptr;
        VkRenderPass m_RenderPass = VK_NULL_HANDLE;
        VkFormat m_ColorFormat = VK_FORMAT_UNDEFINED;
        VkFormat m_DepthFormat = VK_FORMAT_UNDEFINED;

        void CreateRenderPass();
        VkFormat FindDepthFormat();
    };
}