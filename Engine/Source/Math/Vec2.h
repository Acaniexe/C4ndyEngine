#pragma once
#include "MathUtils.h"

namespace C4ndy::Math
{
    struct Vec2
    {
        f32 x, y;

        constexpr Vec2() : x(0.f), y(0.f) {}
        constexpr Vec2(f32 s) : x(s), y(s) {}
        constexpr Vec2(f32 x, f32 y) : x(x), y(y) {}

        constexpr Vec2 operator+(const Vec2& r) const { return {x+r.x, y+r.y}; }
        constexpr Vec2 operator-(const Vec2& r) const { return {x-r.x, y-r.y}; }
        constexpr Vec2 operator*(const Vec2& r) const { return {x*r.x, y*r.y}; }
        constexpr Vec2 operator/(const Vec2& r) const { return {x/r.x, y/r.y}; }
        constexpr Vec2 operator*(f32 s) const { return {x*s, y*s}; }
        constexpr Vec2 operator/(f32 s) const { return {x/s, y/s}; }
        constexpr Vec2 operator-() const { return {-x, -y}; }
        constexpr Vec2& operator+=(const Vec2& r) { x+=r.x; y+=r.y; return *this; }
        constexpr Vec2& operator-=(const Vec2& r) { x-=r.x; y-=r.y; return *this; }
        constexpr Vec2& operator*=(const Vec2& r) { x*=r.x; y*=r.y; return *this; }
        constexpr Vec2& operator/=(const Vec2& r) { x/=r.x; y/=r.y; return *this; }
        friend constexpr Vec2 operator*(f32 s, const Vec2& v) { return v * s; }

        static constexpr f32 Dot(const Vec2& a, const Vec2& b) { return a.x*b.x + a.y*b.y; }
        static constexpr f32 Cross(const Vec2& a, const Vec2& b) { return a.x*b.y - a.y*b.x; }

        f32 LengthSq() const { return x*x + y*y; }
        f32 Length() const { return Sqrt(LengthSq()); }

        Vec2 Normalized() const {
            f32 len = Length();
            return NearlyZero(len) ? Vec2(0.f) : *this / len;
        }

        constexpr bool operator==(const Vec2& r) const { return NearlyEqual(x,r.x) && NearlyEqual(y,r.y); }
        constexpr bool operator!=(const Vec2& r) const { return !(*this == r); }

        static f32 Distance(const Vec2& a, const Vec2& b) { return (b-a).Length(); }
        static f32 DistanceSq(const Vec2& a, const Vec2& b) { return (b-a).LengthSq(); }

        static Vec2 Lerp(const Vec2& a, const Vec2& b, f32 t) { 
           return { Math::Lerp(a.x, b.x, t), Math::Lerp(a.y, b.y, t) };
        }

        static Vec2 Reflect(const Vec2& v, const Vec2& n) {
            return v - n * (2.f * Dot(v, n));
        }
    };
}