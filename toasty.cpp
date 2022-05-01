#include <stdio.h>

#include <SDL.h>
#include <glad.c>

#include "typedefs.h"
#define OPENGL_DEBUG_MODE 1

#include "clock.cpp"
#include "keyboard.cpp"
#include "mouse.cpp"

struct window
{
    SDL_Window *Handle;
    SDL_GLContext Context;

    i32 Width;
    i32 Height;
};

char *
ReadTextFile(char *Filename)
{
    // NOTE(Jorge): The caller of this function needs to free the allocated pointer!
    Assert(Filename);

    SDL_RWops *RWops = SDL_RWFromFile(Filename, "rb");
    if (RWops == NULL) { return NULL; }

    size_t FileSize = SDL_RWsize(RWops);
    char* Result = (char*)Malloc(FileSize + 1);
    char* Buffer = Result;

    size_t BytesReadTotal = 0, BytesRead = 1;
    while (BytesReadTotal < FileSize && BytesRead != 0)
    {
        BytesRead = SDL_RWread(RWops, Buffer, 1, (FileSize - BytesReadTotal));
        BytesReadTotal += BytesRead;
        Buffer += BytesRead;
    }

    SDL_RWclose(RWops);
    if (BytesReadTotal != FileSize)
    {
        Free(Result);
        return NULL;
    }

    Result[BytesReadTotal] = '\0';

    return Result;
}

window
CreateOpenGLWindow(char *Title, i32 Width, i32 Height)
{
    window Result = {};

    Result.Width = Width;
    Result.Height = Height;

    Result.Handle = SDL_CreateWindow(Title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_OPENGL);
    Assert(Result.Handle);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
#ifdef OPENGL_DEBUG_MODE
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    Result.Context = SDL_GL_CreateContext(Result.Handle);
    if(!gladLoadGL()) { printf("gladLoadGL failed!\n"); }
    SDL_GL_MakeCurrent(Result.Handle, Result.Context);

    SDL_GL_SetSwapInterval(1); // Enable vsync

    glViewport(0, 0, Width, Height);

    i32 MajorVersion, MinorVersion;
    glGetIntegerv(GL_MAJOR_VERSION, &MajorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &MinorVersion);
    printf("OpenGL Version %d.%d\n", MajorVersion, MinorVersion);

    return Result;
}

void ProcessEventQueue(b32 *Running)
{
    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
        switch (Event.type)
        {
            // TODO(Jorge): Alt + Tab windowed fullscreen
            case SDL_KEYDOWN:
            {
                if(Event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                {
                    *Running = 0;
                }

                break;
            }
            case SDL_KEYUP:
            {
                break;
            }
            case SDL_QUIT:
            {
                Running = 0;
                break;
            }
            default:
            {
                break;
            }
        }
    }

}

int main(int Argc, char ** Argv)
{
    Argc; Argv;

    SDL_Init(SDL_INIT_EVERYTHING);
    // SDL_SetRelativeMouseMode(SDL_TRUE);
    // SDL_ShowCursor(SDL_ENABLE);

    i32 Width = 800;
    i32 Height = 600;
    window Window = CreateOpenGLWindow("Toasty!", Width, Height);

    clock MainClock = CreateClock();
    keyboard Keyboard = CreateKeyboard();
    f32 MouseSensitivity = 1.0f;
    mouse Mouse = CreateMouse(MouseSensitivity);

    b32 Running = 1;
    while(Running)
    {
        UpdateClock(&MainClock);
        ProcessEventQueue(&Running);
        UpdateKeyboard(&Keyboard);
        UpdateMouse(&Mouse);

        if(ButtonIsPressed(&Mouse, 1))
        {
            printf("Pressed!\n");
        }

        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // UpdateCamera(&MainCamera, &Keyboard, &Mouse, MainClock.DeltaTime);


        // { // Render frame
        //     UpdateCameraViewMatrix(Renderer.MatricesUBO, &MainCamera.View);

        //     ClearScreen(SmoothPurple);

        //     // DrawMesh(NewMesh);
        //     // DrawMesh(WallMesh);

        //     SwapGLBuffers(Window);
        // }

        printf("Seconds Elapsed: %2.2f\n", MainClock.SecondsElapsed);

        SDL_GL_SwapWindow(Window.Handle);
    }

    return 0;
}
