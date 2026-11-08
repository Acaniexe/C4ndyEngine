#pragma once
#include <Core/Core.h>
#include <Core/Window.h>
#include <RHI/RHI.h>
#include <Math/Math.h>

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
        void DrawTestCube(const Math::Mat4& model);

        struct CameraData
        {
            Math::Mat4 view;
            Math::Mat4 projection;
            Math::Vec4 lightPos;
        };
        void SetCamera(const Math::Mat4& view, const Math::Mat4& projection);

        struct PushData
        {
            Math::Mat4 model;
            Math::Vec4 params;
        };
        void DrawLightMarker();

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
        RHI::VulkanPipeline m_MeshPipeline;
        RHI::VulkanBuffer m_CubeVertexBuffer;
        RHI::VulkanBuffer m_CubeIndexBuffer;
        RHI::VulkanBuffer m_CameraBuffer;
        RHI::VulkanDescriptor m_CameraDescriptor;

        Math::Vec4 m_LightPos = Math::Vec4(1.0f, 1.0f, 1.5f, 0.f);

        struct ShaderProgram
        {
            Scope<RHI::VulkanShader> vert;
            Scope<RHI::VulkanShader> frag;
        };

        std::unordered_map<std::string, ShaderProgram> m_Shaders;

        void LoadShaders(const std::string& shaderDir);
        void CreateFrameBuffers();
        void DestroyFrameBuffers();
        void DrawCube(const PushData& push);
    };
}