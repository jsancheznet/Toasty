#pragma once

#include <SDL.h>
#include "typedefs.h"
#include "keyboard.h"

keyboard
CreateKeyboard()
{
    keyboard Result = {0};

    Result.State = SDL_GetKeyboardState(&Result.Numkeys);
    Result.CurrentState = (u8*)Malloc(sizeof(u8) * Result.Numkeys); Assert(Result.CurrentState);
    Result.PrevState = (u8*)Malloc(sizeof(u8) * Result.Numkeys); Assert(Result.PrevState);

    ZeroMemory(Result.CurrentState, sizeof(u8) * Result.Numkeys);
    ZeroMemory(Result.PrevState, sizeof(u8) * Result.Numkeys);

    return Result;
}

void
UpdateKeyboard(keyboard *Keyboard)
{
    memcpy((void*)Keyboard->PrevState, (void*)Keyboard->CurrentState, sizeof(u8) * Keyboard->Numkeys);
    Keyboard->State = SDL_GetKeyboardState(NULL);

    memcpy((void*)Keyboard->CurrentState, (void*)Keyboard->State, sizeof(u8) * Keyboard->Numkeys);
    Keyboard->ModState = SDL_GetModState();
}

b32
KeyIsPressed(keyboard *Keyboard, SDL_Scancode Scancode)
{
    return Keyboard->State[Scancode];
}

b32
KeyIsNotPressed(keyboard *Keyboard, SDL_Scancode Scancode)
{
    return !Keyboard->State[Scancode];
}

b32
KeyWasNotPressed(keyboard *Keyboard, SDL_Scancode Scancode)
{
    return !Keyboard->PrevState[Scancode];
}

b32
KeyIsReleased(keyboard *Keyboard, SDL_Scancode Scancode)
{
    return (!Keyboard->State[Scancode] && Keyboard->PrevState[Scancode]);
}
