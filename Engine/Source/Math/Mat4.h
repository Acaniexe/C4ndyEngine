#pragma once
#include "MathUtils.h"
#include "Vec4.h"

namespace C4ndy::Math
{
    struct Mat4
    {
        Vec4 col[4];

        constexpr Mat4() : col{ Vec4(1,0,0,0), Vec4(0,1,0,0), Vec4(0,0,1,0), Vec4(0,0,0,1) } {}
        constexpr Mat4(const Vec4& c0, const Vec4& c1, const Vec4& c2, const Vec4& c3) : col{c0, c1, c2, c3} {}


        constexpr Vec4 operator*(const Vec4& v) const
        {
            return {
                col[0].x*v.x + col[1].x*v.y + col[2].x*v.z + col[3].x*v.w,
                col[0].y*v.x + col[1].y*v.y + col[2].y*v.z + col[3].y*v.w,
                col[0].z*v.x + col[1].z*v.y + col[2].z*v.z + col[3].z*v.w,
                col[0].w*v.x + col[1].w*v.y + col[2].w*v.z + col[3].w*v.w
            };
        }

        constexpr Mat4 operator*(const Mat4& r) const
        {
            return {
                *this * r.col[0],
                *this * r.col[1],
                *this * r.col[2],
                *this * r.col[3]
            };
        }

        constexpr Mat4 operator*(f32 s) const{
            return {
                col[0] * s,
                col[1] * s,
                col[2] * s,
                col[3] * s
            };
        }

        constexpr Mat4 Transposed() const
        {
            return {
                Vec4(col[0].x, col[1].x, col[2].x, col[3].x),
                Vec4(col[0].y, col[1].y, col[2].y, col[3].y),
                Vec4(col[0].z, col[1].z, col[2].z, col[3].z),
                Vec4(col[0].w, col[1].w, col[2].w, col[3].w)
            };
        }

        static constexpr Mat4 Translation(const Vec3& t)
        {
            Mat4 m;
            m.col[3] = Vec4(t.x, t.y, t.z, 1.f);
            return m;
        }

        static constexpr Mat4 Scale(const Vec3& s)
        {
            Mat4 m;
            m.col[0].x = s.x;
            m.col[1].y = s.y;
            m.col[2].z = s.z;
            return m;
        }

        static Mat4 RotationX(f32 angle)
        {
            f32 s, c;
            SinCos(angle, s, c);
            Mat4 m;
            m.col[1] = Vec4(0.f, c, s, 0.f);
            m.col[2] = Vec4(0.f, -s, c, 0.f);
            return m;
        }

        static Mat4 RotationY(f32 angle)
        {
            f32 s, c;
            SinCos(angle, s, c);
            Mat4 m;
            m.col[0] = Vec4(c, 0.f, -s, 0.f);
            m.col[2] = Vec4(s, 0.f, c, 0.f);
            return m;
        }

        static Mat4 RotationZ(f32 angle)
        {
            f32 s, c;
            SinCos(angle, s, c);
            Mat4 m;
            m.col[0] = Vec4(c, s, 0.f, 0.f);
            m.col[1] = Vec4(-s, c, 0.f, 0.f);
            return m;
        }

        static Mat4 Perspective(f32 fov, f32 aspect, f32 near, f32 far)
        {
            f32 tanHalf = Tan(fov * 0.5f);
            Mat4 m = {};
            m.col[0].x = 1.f / (aspect * tanHalf);
            m.col[1].y = 1.f / tanHalf;
            m.col[2].z = -(far + near) / (far - near);
            m.col[2].w = -1.f;
            m.col[3].z = -(2.f * far * near) / (far - near);
            return m;
        }

        static Mat4 LookAt(const Vec3& eye, const Vec3& target, const Vec3& up)
        {
            Vec3 f = (target - eye).Normalized();
            Vec3 r = Vec3::Cross(f, up).Normalized();
            Vec3 u = Vec3::Cross(r, f);

            Mat4 m;
            m.col[0] = Vec4(r.x, u.x, -f.x, 0.f);
            m.col[1] = Vec4(r.y, u.y, -f.y, 0.f);
            m.col[2] = Vec4(r.z, u.z, -f.z, 0.f);
            m.col[3] = Vec4(-Vec3::Dot(r, eye), -Vec3::Dot(u, eye), Vec3::Dot(f, eye), 1.f);
            return m;
        }

        Vec3 TransformPoint(const Vec3& p) const{
            Vec4 r = *this * Vec4(p.x, p.y, p.z, 1.f);
            return {r.x, r.y, r.z};
        }

        Vec3 TransformDirection(const Vec3& d) const
        {
            Vec4 r = *this * Vec4(d.x, d.y, d.z, 0.f);
            return {r.x, r.y, r.z};
        }

        Mat4 Inversed() const
        {
            f32 c00 = col[2].z*col[3].w - col[3].z*col[2].w;
            f32 c02 = col[1].z*col[3].w - col[3].z*col[1].w;
            f32 c03 = col[1].z*col[2].w - col[2].z*col[1].w;
            f32 c04 = col[2].y*col[3].w - col[3].y*col[2].w;
            f32 c06 = col[1].y*col[3].w - col[3].y*col[1].w;
            f32 c07 = col[1].y*col[2].w - col[2].y*col[1].w;
            f32 c08 = col[2].y*col[3].z - col[3].y*col[2].z;
            f32 c10 = col[1].y*col[3].z - col[3].y*col[1].z;
            f32 c11 = col[1].y*col[2].z - col[2].y*col[1].z;
            f32 c12 = col[2].x*col[3].w - col[3].x*col[2].w;
            f32 c14 = col[1].x*col[3].w - col[3].x*col[1].w;
            f32 c15 = col[1].x*col[2].w - col[2].x*col[1].w;
            f32 c16 = col[2].x*col[3].z - col[3].x*col[2].z;
            f32 c18 = col[1].x*col[3].z - col[3].x*col[1].z;
            f32 c19 = col[1].x*col[2].z - col[2].x*col[1].z;
            f32 c20 = col[2].x*col[3].y - col[3].x*col[2].y;
            f32 c22 = col[1].x*col[3].y - col[3].x*col[1].y;
            f32 c23 = col[1].x*col[2].y - col[2].x*col[1].y;

            Vec4 fac0(c00, c00, c02, c03);
            Vec4 fac1(c04, c04, c06, c07);
            Vec4 fac2(c08, c08, c10, c11);
            Vec4 fac3(c12, c12, c14, c15);
            Vec4 fac4(c16, c16, c18, c19);
            Vec4 fac5(c20, c20, c22, c23);

            Vec4 v0(col[1].x, col[0].x, col[0].x, col[0].x);
            Vec4 v1(col[1].y, col[0].y, col[0].y, col[0].y);
            Vec4 v2(col[1].z, col[0].z, col[0].z, col[0].z);
            Vec4 v3(col[1].w, col[0].w, col[0].w, col[0].w);

            Vec4 inv0(v1*fac0 - v2*fac1 + v3*fac2);
            Vec4 inv1(v0*fac0 - v2*fac3 + v3*fac4);
            Vec4 inv2(v0*fac1 - v1*fac3 + v3*fac5);
            Vec4 inv3(v0*fac2 - v1*fac4 + v2*fac5);

            Vec4 signA(+1, -1, +1, -1);
            Vec4 signB(-1, +1, -1, +1);

            Mat4 inv(inv0*signA, inv1*signB, inv2*signA, inv3*signB);

            Vec4 row0(inv.col[0].x, inv.col[1].x, inv.col[2].x, inv.col[3].x);
            Vec4 dot0(col[0]*row0);
            f32  det = (dot0.x + dot0.y) + (dot0.z + dot0.w);

            if (NearlyZero(det)) return Mat4();
            return inv * (1.f / det);
        }
    };
}