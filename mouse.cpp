#pragma once

#include "mouse.h"

mouse CreateMouse(f32 Sensitivity)
{
    // Everything is zeroed out but sensitivity
    mouse Result = {0};

    Result.Sensitivity = Sensitivity;

    return Result;
}

void UpdateMouse(mouse *Mouse)
{
    Mouse->PrevButtonState = Mouse->ButtonState;
    Mouse->ButtonState = SDL_GetRelativeMouseState(&Mouse->RelX, &Mouse->RelY);
    Mouse->ButtonState = SDL_GetMouseState(&Mouse->X, &Mouse->Y);
}

b32 ButtonIsPressed(mouse *Mouse, i32 Input)
{
    return Mouse->ButtonState & SDL_BUTTON(Input);
}

b32 ButtonWasNotPressed(mouse *Mouse, i32 Input)
{
    return !Mouse->PrevButtonState & SDL_BUTTON(Input);
}
