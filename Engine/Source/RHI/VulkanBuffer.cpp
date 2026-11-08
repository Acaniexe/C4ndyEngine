#include "VulkanBuffer.h"
#include <Core/Logger.h>
#include <cstring>

namespace C4ndy::RHI
{

    VulkanBuffer::VulkanBuffer() = default;
    
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

    void VulkanBuffer::InitDeviceLocal(VulkanContext& context, const void* data, VkDeviceSize size, VkBufferUsageFlags usage)
    {
        VulkanBuffer staging;
        staging.Init(context, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        staging.Map();
        staging.WriteData(data, size);

        Init(context, size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VkDevice device = context.GetDevice();

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = context.GetGraphicsQueueFamily();
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        VkCommandPool pool = VK_NULL_HANDLE;
        vkCreateCommandPool(device, &poolInfo, nullptr, &pool);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = pool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;
        VkCommandBuffer cmd = VK_NULL_HANDLE;
        vkAllocateCommandBuffers(device, &allocInfo, &cmd);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(cmd, &beginInfo);

        VkBufferCopy region{};
        region.size = size;
        vkCmdCopyBuffer(cmd, staging.GetBuffer(), m_Buffer, 1, &region);

        vkEndCommandBuffer(cmd);

        VkSubmitInfo submit{};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &cmd;
        vkQueueSubmit(context.GetGraphicsQueue(), 1, &submit, VK_NULL_HANDLE);
        vkQueueWaitIdle(context.GetGraphicsQueue());

        vkDestroyCommandPool(device, pool, nullptr);
        staging.Shutdown();
    }
}