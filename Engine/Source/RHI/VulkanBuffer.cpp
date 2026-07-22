#include "VulkanBuffer.h"
#include <Core/Logger.h>

namespace C4ndy::RHI
{
    void VulkanBuffer::Init(VulkanContext& context, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memProps)
    {
        m_Context = &context;
        m_Size = size;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(m_Context->GetDevice(), &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS)
            C4NDY_FATAL("Failed to create Vulkan buffer.");

        AllocateMemory(memProps);
    }

    void VulkanBuffer::Shutdown()
    {
        if (m_MappedData) Unmap();
        vkDestroyBuffer(m_Context->GetDevice(), m_Buffer, nullptr);
        vkFreeMemory(m_Context->GetDevice(), m_Memory, nullptr);
        C4NDY_INFO("Vulkan buffer destroyed.");
    }

    void VulkanBuffer::AllocateMemory(VkMemoryPropertyFlags properties)
    {
        VkMemoryRequirements memReqs;
        vkGetBufferMemoryRequirements(m_Context->GetDevice(), m_Buffer, &memReqs);

        VkPhysicalDeviceMemoryProperties memProps;
        vkGetPhysicalDeviceMemoryProperties(m_Context->GetPhysicalDevice(), &memProps);

        u32 memTypeIndex = UINT32_MAX;
        for (u32 i = 0; i < memProps.memoryTypeCount; i++)
        {
            if ((memReqs.memoryTypeBits & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & properties) == properties)
            {
                memTypeIndex = i;
                break;
            }
        }

        if (memTypeIndex == UINT32_MAX)
            C4NDY_FATAL("Failed to find suitable memory type for buffer.");

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReqs.size;
        allocInfo.memoryTypeIndex = memTypeIndex;

        if (vkAllocateMemory(m_Context->GetDevice(), &allocInfo, nullptr, &m_Memory) != VK_SUCCESS)
            C4NDY_FATAL("Failed to allocate Vulkan buffer memory.");

        vkBindBufferMemory(m_Context->GetDevice(), m_Buffer, m_Memory, 0);
    }

    void VulkanBuffer::Map()
    {
        vkMapMemory(m_Context->GetDevice(), m_Memory, 0, m_Size, 0, &m_MappedData);
    }

    void VulkanBuffer::Unmap()
    {
        vkUnmapMemory(m_Context->GetDevice(), m_Memory);
        m_MappedData = nullptr;
    }

    void VulkanBuffer::WriteData(const void* data, VkDeviceSize size)
    {
        if (!m_MappedData)
        {
            C4NDY_WARN("Attempted to write to unmapped buffer!");
            return;
        }
        memcpy(m_MappedData, data, (size_t)size);
    }
}