#pragma once
#include <Core/Core.h>
#include <cmath>
#include <limits>

namespace C4ndy::Math
{
    inline constexpr f64 PI = 3.14159265358979323846;
    inline constexpr f64 TWO_PI = PI * 2.0;
    inline constexpr f64 HALF_PI = PI * 0.5;
    inline constexpr f64 INV_PI = 1.0 / PI;
    inline constexpr f64 DEG2RAD = PI / 180.0;
    inline constexpr f64 RAD2DEG = 180.0 / PI;
    inline constexpr f32 EPSILON = 1e-6f;
    inline constexpr f32 INF = std::numeric_limits<f32>::infinity();
    inline constexpr f32 NEG_INF = -std::numeric_limits<f32>::infinity();

    template<typename T> constexpr T ToRadians(T deg) { return static_cast<T>(deg * DEG2RAD); }
    template<typename T> constexpr T ToDegrees(T rad) { return static_cast<T>(rad * RAD2DEG); }

    template<typename T> constexpr T Abs(T x) { return x < T(0) ? -x : x; }
    template<typename T> constexpr T Min(T a, T b) { return a < b ? a : b; }
    template<typename T> constexpr T Max(T a, T b) { return a > b ? a : b; }
    template<typename T> constexpr T Clamp(T x, T lo, T hi) { return Min(Max(x, lo), hi); }
    template<typename T> constexpr T Saturate(T x) { return Clamp(x, T(0), T(1)); }
    template<typename T> constexpr T Sign(T x) { return x > T(0) ? T(1) : (x < T(0) ? T(-1) : T(0)); }
    template<typename T> constexpr T Square(T x) { return x * x; }

    template<typename T> constexpr T Lerp(T a, T b, f32 t) { return a + (b - a) * t; }
    template<typename T> constexpr T SmoothStep(T edge0, T edge1, T x) {
        T t = Saturate((x - edge0) / (edge1 - edge0));
        return t * t * (T(3) - T(2) * t);
    }

    inline f32 Sqrt(f32 x) { return std::sqrtf(x); }
    inline f32 InvSqrt(f32 x) { return 1.0f / std::sqrtf(x); }
    inline f32 Pow(f32 x, f32 e) { return std::powf(x, e); }
    inline f32 Log(f32 x) { return std::logf(x); }
    inline f32 Log2(f32 x) { return std::log2f(x); }
    inline f32 Exp(f32 x) { return std::expf(x); }
    inline f32 Floor(f32 x) { return std::floorf(x); }
    inline f32 Ceil(f32 x) { return std::ceilf(x); }
    inline f32 Round(f32 x) { return std::roundf(x); }
    inline f32 Frac(f32 x) { return x - floorf(x); }
    inline f32 Mod(f32 x, f32 y) { return std::fmodf(x, y); }

    inline f32 Sin(f32 x) { return std::sinf(x); }
    inline f32 Cos(f32 x) { return std::cosf(x); }
    inline f32 Tan(f32 x) { return std::tanf(x); }
    inline f32 Asin(f32 x) { return std::asinf(x); }
    inline f32 Acos(f32 x) { return std::acosf(x); }
    inline f32 Atan(f32 x) { return std::atanf(x); }
    inline f32 Atan2(f32 y, f32 x) { return std::atan2f(y, x); }
    inline void SinCos(f32 a, f32& s, f32& c) { s = std::sinf(a); c = std::cosf(a); }

    inline bool NearlyEqual(f32 a, f32 b, f32 eps = EPSILON) { return Abs(a - b) <= eps; }
    inline bool NearlyZero(f32 a, f32 eps = EPSILON) { return Abs(a) <= eps; }

    inline bool IsPowerOf2(u32 x) { return x != 0 && (x & (x - 1)) == 0; }
    inline u32 NextPowerOf2(u32 x) {
        if (x == 0) return 1;
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return ++x;
    }
    inline u32 AlignUp(u32 x, u32 align) { return (x + align - 1) & ~(align - 1); }
    inline usize AlignUp(usize x, usize align) { return (x + align - 1) &~(align - 1); }
}