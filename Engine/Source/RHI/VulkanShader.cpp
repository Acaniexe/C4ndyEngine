#include "VulkanShader.h"
#include <Core/Logger.h>
#include <fstream>

namespace C4ndy::RHI
{
    void VulkanShader::Init(VulkanContext& context, const std::string& path, VkShaderStageFlagBits stage)
    {
        m_Context = &context;
        m_Stage = stage;
        LoadSPIRV(path);
        C4NDY_INFO("Vulkan shader loaded: " + path);
    }

    void VulkanShader::Shutdown()
    {
        vkDestroyShaderModule(m_Context->GetDevice(), m_ShaderModule, nullptr);
    }

    void VulkanShader::LoadSPIRV(const std::string& path)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (!file.is_open())
            C4NDY_FATAL("Failed to open shader file: " + path);

        usize fileSize = (usize)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = buffer.size();
        createInfo.pCode = reinterpret_cast<const u32*>(buffer.data());

        if (vkCreateShaderModule(m_Context->GetDevice(), &createInfo, nullptr, &m_ShaderModule) != VK_SUCCESS)
            C4NDY_FATAL("Failed to create shader module: " + path);
    }

    VkPipelineShaderStageCreateInfo VulkanShader::GetStageInfo() const
    {
        VkPipelineShaderStageCreateInfo stageInfo{};
        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.stage = m_Stage;
        stageInfo.module = m_ShaderModule;
        stageInfo.pName = "main";
        return stageInfo;
    }
}