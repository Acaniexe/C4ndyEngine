#include "VulkanSwapchain.h"
#include <Core/Logger.h>
#include <algorithm>

namespace C4ndy::RHI
{
    void VulkanSwapchain::Init(VulkanContext& context, u32 width, u32 height)
    {
        m_Context = &context;
        CreateSwapchain(width, height);
        CreateImageViews();
        C4NDY_INFO("Vulkan Swapchain created.");
    }

    void VulkanSwapchain::Shutdown()
    {
        for (auto view : m_ImageViews)
            vkDestroyImageView(m_Context->GetDevice(), view, nullptr);
        m_ImageViews.clear();

        vkDestroySwapchainKHR(m_Context->GetDevice(), m_Swapchain, nullptr);
        C4NDY_INFO("Vulkan swapchain destroyed");
    }

    void VulkanSwapchain::Recreate(u32 width, u32 height)
    {
        vkDeviceWaitIdle(m_Context->GetDevice());
        Shutdown();
        CreateSwapchain(width, height);
        CreateImageViews();
        C4NDY_INFO("Vulkan swapchain recreated.");
    }

    VkSurfaceFormatKHR VulkanSwapchain::ChooseSurfaceFormat()
    {
        u32 formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_Context->GetPhysicalDevice(), m_Context->GetSurface(), &formatCount, nullptr);
        
        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_Context->GetPhysicalDevice(), m_Context->GetSurface(), &formatCount, formats.data());
            
        for (const auto& format : formats)
        {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
                format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return format;
        }

        return formats[0];
    }

    VkPresentModeKHR VulkanSwapchain::ChoosePresentMode()
    {
        u32 modeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_Context->GetPhysicalDevice(), m_Context->GetSurface(), &modeCount, nullptr);
        
        std::vector<VkPresentModeKHR> modes(modeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_Context->GetPhysicalDevice(), m_Context->GetSurface(), &modeCount, modes.data()); 
        
        for (const auto& mode : modes)
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return mode;
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanSwapchain::ChooseExtent(u32 width, u32 height)
    {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            m_Context->GetPhysicalDevice(), m_Context->GetSurface(), &capabilities); 
        
        if (capabilities.currentExtent.width != UINT32_MAX)
            return capabilities.currentExtent;

        VkExtent2D extent = { width, height };
        extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return extent;
    }

    void VulkanSwapchain::CreateSwapchain(u32 width, u32 height)
    {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            m_Context->GetPhysicalDevice(), m_Context->GetSurface(), &capabilities);
        
        VkSurfaceFormatKHR format = ChooseSurfaceFormat();
        VkPresentModeKHR presentMode = ChoosePresentMode();
        VkExtent2D extent = ChooseExtent(width, height);

        u32 imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
            imageCount = capabilities.maxImageCount;


        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_Context->GetSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = format.format;
        createInfo.imageColorSpace = format.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        u32 queueFamilies[] = {
            m_Context->GetGraphicsQueueFamily(), m_Context->GetPresentQueueFamily()
        };

        if (queueFamilies[0] != queueFamilies[1])
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilies;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_Context->GetDevice(), &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS)
            C4NDY_FATAL("Failed to create vulkan swapchain!");

        vkGetSwapchainImagesKHR(m_Context->GetDevice(), m_Swapchain, &imageCount, nullptr);
        m_Images.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Context->GetDevice(), m_Swapchain, &imageCount, m_Images.data());

        m_ImageFormat = format.format;
        m_Extent = extent;
    }

    void VulkanSwapchain::CreateImageViews()
    {
        m_ImageViews.resize(m_Images.size());

        for (usize i = 0; i < m_Images.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_Images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_ImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_Context->GetDevice(), &createInfo, nullptr, &m_ImageViews[i]) != VK_SUCCESS)
                C4NDY_FATAL("Failed to create swapchain image view!");
        }
    }

    u32 VulkanSwapchain::AcquireNextImage(VkSemaphore imageAvailableSemaphore)
    {
        u32 imageIndex = 0;
        VkResult result = vkAcquireNextImageKHR(m_Context->GetDevice(), m_Swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            C4NDY_WARN("Swapchain out of date during acquire!");
            return UINT32_MAX;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            C4NDY_FATAL("Failed to acquire swapchain image!");
        }

        return imageIndex;
    }

    void VulkanSwapchain::Present(VkSemaphore renderFinishedSemaphore, u32 imageIndex)
    {
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &imageIndex;

        VkResult result = vkQueuePresentKHR(m_Context->GetPresentQueue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
            C4NDY_WARN("Swapchain suboptimal or out of date during present!");
        else if (result != VK_SUCCESS)
            C4NDY_FATAL("Failed to present swapchain image!");
    }
}