#include "Timer.h"

namespace C4ndy
{
    Timer::Timer()
    {
        Reset();
    }

    void Timer::Tick()
    {
        auto now = std::chrono::high_resolution_clock::now();
        m_DeltaTime = std::chrono::duration<float>(now - m_LastTime).count();
        if (m_DeltaTime > k_MaxDelta)
            m_DeltaTime = k_MaxDelta;

        m_LastTime = now;
        m_ElapsedTime += m_DeltaTime;
    }

    void Timer::Reset()
    {
        auto now = std::chrono::high_resolution_clock::now();
        m_LastTime = now;
        m_StartTime = now;
        m_ElapsedTime = 0.0;
    }
}