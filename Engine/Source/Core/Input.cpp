#include "Input.h"
#include <cstring>

namespace C4ndy
{
    bool Input::s_Keys[k_MaxKeys] = {};
    bool Input::s_KeysPrev[k_MaxKeys] = {};
    bool Input::s_Buttons[k_MaxButtons] = {};
    bool Input::s_ButtonPrev[k_MaxButtons] = {};
    f32  Input::s_MouseX    = 0.0f;
    f32  Input::s_MouseY    = 0.0f;
    f32  Input::s_MousePX   = 0.0f;
    f32  Input::s_MousePY   = 0.0f;
    f32  Input::s_ScrollDelta = 0.0f;

    void Input::Update()
    {
        memcpy(s_KeysPrev, s_Keys, sizeof(s_Keys));
        memcpy(s_ButtonPrev, s_Buttons, sizeof(s_Buttons));
        s_MousePX = s_MouseX;
        s_MousePY = s_MouseY;
        s_ScrollDelta = 0.0f;
    }

    bool Input::IsKeyDown(i32 key)
    {
        if (key < 0 || key >= static_cast<i32>(k_MaxKeys)) return false;
        return s_Keys[key];
    }

    bool Input::IsKeyPressed(i32 key)
    {
        if (key < 0 || key >= static_cast<i32>(k_MaxKeys)) return false;
        return s_Keys[key] && !s_KeysPrev[key];
    }

    bool Input::IsKeyReleased(i32 key)
    {
        if (key < 0 || key >= static_cast<i32>(k_MaxKeys)) return false;
        return !s_Keys[key] && s_KeysPrev[key];
    }

    bool Input::IsMouseButtonDown(i32 btn)
    {
        if (btn < 0 || btn >= static_cast<i32>(k_MaxButtons)) return false;
        return s_Buttons[btn];
    }

    bool Input::IsMouseButtonPressed(i32 btn)
    {
        if (btn < 0 || btn >= static_cast<i32>(k_MaxButtons)) return false;
        return s_Buttons[btn] && !s_ButtonPrev[btn];
    }

    bool Input::IsMouseButtonReleased(i32 btn)
    {
        if (btn < 0 || btn >= static_cast<i32>(k_MaxButtons)) return false;
        return !s_Buttons[btn] && s_ButtonPrev[btn];
    }

    f32 Input::GetMouseX()  { return s_MouseX; }
    f32 Input::GetMouseY()  { return s_MouseY; }
    f32 Input::GetMouseDeltaX() { return s_MouseX - s_MousePX; }
    f32 Input::GetMouseDeltaY() { return s_MouseY - s_MousePY; }
    f32 Input::GetScrollDelta() { return s_ScrollDelta; }

    void Input::OnKeyDown(i32 key)
    {
        if (key >= 0 && key < static_cast<i32>(k_MaxKeys))
            s_Keys[key] = true;
    }

    void Input::OnKeyUp(i32 key)
    {
        if (key >= 0 && key < static_cast<i32>(k_MaxKeys))
            s_Keys[key] = false;
    }

    void Input::OnMouseMove(f32 x, f32 y)
    {
        s_MouseX = x;
        s_MouseY = y;
    }

    void Input::OnMouseButtonDown(i32 btn)
    {
        if (btn >= 0 && btn < static_cast<i32>(k_MaxButtons))
            s_Buttons[btn] = true;
    }

    void Input::OnMouseButtonUp(i32 btn)
    {
        if (btn >= 0 && btn < static_cast<i32>(k_MaxButtons))
            s_Buttons[btn] = false;
    }

    void Input::OnMouseScroll(f32 delta)
    {
        s_ScrollDelta += delta;
    }
}