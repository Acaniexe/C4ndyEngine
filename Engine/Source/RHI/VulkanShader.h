#pragma once
#include <Core/Core.h>
#include "VulkanContext.h"

namespace C4ndy::RHI
{
    class VulkanShader
    {
    public:
        VulkanShader();
        C4NDY_NON_COPYABLE_MOVEABLE(VulkanShader);

        void Init(VulkanContext&, const std::string& path, VkShaderStageFlagBits stage);
        void Shutdown();
        
        VkShaderModule GetModule() const { return m_ShaderModule; }
        VkShaderStageFlagBits GetStage() const { return m_Stage; }
        VkPipelineShaderStageCreateInfo GetStageInfo() const;

    private:
        VulkanContext* m_Context = nullptr;
        VkShaderModule m_ShaderModule = VK_NULL_HANDLE;
        VkShaderStageFlagBits m_Stage = VK_SHADER_STAGE_VERTEX_BIT;

        void LoadSPIRV(const std::string& path);
    };
}