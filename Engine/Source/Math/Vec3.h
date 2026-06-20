#pragma once
#include "MathUtils.h"

namespace C4ndy::Math 
{
    struct Vec3
    {
        f32 x, y, z;

        constexpr Vec3() : x(0.f), y(0.f), z(0.f) {}
        constexpr Vec3(f32 s) : x(s), y(s), z(s) {}
        constexpr Vec3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}

        constexpr Vec3 operator+(const Vec3& r) const { return {x+r.x, y+r.y, z+r.z}; }
        constexpr Vec3 operator-(const Vec3& r) const { return {x-r.x, y-r.y, z-r.z}; }
        constexpr Vec3 operator*(const Vec3& r) const { return {x*r.x, y*r.y, z*r.z}; }
        constexpr Vec3 operator/(const Vec3& r) const { return {x/r.x, y/r.y, z/r.z}; }
        constexpr Vec3 operator*(f32 s)         const { return {x*s,   y*s,   z*s};   }
        constexpr Vec3 operator/(f32 s)         const { return {x/s,   y/s,   z/s};   }
        constexpr Vec3 operator-()              const { return {-x,    -y,    -z};    }
        constexpr Vec3& operator+=(const Vec3& r) { x+=r.x; y+=r.y; z+=r.z; return *this; }
        constexpr Vec3& operator-=(const Vec3& r) { x-=r.x; y-=r.y; z-=r.z; return *this; }
        constexpr Vec3& operator*=(const Vec3& r) { x*=r.x; y*=r.y; z*=r.z; return *this; }
        constexpr Vec3& operator/=(const Vec3& r) { x/=r.x; y/=r.y; z/=r.z; return *this; }
        friend constexpr Vec3 operator*(f32 s, const Vec3& v) { return v * s; }

        static constexpr f32 Dot(const Vec3& a, const Vec3& b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
        static constexpr Vec3 Cross(const Vec3& a, const Vec3& b) {
            return { a.y*b.z - a.z*b.y,
                     a.z*b.x - a.x*b.z,
                     a.x*b.y - a.y*b.x };
        }

        f32 LengthSq() const { return x*x + y*y + z*z; }
        f32 Length() const { return Sqrt(LengthSq()); }

        Vec3 Normalized() const {
            f32 len = Length();
            return NearlyZero(len) ? Vec3(0.f) : *this / len;
        }

        constexpr bool operator==(const Vec3& r) const { return NearlyEqual(x,r.x) && NearlyEqual(y,r.y) && NearlyEqual(z,r.z); }
        constexpr bool operator!=(const Vec3& r) const { return !(*this == r); }

        static f32 Distance(const Vec3& a, const Vec3& b) { return (b-a).Length(); }
        static f32 DistanceSq(const Vec3& a, const Vec3& b) { return (b-a).LengthSq(); }

        static Vec3 Lerp(const Vec3& a, const Vec3& b, f32 t) { 
            return { Math::Lerp(a.x, b.x, t), Math::Lerp(a.y, b.y, t), Math::Lerp(a.z, b.z, t) };
        }

        static Vec3 Reflect(const Vec3& v, const Vec3& n) {
            return v - n * (2.f * Dot(v, n));
        }
    };
}