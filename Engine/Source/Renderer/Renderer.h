#pragma once
#include <Core/Core.h>
#include <Core/Window.h>
#include <RHI/RHI.h>

namespace C4ndy::Renderer
{
    class Renderer
    {
    public:
        Renderer();
        C4NDY_NON_COPYABLE_MOVEABLE(Renderer);

        void Init(Window& window);
        void Shutdown();

        bool BeginFrame();
        void EndFrame();
        void OnResize(u32 width, u32 height);

        VkCommandBuffer GetCommandBuffer() const { return m_CommandBuffer.GetCommandBuffer(); }

    private:
        Window* m_Window = nullptr;
        
        RHI::VulkanContext m_Context;
        RHI::VulkanSwapchain m_Swapchain;
        RHI::VulkanRenderPass m_RenderPass;
        RHI::VulkanPipeline m_Pipeline;
        RHI::VulkanCommandBuffer m_CommandBuffer;
        RHI::VulkanImage m_DepthImage;
        std::vector<VkFramebuffer> m_Framebuffers;
        u32 m_CurrentImageIndex = 0;

        struct ShaderProgram
        {
            Scope<RHI::VulkanShader> vert;
            Scope<RHI::VulkanShader> frag;
        };

        std::unordered_map<std::string, ShaderProgram> m_Shaders;

        void LoadShaders(const std::string& shaderDir);
        void CreateFrameBuffers();
        void DestroyFrameBuffers();
    };
}