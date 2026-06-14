#include "Core.h"

#if defined(C4NDY_PLATFORM_WINDOWS)

#include "Window.h"
#include "Input.h"
#include "Logger.h"
#include <Windows.h>

namespace C4ndy
{
    class WindowsWindow : public Window
    {
    public:
        explicit WindowsWindow(const WindowDesc& desc)
            : m_Width(desc.Width), m_Height(desc.Height), m_VSync(desc.VSync)
        {
            WNDCLASSEX wc = {};
            wc.cbSize = sizeof(WNDCLASSEX);
            wc.lpfnWndProc = WindowsWindow::WndProcStatic;
            wc.hInstance = GetModuleHandle(nullptr);
            wc.lpszClassName = L"C4ndyWindowClass";
            wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

            std::wstring title(desc.Title.begin(), desc.Title.end());

            ATOM classAtom = RegisterClassEx(&wc);
            if (classAtom == 0) 
                Logger::Get().Error("RegisterClass failed: " + std::to_string(GetLastError()));

            m_Hwnd = CreateWindowEx(
                0, L"C4ndyWindowClass", title.c_str(),
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT,
                (int)desc.Width, (int)desc.Height,
                nullptr, nullptr, GetModuleHandle(nullptr), this
            );

            if (m_Hwnd == nullptr)
                Logger::Get().Error("CreatedWindowEx failed: " + std::to_string(GetLastError()));
            else
                Logger::Get().Info("Window Created! HWND valid.");


            ShowWindow(m_Hwnd, SW_SHOW);
        }

        ~WindowsWindow() override
        {
            DestroyWindow(m_Hwnd);
        }

        void OnUpdate() override
        {
            MSG msg;
            while (PeekMessage(&msg, m_Hwnd, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        u32 GetWidth() const override { return m_Width; }
        u32 GetHeight() const override { return m_Height; }
        bool ShouldClose() const override { return m_ShouldClose; }
        void SetEventCallback(const EventCallbackFn& cb) override { m_EventCallback = cb; }
        void SetVSync(bool enabled) override { m_VSync = enabled; }
        bool IsVSync() const override { return m_VSync; }

    private:
        static LRESULT CALLBACK WndProcStatic(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            WindowsWindow* window;
            if (msg == WM_NCCREATE)
            {
                auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
                window = reinterpret_cast<WindowsWindow*>(cs->lpCreateParams);
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
            }
            else
            {
                window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            }

            if (window)
                return window->HandleMessage(hwnd, msg, wParam, lParam);
            
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }

        LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            switch (msg)
            {
                case WM_CLOSE:
                {
                       m_ShouldClose = true;
                    WindowCloseEvent e;
                    if (m_EventCallback) m_EventCallback(e);
                    return 0;
                }
                
                case WM_SIZE:
                {
                    m_Width = LOWORD(lParam);
                    m_Height = HIWORD(lParam);
                    WindowResizeEvent e(m_Width, m_Height);
                    if (m_EventCallback) m_EventCallback(e);
                    return 0;
                }

                case WM_KEYDOWN:
                {
                    Input::OnKeyDown((i32)wParam);
                    KeyPressedEvent e((i32)wParam, false);
                    if (m_EventCallback) m_EventCallback(e);
                    return 0;
                }

                case WM_KEYUP:
                {
                    Input::OnKeyUp((i32)wParam);
                    KeyReleasedEvent e((i32)wParam);
                    if (m_EventCallback) m_EventCallback(e);
                    return 0;
                }

                case WM_LBUTTONDOWN:
                {
                    Input::OnMouseButtonDown(Mouse::Left);
                    MouseButtonPressedEvent e(Mouse::Left);
                    if (m_EventCallback) m_EventCallback(e);
                    return 0;
                }

                case WM_LBUTTONUP:
                {
                    Input::OnMouseButtonUp(Mouse::Left);
                    MouseButtonReleasedEvent e(Mouse::Left);
                    if (m_EventCallback) m_EventCallback(e);
                    return 0;
                }

                case WM_RBUTTONDOWN:
                {
                    Input::OnMouseButtonDown(Mouse::Right);
                    MouseButtonPressedEvent e(Mouse::Right);
                    if (m_EventCallback) m_EventCallback(e);
                    return 0;
                }

                case WM_RBUTTONUP:
                {
                    Input::OnMouseButtonUp(Mouse::Right);
                    MouseButtonReleasedEvent e(Mouse::Right);
                    if (m_EventCallback) m_EventCallback(e);
                    return 0;
                }

                case WM_MOUSEMOVE:
                {
                    f32 x = (f32)LOWORD(lParam);
                    f32 y = (f32)HIWORD(lParam);
                    Input::OnMouseMove(x, y);
                    MouseMovedEvent e(x, y);
                    if (m_EventCallback) m_EventCallback(e);
                    return 0;
                }

                case WM_MOUSEWHEEL:
                {
                    f32 delta = (f32)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
                    Input::OnMouseScroll(delta);
                    MouseScrolledEvent e(0.0f, delta);
                    if (m_EventCallback) m_EventCallback(e);
                    return 0;
                }
            }
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }

        HWND m_Hwnd = nullptr;
        u32 m_Width, m_Height;
        bool m_VSync;
        bool m_ShouldClose = false;
        EventCallbackFn m_EventCallback;      
    };

    Scope<Window> Window::Create(const WindowDesc& desc)
    {
        return MakeScope<WindowsWindow>(desc);
    }
}

#endif