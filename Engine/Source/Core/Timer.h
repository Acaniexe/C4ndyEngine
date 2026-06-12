#pragma once
#include "Core.h"
#include <chrono>

using TimePoint = std::chrono::high_resolution_clock::time_point;

namespace C4ndy
{
    class Timer
    {
    public:
        Timer();

        void Tick();

        f32 DeltaTime()    const { return m_DeltaTime; }

        f64 ElapsedTime()  const { return m_ElapsedTime; }

        f32 FPS()          const { return m_DeltaTime > 0.0f ? 1.0f / m_DeltaTime : 0.0f; }

        void Reset();

    private:
        TimePoint m_LastTime;
        TimePoint m_StartTime;

        f32 m_DeltaTime  = 0.0f;
        f64 m_ElapsedTime = 0.0f;

        static constexpr f32 k_MaxDelta = 0.1f;
    };
}