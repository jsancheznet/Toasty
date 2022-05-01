#pragma once

#include "typedefs.h"

struct mouse
{
    u32 ButtonState;
    u32 PrevButtonState;

    // Use by the camera system, relative to last mouse position
    i32 RelX;
    i32 RelY;

    // Mouse position inside the window
    i32 X;
    i32 Y;

    f32 Sensitivity;
};
