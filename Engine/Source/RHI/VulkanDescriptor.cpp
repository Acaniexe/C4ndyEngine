#include "VulkanDescriptor.h"
#include <Core/Logger.h>

namespace C4ndy::RHI
{
    void VulkanDescriptor::Init(VulkanContext& context)
    {
        m_Context = &context;
    }

    void VulkanDescriptor::Shutdown()
    {
        vkDestroyDescriptorSetLayout(m_Context->GetDevice(), m_Layout, nullptr);
        vkDestroyDescriptorPool(m_Context->GetDevice(), m_Pool, nullptr);
        C4NDY_INFO("Vulkan descriptor destroyed.");
    }

    void VulkanDescriptor::AddBinding(u32 binding, VkDescriptorType type, VkShaderStageFlags stageFlags)
    {
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = type;
        layoutBinding.descriptorCount = 1;
        layoutBinding.stageFlags = stageFlags;
        layoutBinding.pImmutableSamplers = nullptr;
        m_Bindings.push_back(layoutBinding);
    }

    void VulkanDescriptor::Build()
    {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = (u32)m_Bindings.size();
        layoutInfo.pBindings = m_Bindings.data();

        if (vkCreateDescriptorSetLayout(m_Context->GetDevice(), &layoutInfo, nullptr, &m_Layout) != VK_SUCCESS)
            C4NDY_FATAL("Failed to create descriptor set layout!");

        std::vector<VkDescriptorPoolSize> poolSizes;
        for (auto& binding : m_Bindings)
        {
            VkDescriptorPoolSize poolSize{};
            poolSize.type = binding.descriptorType;
            poolSize.descriptorCount = 1;
            poolSizes.push_back(poolSize);
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = (u32)poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = 1;

        if (vkCreateDescriptorPool(m_Context->GetDevice(), &poolInfo, nullptr, &m_Pool) != VK_SUCCESS)
            C4NDY_FATAL("Failed to create descriptor pool!");

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_Pool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &m_Layout;

        if (vkAllocateDescriptorSets(m_Context->GetDevice(), &allocInfo, &m_Set) != VK_SUCCESS)
            C4NDY_FATAL("Failed to allocate descriptor set!");
    }

    void VulkanDescriptor::UpdateBuffer(u32 binding, VkBuffer buffer, VkDeviceSize size)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = size;

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_Set;
        write.dstBinding = binding;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(m_Context->GetDevice(), 1, &write, 0, nullptr);
    }

    void VulkanDescriptor::UpdateImage(u32 binding, VkImageView imageView, VkSampler sampler)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = imageView;
        imageInfo.sampler = sampler;

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_Set;
        write.dstBinding = binding;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(m_Context->GetDevice(), 1, &write, 0, nullptr);
    }
}