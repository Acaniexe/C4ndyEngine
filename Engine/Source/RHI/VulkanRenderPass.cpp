#include "VulkanRenderPass.h"
#include <Core/Logger.h>

namespace C4ndy::RHI
{
    void VulkanRenderPass::Init(VulkanContext& context, VkFormat colorFormat, VkFormat depthFormat)
    {
        m_Context = &context;
        m_ColorFormat = colorFormat;
        m_DepthFormat = depthFormat == VK_FORMAT_UNDEFINED ? FindDepthFormat() : depthFormat;

        CreateRenderPass();
        C4NDY_INFO("Vulkan render pass created.");
    }

    void VulkanRenderPass::Shutdown()
    {
        vkDestroyRenderPass(m_Context->GetDevice(), m_RenderPass, nullptr);
        C4NDY_INFO("Vulkan render pass destroyed.");
    }

    void VulkanRenderPass::End(VkCommandBuffer cmd)
    {
        vkCmdEndRenderPass(cmd);
    }
}