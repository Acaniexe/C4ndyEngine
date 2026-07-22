#pragma once
#include <Core/Core.h>
#include "VulkanContext.h"

namespace C4ndy::RHI
{
    class VulkanImage
    {
    public:
        VulkanImage();
        C4NDY_NON_COPYABLE_MOVEABLE(VulkanImage);

        void Init(VulkanContext&, u32 width, u32 height, VkFormat, VkImageUsageFlags, VkImageAspectFlags);
        void Shutdown();
        void TransitionLayout(VkCommandBuffer, VkImageLayout newLayout);

        VkImage GetImage() const { return m_Image; }
        VkImageView GetView() const { return m_View; }
        VkFormat GetFormat() const { return m_Format; }
        VkExtent2D GetExtent() const { return m_Extent; }

    private:
        VulkanContext* m_Context = nullptr;
        VkImage m_Image = VK_NULL_HANDLE;
        VkDeviceMemory m_Memory = VK_NULL_HANDLE;
        VkImageView m_View = VK_NULL_HANDLE;
        VkFormat m_Format = VK_FORMAT_UNDEFINED;
        VkExtent2D m_Extent{};
        VkImageLayout m_CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        void CreateImage(VkImageUsageFlags usage);
        void AllocateMemory();
        void CreateImageView(VkImageAspectFlags aspectFlags);
    };
}