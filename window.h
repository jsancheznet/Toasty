#pragma once

struct window
{
    SDL_Window *Handle;
    SDL_GLContext Context;

    i32 Width;
    i32 Height;
};
