#pragma once
#include "Vec3.h"
#include "MathUtils.h"

namespace C4ndy::Math
{
    struct Vec4
    {
        f32 x, y, z, w;

        constexpr Vec4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
        constexpr Vec4(f32 s) : x(s), y(s), z(s), w(s) {}
        constexpr Vec4(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w) {}

        constexpr Vec3 XYZ() const { return {x, y, z}; }
        constexpr Vec3 RGB() const { return {x, y, z}; }
        
        constexpr Vec4 operator+(const Vec4& r) const { return {x+r.x, y+r.y, z+r.z, w+r.w}; }
        constexpr Vec4 operator-(const Vec4& r) const { return {x-r.x, y-r.y, z-r.z, w-r.w}; }
        constexpr Vec4 operator*(const Vec4& r) const { return {x*r.x, y*r.y, z*r.z, w*r.w}; }
        constexpr Vec4 operator/(const Vec4& r) const { return {x/r.x, y/r.y, z/r.z, w/r.w}; }
        constexpr Vec4 operator*(f32 s) const { return {x*s, y*s, z*s, w*s}; }
        constexpr Vec4 operator/(f32 s) const { return {x/s, y/s, z/s, w/s}; }
        constexpr Vec4 operator-() const { return {-x, -y, -z, -w}; }
        constexpr Vec4& operator+=(const Vec4& r) { x+=r.x; y+=r.y; z+=r.z; w+=r.w; return *this; }
        constexpr Vec4& operator-=(const Vec4& r) { x-=r.x; y-=r.y; z-=r.z; w-=r.w; return *this; }
        constexpr Vec4& operator*=(const Vec4& r) { x*=r.x; y*=r.y; z*=r.z; w*=r.w; return *this; }
        constexpr Vec4& operator/=(const Vec4& r) { x/=r.x; y/=r.y; z/=r.z; w/=r.w; return *this; }
        friend constexpr Vec4 operator*(f32 s, const Vec4& v) { return v * s; }

        static constexpr f32 Dot(const Vec4& a, const Vec4& b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }
        static constexpr Vec4 Cross(const Vec4& a, const Vec4& b) {
            return { a.y*b.z - a.z*b.y,
                     a.z*b.x - a.x*b.z,
                     a.x*b.y - a.y*b.x,
                     0.f };
        }

        f32 LengthSq() const { return x*x + y*y + z*z + w*w; }
        f32 Length() const { return Sqrt(LengthSq()); }

        Vec4 Normalized() const {
            f32 len = Length();
            return NearlyZero(len) ? Vec4(0.f) : *this / len;
        }

        constexpr bool operator==(const Vec4& r) const { return NearlyEqual(x,r.x) && NearlyEqual(y,r.y) && NearlyEqual(z,r.z) && NearlyEqual(w,r.w); }
        constexpr bool operator!=(const Vec4& r) const { return !(*this == r); }

        static f32 Distance(const Vec4& a, const Vec4& b) { return (b-a).Length(); }
        static f32 DistanceSq(const Vec4& a, const Vec4& b) { return (b-a).LengthSq(); }

        static Vec4 Lerp(const Vec4& a, const Vec4& b, f32 t) {
            return { Math::Lerp(a.x, b.x, t), Math::Lerp(a.y, b.y, t), Math::Lerp(a.z, b.z, t), Math::Lerp(a.w, b.w, t) };
        }

        static Vec4 Reflect(const Vec4& v, const Vec4& n) {
            return v - n * (2.f * Dot(v, n));
        }
    };
}