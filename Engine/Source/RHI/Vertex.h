#pragma once
#include <Core/Core.h>
#include <Math/Math.h>
#include <Vulkan/vulkan.h>
#include <array>
#include <cstddef>

namespace C4ndy::RHI
{
    struct Vertex
    {
        Math::Vec3 position;
        Math::Vec3 normal;
        Math::Vec3 uv;
    };

    inline VkVertexInputBindingDescription GetVertexBinding()
    {
        VkVertexInputBindingDescription binding{};
        binding.binding = 0;
        binding.stride = sizeof(Vertex);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding;
    }

    inline std::array<VkVertexInputAttributeDescription, 3> GetVertexAttributes()
    {
        std::array<VkVertexInputAttributeDescription, 3> attrs{};
        attrs[0] = { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, (u32)offsetof(Vertex, position) };
        attrs[1] = { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, (u32)offsetof(Vertex, normal) };
        attrs[2] = { 2, 0, VK_FORMAT_R32G32_SFLOAT, (u32)offsetof(Vertex, uv) };
        return attrs;
    }
}