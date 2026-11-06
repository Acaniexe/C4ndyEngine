#pragma once
#include <Math/Math.h>

namespace C4ndy::Renderer
{
    class Camera
    {
    public:
        void SetAspect(f32 aspect);
        void Rotate(f32 deltaYaw, f32 deltaPitch);
        void Move(const Math::Vec3& offset);
        Math::Vec3 GetForward() const;
        Math::Vec3 GetRight() const;
        Math::Mat4 GetView() const;
        Math::Mat4 GetProjection() const;
    private:
        Math::Vec3 m_Position = Math::Vec3(0.f, 0.f, 2.f);
        f32 m_Yaw = 0.f;
        f32 m_Pitch = 0.f;
        f32 m_Fov = Math::ToRadians(60.f);
        f32 m_Aspect = 16.f / 9.f;
        f32 m_Near = 0.1f;
        f32 m_Far = 100.0f;
    };
}