#include "Renderer.h"
#include <Core/Logger.h>
#include <windows.h>
#include <array>
#include <filesystem>

namespace C4ndy::Renderer
{

    Renderer::Renderer() = default;

    void Renderer::Init(Window& window)
    {
        m_Window = &window;
        m_Context.Init(window.GetNativeHandle());

        m_Swapchain.Init(m_Context, window.GetWidth(), window.GetHeight());

        m_DepthImage.Init(
            m_Context, window.GetWidth(), window.GetHeight(),
            VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT
        );

        m_RenderPass.Init(m_Context, m_Swapchain.GetFormat(), VK_FORMAT_D32_SFLOAT);

        CreateFrameBuffers();

        char exePath[MAX_PATH];
        GetModuleFileNameA(nullptr, exePath, MAX_PATH);
        std::string rootPath = std::filesystem::path(exePath)
            .parent_path()
            .parent_path()
            .parent_path()
            .string();
        LoadShaders(rootPath + "/Engine/Shaders");

        m_Pipeline.Init(m_Context);
        m_Pipeline.SetShaders(*m_Shaders["triangle"].vert, *m_Shaders["triangle"].frag);
        m_Pipeline.SetRenderPass(m_RenderPass);
        m_Pipeline.SetExtent(m_Swapchain.GetExtent());
        m_Pipeline.Build();

        m_CommandBuffer.Init(m_Context);

        C4NDY_INFO("Renderer Initialised.");
    }

    void Renderer::Shutdown()
    {
        vkDeviceWaitIdle(m_Context.GetDevice());

        m_CommandBuffer.Shutdown();
        m_Pipeline.Shutdown();
        for (auto& [name, program] : m_Shaders)
        {
            program.vert->Shutdown();
            program.frag->Shutdown();
        }
        m_Shaders.clear();
        DestroyFrameBuffers();
        m_RenderPass.Shutdown();
        m_DepthImage.Shutdown();
        m_Swapchain.Shutdown();
        m_Context.Shutdown();

        C4NDY_INFO("Renderer shutdown.");
    }

    void Renderer::LoadShaders(const std::string& shaderDir)
    {
        if (!std::filesystem::exists(shaderDir))
        {
            C4NDY_WARN("Shader directory not found: " + shaderDir);
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(shaderDir))
        {
            std::string path = entry.path().string();

            if (path.size() < 9 || path.substr(path.size() - 9) != ".vert.spv")
                continue;
            std::string base = path.substr(0, path.size() - 9);
            std::string name = entry.path().stem().stem().string();
            std::string fragPath = base + ".frag.spv";

            if (!std::filesystem::exists(fragPath))
            {
                C4NDY_WARN("No matching .frag.spv for: " + path);
                continue;
            }

            ShaderProgram program;
            program.vert = MakeScope<RHI::VulkanShader>();
            program.frag = MakeScope<RHI::VulkanShader>();
            program.vert->Init(m_Context, path, VK_SHADER_STAGE_VERTEX_BIT);
            program.frag->Init(m_Context, fragPath, VK_SHADER_STAGE_FRAGMENT_BIT);
            m_Shaders.emplace(name, std::move(program));

            C4NDY_INFO("Loaded shader pair: " + name);
        }
    }

    void Renderer::CreateFrameBuffers()
    {
        m_Framebuffers.resize(m_Swapchain.GetImageCount());

        for (u32 i = 0; i < m_Swapchain.GetImageCount(); i++)
        {
            std::array<VkImageView, 2> attachments = {
                m_Swapchain.GetImageView(i), m_DepthImage.GetView()
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_RenderPass.GetRenderPass();
            framebufferInfo.attachmentCount = (u32)attachments.size();
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = m_Swapchain.GetExtent().width;
            framebufferInfo.height = m_Swapchain.GetExtent().height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_Context.GetDevice(), &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
                C4NDY_FATAL("Failed to create framebuffers!");
        }
        C4NDY_INFO("FrameBuffers created.");
    }

    void Renderer::DestroyFrameBuffers()
    {
        for (auto fb : m_Framebuffers)
            vkDestroyFramebuffer(m_Context.GetDevice(), fb, nullptr);
        m_Framebuffers.clear();
    }

    bool Renderer::BeginFrame()
    {
        m_CommandBuffer.WaitForFence();

        m_CurrentImageIndex = m_Swapchain.AcquireNextImage(
            m_CommandBuffer.GetImageAvailableSemaphore()
        );

        if (m_CurrentImageIndex == UINT32_MAX)
        {
            if (m_Window)
            {
                OnResize(m_Window->GetWidth(), m_Window->GetHeight());
            }
            return false;
        }

        m_CommandBuffer.Reset();
        m_CommandBuffer.Begin();

        m_RenderPass.Begin(m_CommandBuffer.GetCommandBuffer(), m_Framebuffers[m_CurrentImageIndex], m_Swapchain.GetExtent());

        m_Pipeline.Bind(m_CommandBuffer.GetCommandBuffer());

        VkViewport viewPort{};
        viewPort.x = 0.0f;
        viewPort.y = 0.0f;
        viewPort.width = (f32)m_Swapchain.GetExtent().width;
        viewPort.height = (f32)m_Swapchain.GetExtent().height;
        viewPort.minDepth = 0.0f;
        viewPort.maxDepth = 1.0f;
        vkCmdSetViewport(m_CommandBuffer.GetCommandBuffer(), 0, 1, &viewPort);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_Swapchain.GetExtent();
        vkCmdSetScissor(m_CommandBuffer.GetCommandBuffer(), 0, 1, &scissor);

        return true;
    }

    void Renderer::EndFrame()
    {
        m_RenderPass.End(m_CommandBuffer.GetCommandBuffer());
        m_CommandBuffer.End();
        m_CommandBuffer.Submit();
        
        if (m_Swapchain.Present(m_CommandBuffer.GetRenderFinishedSemaphore(), m_CurrentImageIndex))
        {
            if (m_Window)
            {
                OnResize(m_Window->GetWidth(), m_Window->GetHeight());
            }
        }
    }

    void Renderer::OnResize(u32 width, u32 height)
    {
        if (width == 0 || height == 0)
            return;
        
        vkDeviceWaitIdle(m_Context.GetDevice());
        DestroyFrameBuffers();
        m_Swapchain.Recreate(width, height);

        const VkExtent2D extent = m_Swapchain.GetExtent();

        m_DepthImage.Shutdown();

        m_DepthImage.Init(m_Context, extent.width, extent.height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
        CreateFrameBuffers();

        C4NDY_INFO("Renderer resized to " + std::to_string(extent.width) + "x" + std::to_string(extent.height));
    }
}