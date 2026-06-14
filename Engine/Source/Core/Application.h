#pragma once
#include "Core.h"
#include "Window.h"
#include "Timer.h"
#include "Event.h"

namespace C4ndy
{
    class Application
    {
    public:
        Application();
        virtual ~Application() = default;

        void Run();
        void OnEvent(Event& e);
        void Close();

    private:
        bool OnWindowClose(WindowCloseEvent& e);
        Scope<Window> m_Window;
        Timer m_Timer;
        bool m_Running = true;
    };
}