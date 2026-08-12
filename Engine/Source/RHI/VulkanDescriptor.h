#pragma once
#include <Core/Core.h>
#include "VulkanContext.h"

namespace C4ndy::RHI
{
    class VulkanDescriptor
    {
    public: 
        VulkanDescriptor();
        C4NDY_NON_COPYABLE_MOVEABLE(VulkanDescriptor);

        void Init(VulkanContext& context);
        void Shutdown();
        void AddBinding(u32 binding, VkDescriptorType, VkShaderStageFlags);
        void Build();
        void UpdateBuffer(u32 binding, VkBuffer, VkDeviceSize size);
        void UpdateImage(u32 binding, VkImageView, VkSampler);

        VkDescriptorSetLayout GetLayout() const { return m_Layout; }
        VkDescriptorSet GetSet() const { return m_Set; }

    private:
        VulkanContext* m_Context = nullptr;
        VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;
        VkDescriptorPool m_Pool = VK_NULL_HANDLE;
        VkDescriptorSet m_Set = VK_NULL_HANDLE;

        std::vector<VkDescriptorSetLayoutBinding> m_Bindings;
    };
}