#include "Camera.h"
#include <Core/Logger.h>

namespace C4ndy::Renderer
{
    void Camera::SetAspect(f32 aspect)
    {
        m_Aspect = aspect;
    }

    void Camera::Move(const Math::Vec3& offset)
    {
        m_Position += offset;
    }

    void Camera::Rotate(f32 deltaYaw, f32 deltaPitch)
    {
        m_Yaw += deltaYaw;
        m_Pitch += deltaPitch;

        const f32 limit = 1.55f;
        m_Pitch = Math::Clamp(m_Pitch, -limit, limit);
    }

    Math::Vec3 Camera::GetForward() const
    {
        return Math::Vec3(
                Math::Cos(m_Pitch) * Math::Sin(m_Yaw),
                Math::Sin(m_Pitch),
               -Math::Cos(m_Pitch) * Math::Cos(m_Yaw));
    }

    Math::Vec3 Camera::GetRight() const 
    {
        return Math::Vec3::Cross(GetForward(), Math::Vec3(0.f, 1.f, 0.f)).Normalized();
    }

    Math::Mat4 Camera::GetView() const
    {
        return Math::Mat4::LookAt(m_Position, m_Position + GetForward(), Math::Vec3(0.f, 1.f, 0.f));
    }

    Math::Mat4 Camera::GetProjection() const
    {
        return Math::Mat4::Perspective(m_Fov, m_Aspect, m_Near, m_Far);
    }
}