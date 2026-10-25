#include "Application.h"
#include "Logger.h"
#include "Input.h"
#include <filesystem>

namespace C4ndy
{
    std::string Application::s_ExecutablePath;

    void Application::SetExecutablePath(const char* path)
    {
        if (path == nullptr || path[0] == '\0')
        {
            s_ExecutablePath.clear();
            C4NDY_WARN("Executable path was empty; Shader path resolution may fail.");
            return;
        }

        s_ExecutablePath = std::filesystem::absolute(path).lexically_normal().string();
        C4NDY_INFO("Executable path: " + s_ExecutablePath);
    }

    Application::Application()
    {
        WindowDesc desc;
        desc.Title = "C4ndy Engine";
        desc.Width = 1280;
        desc.Height = 720;

        m_Window = Window::Create(desc);
        m_Window->SetEventCallback([this](Event& e) { OnEvent(e); });

        m_Renderer.Init(*m_Window);

        Logger::Get().Info("Application started.");
    }

    void Application::Run()
    {
        while (m_Running)
        {
            m_Timer.Tick();
            m_Window->OnUpdate();
            Input::Update();

            if (m_Renderer.BeginFrame())
            {
                vkCmdDraw(m_Renderer.GetCommandBuffer(), 3, 1, 0, 0);
                m_Renderer.EndFrame();
            }

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
        m_Renderer.Shutdown();
        m_Running = false;
    }
}