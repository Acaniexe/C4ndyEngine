#pragma once
#include "Core.h"

namespace C4ndy
{
    enum class EventType : u32
    {
        None = 0,

        WindowClose,
        WindowResize,
        WindowFocus,
        WindowLostFocus,

        KeyPressed,
        KeyReleased,
        KeyTyped,

        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseScrolled,
    };

    enum EventCategory : u32
    {
        EventCategoryNone        = 0,
        EventCategoryApplication = C4NDY_BIT(0),
        EventCategoryInput       = C4NDY_BIT(1),
        EventCategoryKeyboard    = C4NDY_BIT(2),
        EventCategoryMouse       = C4NDY_BIT(3),
        EventCategoryMouseButton = C4NDY_BIT(4),
    };

    struct Event
    {
        bool Handled = false;

        virtual EventType     GetType()           const = 0;
        virtual u32           GetCategoryFlags()  const = 0;
        virtual const char*   GetName()           const = 0;

        bool IsInCategory(EventCategory cat) const
        {
            return C4NDY_HAS_FLAG(GetCategoryFlags(), static_cast<u32>(cat));
        }

        virtual ~Event() = default;
    };

    #define C4NDY_EVENT_TYPE(type)                                          \
        static  EventType GetStaticType()        { return EventType::type; } \
        virtual EventType GetType()  const override { return GetStaticType(); } \
        virtual const char* GetName() const override { return #type; }

    #define C4NDY_EVENT_CATEGORY(cat) \
        virtual u32 GetCategoryFlags() const override { return cat; }

    struct WindowCloseEvent : Event
    {
        C4NDY_EVENT_TYPE(WindowClose)
        C4NDY_EVENT_CATEGORY(EventCategoryApplication)
    };

    struct WindowResizeEvent : Event
    {
        u32 Width, Height;
        WindowResizeEvent(u32 w, u32 h) : Width(w), Height(h) {}
        C4NDY_EVENT_TYPE(WindowResize)
        C4NDY_EVENT_CATEGORY(EventCategoryApplication)
    };

    struct KeyEvent : Event
    {
        i32 KeyCode;
        C4NDY_EVENT_CATEGORY(EventCategoryInput | EventCategoryKeyboard)
    protected:
        explicit KeyEvent(i32 key) : KeyCode(key) {}
    };

    struct KeyPressedEvent : KeyEvent
    {
        bool Repeat;
        KeyPressedEvent(i32 key, bool repeat) : KeyEvent(key), Repeat(repeat) {}
        C4NDY_EVENT_TYPE(KeyPressed)
    };

    struct KeyReleasedEvent : KeyEvent
    {
        explicit KeyReleasedEvent(i32 key) : KeyEvent(key) {}
        C4NDY_EVENT_TYPE(KeyReleased)
    };

    struct KeyTypedEvent : Event
    {
        u32 Codepoint;
        explicit KeyTypedEvent(u32 cp) : Codepoint(cp) {}
        C4NDY_EVENT_TYPE(KeyTyped)
        C4NDY_EVENT_CATEGORY(EventCategoryInput | EventCategoryKeyboard)
    };

    struct MouseMovedEvent : Event
    {
        f32 X, Y;
        MouseMovedEvent(f32 x, f32 y) : X(x), Y(y) {}
        C4NDY_EVENT_TYPE(MouseMoved)
        C4NDY_EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)
    };

    struct MouseScrolledEvent : Event
    {
        f32 OffsetX, OffsetY;
        MouseScrolledEvent(f32 ox, f32 oy) : OffsetX(ox), OffsetY(oy) {}
        C4NDY_EVENT_TYPE(MouseScrolled)
        C4NDY_EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)
    };

    struct MouseButtonEvent : Event
    {
        i32 Button;
        C4NDY_EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse | EventCategoryMouseButton)
    protected:
        explicit MouseButtonEvent(i32 btn) : Button(btn) {}
    };

    struct MouseButtonPressedEvent : MouseButtonEvent
    {
        explicit MouseButtonPressedEvent(i32 btn) : MouseButtonEvent(btn) {}
        C4NDY_EVENT_TYPE(MouseButtonPressed)
    };

    struct MouseButtonReleasedEvent : MouseButtonEvent
    {
        explicit MouseButtonReleasedEvent(i32 btn) : MouseButtonEvent(btn) {}
        C4NDY_EVENT_TYPE(MouseButtonReleased)
    };

    class EventDispatcher
    {
    public:
        explicit EventDispatcher(Event& e) : m_Event(e) {}

        template<typename T>
        bool Dispatch(std::function<bool(T&)> fn)
        {
            if (m_Event.GetType() == T::GetStaticType())
            {
                m_Event.Handled = fn(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }

    private:
        Event& m_Event;
    };
}