#include "Application.h"
#include "Logger.h"
#include "Input.h"

namespace C4ndy
{
    Application::Application()
    {
        WindowDesc desc;
        desc.Title = "C4ndy Engine";
        desc.Width = 1280;
        desc.Height = 720;

        m_Window = Window::Create(desc);
        m_Window->SetEventCallback([this](Event& e) { OnEvent(e); });

        Logger::Get().Info("Application started.");
    }

    void Application::Run()
    {
        while (m_Running)
        {
            m_Timer.Tick();

            m_Window->OnUpdate();
            Input::Update();

            if (m_Window->ShouldClose())
                m_Running = false;
        }
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(
            [this](WindowCloseEvent& e) { return OnWindowClose(e); });
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        C4NDY_UNUSED(e);
        m_Running = false;
        return true;
    }

    void Application::Close()
    {
        m_Running = false;
    }
}