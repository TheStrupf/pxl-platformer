#include "input.h"
#include "raylib.h"
#include <stdbool.h>

int btn_state = 0;
int btn_pstate = 0;

// Hardcoded input for now
// Will be changed to be flexible in the future

void btn_update()
{
        btn_pstate = btn_state;
        btn_state = 0;
        if (IsKeyDown(KEY_ENTER))
                btn_state |= BTN_START;
        if (IsKeyDown(KEY_BACKSPACE))
                btn_state |= BTN_SELECT;
        if (IsKeyDown(KEY_A))
                btn_state |= BTN_LEFT;
        if (IsKeyDown(KEY_D))
                btn_state |= BTN_RIGHT;
        if (IsKeyDown(KEY_W))
                btn_state |= BTN_UP;
        if (IsKeyDown(KEY_S))
                btn_state |= BTN_DOWN;
        if (IsKeyDown(KEY_LEFT))
                btn_state |= BTN_FACE_LEFT;
        if (IsKeyDown(KEY_RIGHT))
                btn_state |= BTN_FACE_RIGHT;
        if (IsKeyDown(KEY_UP))
                btn_state |= BTN_FACE_UP;
        if (IsKeyDown(KEY_DOWN))
                btn_state |= BTN_FACE_DOWN;
        if (IsKeyDown(KEY_Q))
                btn_state |= BTN_L_SHOULDER;
        if (IsKeyDown(KEY_R))
                btn_state |= BTN_R_SHOULDER;
}

bool btn_pressed(int b)
{
        return (btn_state & b);
}

bool btn_was_pressed(int b)
{
        return (btn_pstate & b);
}

bool btn_just_pressed(int b)
{
        return (!btn_was_pressed(b) && btn_pressed(b));
}

bool btn_just_released(int b)
{
        return (btn_was_pressed(b) && !btn_pressed(b));
}
