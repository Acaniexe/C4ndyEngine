#pragma once
#include "MathUtils.h"
#include "Vec3.h"

namespace C4ndy::Math
{
    struct Mat3
    {
        Vec3 col[3];

        constexpr Mat3() : col{ Vec3(1,0,0), Vec3(0,1,0), Vec3(0,0,1) } {}
        constexpr Mat3(const Vec3& c0, const Vec3& c1, const Vec3& c2) : col{c0, c1, c2} {}

        constexpr Vec3 operator*(const Vec3& v) const 
        {
            return {
                col[0].x*v.x + col[1].x*v.y + col[2].x*v.z,
                col[0].y*v.x + col[1].y*v.y + col[2].y*v.z,
                col[0].z*v.x + col[1].z*v.y + col[2].z*v.z
            };
        }

        constexpr Mat3 operator*(const Mat3& r) const
        {
            return {
                *this * r.col[0],
                *this * r.col[1],
                *this * r.col[2]
            };
        }

        constexpr Mat3 Transposed() const
        {
            return {
                Vec3(col[0].x, col[1].x, col[2].x),
                Vec3(col[0].y, col[1].y, col[2].y),
                Vec3(col[0].z, col[1].z, col[2].z)
            };
        }

        constexpr f32 Determinant() const
        {
            return col[0].x * (col[1].y*col[2].z - col[2].y*col[1].z)
                 - col[1].x * (col[0].y*col[2].z - col[2].y*col[0].z)
                 + col[2].x * (col[0].y*col[1].z - col[1].y*col[0].z);
        }

        constexpr Mat3 Inversed() const
        {
            f32 det = Determinant();
            if (NearlyZero(det)) return Mat3();

            f32 invDet = 1.f / det;

            return {
                Vec3(
                    (col[1].y*col[2].z - col[2].y*col[1].z) * invDet,
                    (col[2].y*col[0].z - col[0].y*col[2].z) * invDet,
                    (col[0].y*col[1].z - col[1].y*col[0].z) * invDet
                ),
                Vec3(
                    (col[2].x*col[1].z - col[1].x*col[0].z) * invDet,
                    (col[0].x*col[2].z - col[2].x*col[0].z) * invDet,
                    (col[1].x*col[0].z - col[0].x*col[1].z) * invDet
                ),
                Vec3(
                    (col[1].x*col[2].y - col[2].x*col[1].y) * invDet,
                    (col[2].x*col[0].y - col[0].x*col[2].y) * invDet,
                    (col[0].x*col[1].y - col[1].x*col[0].y) * invDet
                )
            };
        }
    };
}