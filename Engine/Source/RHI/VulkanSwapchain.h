#pragma once
#include <Core/Core.h>
#include "VulkanContext.h"

namespace C4ndy::RHI
{
    class VulkanSwapchain
    {
    public:
        VulkanSwapchain();
        C4NDY_NON_COPYABLE_MOVEABLE(VulkanSwapchain);

        void Init(VulkanContext& context, u32 width, u32 height);
        void Shutdown();
        void Recreate(u32 width, u32 height);
        u32 AcquireNextImage(VkSemaphore imageAvailableSemaphore);
        bool Present(VkSemaphore renderFinishedSemaphore, u32 imageIndex);

        VkImageView GetImageView(u32 index) const { return m_ImageViews[index]; }
        VkFormat GetFormat() const { return m_ImageFormat; }
        VkExtent2D GetExtent() const { return m_Extent; }
        u32 GetImageCount() const { return (u32)m_Images.size(); }

    private:
        VulkanContext* m_Context = nullptr;
        VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
        std::vector<VkImage> m_Images;
        std::vector<VkImageView> m_ImageViews;
        VkFormat m_ImageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D m_Extent{};

        VkSurfaceFormatKHR ChooseSurfaceFormat();
        VkPresentModeKHR ChoosePresentMode();
        VkExtent2D ChooseExtent(u32 width, u32 height);
        void CreateImageViews();
        void CreateSwapchain(u32 width, u32 height);
    };
}