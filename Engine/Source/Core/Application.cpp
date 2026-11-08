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

            f32 dt = (f32)m_Timer.DeltaTime();

            if (Input::IsMouseButtonDown(Mouse::Right))
            {
                const f32 sens = 0.003f;
                m_Camera.Rotate(Input::GetMouseDeltaX() * sens, -Input::GetMouseDeltaY() * sens);
            }

            const f32 speed = 3.0f * dt;
            Math::Vec3 fwd = m_Camera.GetForward();
            Math::Vec3 right = m_Camera.GetRight();
            if (Input::IsKeyDown(Key::W)) m_Camera.Move(fwd * speed);
            if (Input::IsKeyDown(Key::S)) m_Camera.Move(fwd * -speed);
            if (Input::IsKeyDown(Key::D)) m_Camera.Move(right * speed);
            if (Input::IsKeyDown(Key::A)) m_Camera.Move(right * -speed);
            if (Input::IsKeyDown(Key::E)) m_Camera.Move(Math::Vec3(0.f, speed, 0.f));
            if (Input::IsKeyDown(Key::Q)) m_Camera.Move(Math::Vec3(0.f, -speed, 0.f));

            if (m_Window->GetHeight() > 0)
                m_Camera.SetAspect((f32)m_Window->GetWidth() / (f32)m_Window->GetHeight());

            if (m_Renderer.BeginFrame())
            {
               // vkCmdDraw(m_Renderer.GetCommandBuffer(), 3, 1, 0, 0);
                f32 t = (f32)m_Timer.ElapsedTime();
                Math::Mat4 model = Math::Mat4::RotationY(t) * Math::Mat4::RotationX(t * 0.5f);
                m_Renderer.SetCamera(m_Camera.GetView(), m_Camera.GetProjection());
                m_Renderer.DrawTestCube(model);
                m_Renderer.DrawLightMarker();
                m_Renderer.EndFrame();
            }

            if (m_Window->ShouldClose())
                m_Running = false;

            Input::Update();
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