#include "VulkanImage.h"
#include <Core/Logger.h>

namespace C4ndy::RHI
{
    VulkanImage::VulkanImage() = default;
    
    void VulkanImage::Init(VulkanContext& context, u32 width, u32 height, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectFlags)
    {
        m_Context = &context;
        m_Format = format;
        m_Extent = { width, height };

        CreateImage(usage);
        AllocateMemory();
        CreateImageView(aspectFlags);

        C4NDY_INFO("Vulkan image created.");
    }

    void VulkanImage::Shutdown()
    {
        vkDestroyImageView(m_Context->GetDevice(), m_View, nullptr);
        vkDestroyImage(m_Context->GetDevice(), m_Image, nullptr);
        vkFreeMemory(m_Context->GetDevice(), m_Memory, nullptr);

        C4NDY_INFO("Vulkan image destroyed.");
    }

    void VulkanImage::CreateImage(VkImageUsageFlags usage)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = m_Format;
        imageInfo.extent.width = m_Extent.width;
        imageInfo.extent.height = m_Extent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        if (vkCreateImage(m_Context->GetDevice(), &imageInfo, nullptr, &m_Image) != VK_SUCCESS)
            C4NDY_FATAL("Failed to create vulkan image.");
    }

    void VulkanImage::AllocateMemory()
    {
        VkMemoryRequirements memReqs;
        vkGetImageMemoryRequirements(m_Context->GetDevice(), m_Image, &memReqs);

        VkPhysicalDeviceMemoryProperties memProps;
        vkGetPhysicalDeviceMemoryProperties(m_Context->GetPhysicalDevice(), &memProps);

        u32 memTypeIndex = UINT32_MAX;
        for (u32 i = 0; i < memProps.memoryTypeCount; i++)
        {
            if ((memReqs.memoryTypeBits & ( 1 << i)) && (memProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
            {
                memTypeIndex = i;
                break;
            }
        }

        if (memTypeIndex == UINT32_MAX)
            C4NDY_FATAL("Failed to find suitable GPU memory type for image!");

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReqs.size;
        allocInfo.memoryTypeIndex = memTypeIndex;

        if (vkAllocateMemory(m_Context->GetDevice(), &allocInfo, nullptr, &m_Memory) != VK_SUCCESS)
            C4NDY_FATAL("Failed to allocate Vulkan image memory!");
        
            vkBindImageMemory(m_Context->GetDevice(), m_Image, m_Memory, 0);
    }

    void VulkanImage::CreateImageView(VkImageAspectFlags aspectFlags)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_Image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = m_Format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_Context->GetDevice(), &viewInfo, nullptr, &m_View) != VK_SUCCESS)
            C4NDY_FATAL("Failed to create vulkan image view!");
    }

    void VulkanImage::TransitionLayout(VkCommandBuffer cmd, VkImageLayout newLayout)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = m_CurrentLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = m_Image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        m_CurrentLayout = newLayout;
    }
}