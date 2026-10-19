#pragma once
#include <Core/Core.h>
#include "VulkanContext.h"
#include "VulkanShader.h"
#include "VulkanRenderPass.h"

namespace C4ndy::RHI
{
    class VulkanPipeline
    {
    public:
        VulkanPipeline();
        C4NDY_NON_COPYABLE_MOVEABLE(VulkanPipeline);

        void Init(VulkanContext& context);
        void Shutdown();

        void SetShaders(VulkanShader& vert, VulkanShader& frag);
        void SetRenderPass(VulkanRenderPass&);
        void SetDescriptorLayout(VkDescriptorSetLayout);
        void SetExtent(VkExtent2D);

        void Build();
        void Bind(VkCommandBuffer);
        VkPipelineLayout GetLayout() const { return m_PipelineLayout; }

    private:
        VulkanContext* m_Context = nullptr;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
        VkPipelineShaderStageCreateInfo m_ShdaerStages[2]{};
        VkRenderPass m_RenderPass = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_DescriptorLayout = VK_NULL_HANDLE;
        VkExtent2D m_Extent{};
    };
}