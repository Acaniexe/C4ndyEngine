#pragma once
#include "MathUtils.h"
#include "Vec3.h"
#include "Mat4.h"

namespace C4ndy::Math
{
    struct Quaternion
    {
        f32 x, y, z, w;

        constexpr Quaternion() : x(0.f), y(0.f), z(0.f), w(1.f) {}
        constexpr Quaternion(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w) {}

        static Quaternion AngleAxis(const Vec3& axis, f32 angle)
        {
            f32 half = angle * 0.5f;
            f32 s = Sin(half);
            Vec3 a = axis.Normalized();
            return { a.x*s, a.y*s, a.z*s, Cos(half) };
        }

        constexpr Quaternion operator*(const Quaternion& r) const
        {
            return {
                w*r.x + x*r.w + y*r.z - z*r.y,
                w*r.y - x*r.z + y*r.w + z*r.x,
                w*r.z + x*r.y - y*r.x + z*r.w,
                w*r.w - x*r.x - y*r.y - z*r.z
            };
        }

        Quaternion Normalized() const
        {
            f32 len = Sqrt(x*x + y*y + z*z + w*w);
            return NearlyZero(len) ? Quaternion() : Quaternion(x/len, y/len, z/len, w/len);
        }

        constexpr Quaternion Conjugate() const 
        {
            return { -x, -y, -z, w };
        }

        Vec3 RotateVector(const Vec3& v) const
        {
            Vec3 qv(x, y, z);
            Vec3 t = Vec3::Cross(qv, v) * 2.f;
            return v + t * w + Vec3::Cross(qv, t);
        }

        Mat4 ToMatrix() const
        {
            f32 xx=x*x, yy=y*y, zz=z*z;
            f32 xy=x*y, xz=x*z, yz=y*z;
            f32 wx=w*x, wy=w*y, wz=w*z;

            return {
                Vec4(1-2*(yy+zz), 2*(xy+wz), 2*(xz-wy), 0),
                Vec4( 2*(xy-wz), 1-2*(xx+zz), 2*(yz+wx), 0),
                Vec4( 2*(xz+wy), 2*(yz-wx), 1-2*(xx+yy), 0),
                Vec4(0, 0, 0, 1)
            };
        }

        static Quaternion Slerp(const Quaternion& a, const Quaternion& b, f32 t)
        {
            f32 dot = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
            Quaternion bb = dot < 0.f ? Quaternion(-b.x,-b.y,-b.z,-b.w) : b;
            dot = Abs(dot);

            if (dot > 0.9995f)
            {
                return Quaternion(
                    a.x + t*(bb.x-a.x),
                    a.y + t*(bb.y-a.y),
                    a.z + t*(bb.z-a.z),
                    a.w + t*(bb.w-a.w)
                ).Normalized();
            }

            f32 theta0 = Acos(dot);
            f32 theta = theta0 * t;
            f32 s0 = Cos(theta) - dot * Sin(theta) / Sin(theta0);
            f32 s1 = Sin(theta) / Sin(theta0);

            return Quaternion(
                s0*a.x + s1*bb.x,
                s0*a.y + s1*bb.y,
                s0*a.z + s1*bb.z,
                s0*a.w + s1*bb.w
            );
        }
    };
}