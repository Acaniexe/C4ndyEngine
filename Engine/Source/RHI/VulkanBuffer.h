#pragma once
#include <Core/Core.h>
#include "VulkanContext.h"


namespace C4ndy::RHI
{
    class VulkanBuffer
    {
    public:
        VulkanBuffer();
        C4NDY_NON_COPYABLE_MOVEABLE(VulkanBuffer);

        void Init(VulkanContext&, VkDeviceSize size, VkBufferUsageFlags, VkMemoryPropertyFlags);
        void Shutdown();

        void Map();
        void Unmap();
        void WriteData(const void* data, VkDeviceSize size);

        VkBuffer GetBuffer() const { return m_Buffer; }
        VkDeviceSize GetSize() const { return m_Size; }

    private:
        VulkanContext* m_Context = nullptr;
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_Memory = VK_NULL_HANDLE;
        VkDeviceSize m_Size = 0;
        void* m_MappedData = nullptr;
        void AllocateMemory(VkMemoryPropertyFlags);
    };
}