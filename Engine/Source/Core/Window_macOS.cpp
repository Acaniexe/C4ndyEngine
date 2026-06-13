#if defined(C4NDY_PLATFORM_MACOS)

#include "Window.h"
#include "Logger.h"

namespace C4ndy
{
    //TODO: Implement using Cocoa

    Scope<Window> Window::Create(const WindowDesc& desc)
    {
        Logger::Get().Fatal("macOS Windowing not yet implemented!");
        return nullptr;
    }
}

#endif