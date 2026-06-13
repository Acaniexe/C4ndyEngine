#pragma once
#include "Core.h"
#include "Event.h"

namespace C4ndy
{
    struct WindowDesc
    {
        std::string Title = "C4ndy Engine";
        u32 Width = 1280;
        u32 Height = 720;
        bool VSync = true;
    };

    class Window
    {
    public: 
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() = default;

        virtual void OnUpdate() = 0;
        virtual u32 GetWidth() const = 0;
        virtual u32 GetHeight() const = 0;
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        virtual bool ShouldClose() const = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        static Scope<Window> Create(const WindowDesc& desc = WindowDesc());
    };
}