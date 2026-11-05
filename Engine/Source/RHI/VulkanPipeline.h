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
        void SetPushConstantSize(u32 size) { m_PushConstantSize = size; }

        void Build();
        void Bind(VkCommandBuffer);
        void SetUseVertexInput(bool use) { m_UseVertexInput = use; }
        VkPipelineLayout GetLayout() const { return m_PipelineLayout; }

    private:
        VulkanContext* m_Context = nullptr;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
        VkPipelineShaderStageCreateInfo m_ShaderStages[2]{};
        VkRenderPass m_RenderPass = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_DescriptorLayout = VK_NULL_HANDLE;
        VkExtent2D m_Extent{};
        bool m_UseVertexInput = false;
        u32 m_PushConstantSize = 0;
    };
}