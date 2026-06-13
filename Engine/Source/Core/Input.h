#pragma once
#include "Core.h"

namespace C4ndy
{
    namespace Key
    {
        constexpr i32 Space     = 0x20;
        constexpr i32 Enter     = 0x0D;
        constexpr i32 Escape    = 0x1B;
        constexpr i32 Tab       = 0x09;
        constexpr i32 Backspace = 0x08;
        constexpr i32 Delete    = 0x2E;

        constexpr i32 Left      = 0x25;
        constexpr i32 Up        = 0x26;
        constexpr i32 Right     = 0x27;
        constexpr i32 Down      = 0x28;

        constexpr i32 Shift     = 0x10;
        constexpr i32 Control   = 0x11;
        constexpr i32 Alt       = 0x12;

        //A–Z
        constexpr i32 A = 'A'; constexpr i32 B = 'B'; constexpr i32 C = 'C';
        constexpr i32 D = 'D'; constexpr i32 E = 'E'; constexpr i32 F = 'F';
        constexpr i32 G = 'G'; constexpr i32 H = 'H'; constexpr i32 I = 'I';
        constexpr i32 J = 'J'; constexpr i32 K = 'K'; constexpr i32 L = 'L';
        constexpr i32 M = 'M'; constexpr i32 N = 'N'; constexpr i32 O = 'O';
        constexpr i32 P = 'P'; constexpr i32 Q = 'Q'; constexpr i32 R = 'R';
        constexpr i32 S = 'S'; constexpr i32 T = 'T'; constexpr i32 U = 'U';
        constexpr i32 V = 'V'; constexpr i32 W = 'W'; constexpr i32 X = 'X';
        constexpr i32 Y = 'Y'; constexpr i32 Z = 'Z';

        //0–9
        constexpr i32 D0 = '0'; constexpr i32 D1 = '1'; constexpr i32 D2 = '2';
        constexpr i32 D3 = '3'; constexpr i32 D4 = '4'; constexpr i32 D5 = '5';
        constexpr i32 D6 = '6'; constexpr i32 D7 = '7'; constexpr i32 D8 = '8';
        constexpr i32 D9 = '9';

        //F-keys
        constexpr i32 F1  = 0x70; constexpr i32 F2  = 0x71; constexpr i32 F3  = 0x72;
        constexpr i32 F4  = 0x73; constexpr i32 F5  = 0x74; constexpr i32 F6  = 0x75;
        constexpr i32 F7  = 0x76; constexpr i32 F8  = 0x77; constexpr i32 F9  = 0x78;
        constexpr i32 F10 = 0x79; constexpr i32 F11 = 0x7A; constexpr i32 F12 = 0x7B;
    }

    namespace Mouse
    {
        constexpr i32 Left   = 0;
        constexpr i32 Right  = 1;
        constexpr i32 Middle = 2;
    }

    class Input
    {
    public:
        static bool IsKeyDown(i32 keyCode);
        static bool IsKeyPressed(i32 keyCode);
        static bool IsKeyReleased(i32 keyCode);

        static bool IsMouseButtonDown(i32 button);
        static bool IsMouseButtonPressed(i32 button);
        static bool IsMouseButtonReleased(i32 button);
        static f32 GetMouseX();
        static f32 GetMouseY();
        static f32 GetMouseDeltaX();
        static f32 GetMouseDeltaY();
        static f32 GetScrollDelta();

        static void Update();
        static void OnKeyDown(i32 key);
        static void OnKeyUp(i32 key);
        static void OnMouseMove(f32 x, f32 y);
        static void OnMouseButtonDown(i32 btn);
        static void OnMouseButtonUp(i32 btn);
        static void OnMouseScroll(f32 delta);

    private:
        static constexpr usize k_MaxKeys = 256;
        static constexpr usize k_MaxButtons = 8;

        static bool s_Keys[k_MaxKeys];
        static bool s_KeysPrev[k_MaxKeys];

        static bool s_Buttons[k_MaxButtons];
        static bool s_ButtonPrev[k_MaxButtons];

        static f32 s_MouseX, s_MouseY;
        static f32 s_MousePX, s_MousePY;
        static f32 s_ScrollDelta;
    };
}