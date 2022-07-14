#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

// Buttons to pretty much conform to SNES inputs
// No analog stick. Maybe in the future, will
// depend on the design

enum buttons {
        BTN_START = 1,
        BTN_SELECT = 2,
        BTN_LEFT = 4,
        BTN_RIGHT = 8,
        BTN_UP = 16,
        BTN_DOWN = 32,
        BTN_FACE_LEFT = 64,
        BTN_FACE_RIGHT = 128,
        BTN_FACE_UP = 256,
        BTN_FACE_DOWN = 512,
        BTN_L_SHOULDER = 1024,
        BTN_R_SHOULDER = 2048,
};

extern int btn_state;
extern int btn_pstate;

void btn_update();

// input: button flags
// returns true only if all flags are set, else false
bool btn_pressed(int);
bool btn_was_pressed(int);
bool btn_just_pressed(int);
bool btn_just_released(int);

#endif