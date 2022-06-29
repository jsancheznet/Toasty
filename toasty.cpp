/*
  Todo List

   - Infinite Grid: http://asliceofrendering.com/page2/
   - Arcball Camera: https://asliceofrendering.com/camera/2019/11/30/ArcballCamera/
 */

#include <stdio.h>

#include <SDL.h>

#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include <glad.c>

// #pragma warning(disable: 4100) // TODO(Jorge): What is this?, Disable it, and fix the code
// #pragma warning(disable:4127)  // GLM fails to compile if this warning is turned on!
#pragma warning(disable: 4201) // GLM warning
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/constants.hpp> // ???
#include <glm/gtx/rotate_vector.hpp> // glm::rotate vector

#include "imgui.cpp"
#include "imgui_tables.cpp"
#include "imgui_widgets.cpp"
#include "imgui_draw.cpp"
#include "imgui_demo.cpp" // You don't need this file, i use it as gui reference
#include "imgui_impl_opengl3.cpp"
#include "imgui_impl_sdl.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define OPENGL_DEBUG_MODE
#include "typedefs.h"
#include "helpers.cpp"
#include "clock.cpp"
#include "keyboard.cpp"
#include "mouse.cpp"
#include "renderer.cpp"
#include "camera.cpp"
#include "texture.cpp"
#include "model.cpp"

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

    i32 Flags; glGetIntegerv(GL_CONTEXT_FLAGS, &Flags);
    if (Flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        printf("OpenGL Debug Mode: Enabled\n");
    }

    SDL_GL_SetSwapInterval(1); // Enable vsync

    glViewport(0, 0, Width, Height);

    i32 MajorVersion, MinorVersion;
    glGetIntegerv(GL_MAJOR_VERSION, &MajorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &MinorVersion);
    printf("OpenGL Version %d.%d\n", MajorVersion, MinorVersion);

    return Result;
}

void SetupDearImGui(window *Window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(Window->Handle, Window->Context);
    ImGui_ImplOpenGL3_Init("#version 440");
}

void ProcessEventQueue(b32 *Running)
{
    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
        // ImGui_ImplSDL2_ProcessEvent(&Event);

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
                *Running = 0;
                break;
            }
            default:
            {
                break;
            }
        }
    }

}

u32 CreateShader(char *Filename)
{
    Assert(Filename);

    u32 Result;
    char *SourceFile = ReadTextFile(Filename);

    // Compile Vertex Shader
    u32 VertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    char *VertexSource[2] = {"#version 440 core\n#define VERTEX_SHADER\n", SourceFile};
    glShaderSource(VertexShaderObject, 2, VertexSource, NULL);
    glCompileShader(VertexShaderObject);
    i32 Compiled;
    glGetShaderiv(VertexShaderObject, GL_COMPILE_STATUS, &Compiled);
    if (Compiled != GL_TRUE)
    {
        i32 LogLength = 0;
        char ErrorMessage[1024];
        glGetShaderInfoLog(VertexShaderObject, 1024, &LogLength, ErrorMessage);
        fprintf(stderr, "%s-%s\n", Filename, ErrorMessage);
        VertexShaderObject = 0;
    }

    // Compile Fragment Shader
    u32 FragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    char *FragmentSource[2] = {"#version 440 core\n#define FRAGMENT_SHADER\n", SourceFile};
    glShaderSource(FragmentShaderObject, 2, FragmentSource, NULL);
    glCompileShader(FragmentShaderObject);
    // i32 Compiled;
    glGetShaderiv(FragmentShaderObject, GL_COMPILE_STATUS, &Compiled);
    if (Compiled != GL_TRUE)
    {
        i32 LogLength = 0;
        char ErrorMessage[1024];
        glGetShaderInfoLog(FragmentShaderObject, 1024, &LogLength, ErrorMessage);
        fprintf(stderr, "%s-%s\n", Filename, ErrorMessage);
        FragmentShaderObject = 0;
    }

    // Link program
    Result = glCreateProgram();
    glAttachShader(Result, VertexShaderObject);
    glAttachShader(Result, FragmentShaderObject);
    glLinkProgram(Result);
    i32 IsLinked = 0;
    glGetProgramiv(Result, GL_LINK_STATUS, (GLint *)&IsLinked);
    if (IsLinked == GL_FALSE)
    {
        i32 MaxLogLength = 1024;
        char InfoLog[1024] = {0};
        glGetProgramInfoLog(Result, MaxLogLength, &MaxLogLength, &InfoLog[0]);
        printf("%s: SHADER PROGRAM FAILED TO COMPILE/LINK\n", Filename);
        printf("%s\n", InfoLog);
        glDeleteProgram(Result);
        Result = 0;
    }

    glDeleteShader(VertexShaderObject);
    glDeleteShader(FragmentShaderObject);
    Free(SourceFile);

    return Result;
}

void ActivateShader(shader Shader)
{
    glUseProgram(Shader);
}

void BindTexture2D(u32 TextureUnit, u32 Handle)
{
    glActiveTexture(GL_TEXTURE0 + TextureUnit);
    glBindTexture(GL_TEXTURE_2D, Handle);
}

#if 0
void GUI_Model(model &Model)
{
    if (ImGui::CollapsingHeader(Model.Name.c_str()))
    {
        ImGui::Text("VAO: %d", Model.VAO);
        ImGui::Text("Working Directory: %s", Model.Path.c_str());
        ImGui::Text("Mesh Count: %d", Model.Meshes.size());

        for(mesh &Mesh : Model.Meshes)
        {
            if (ImGui::TreeNode(Mesh.Name.c_str()))
            {
                ImGui::Text("VBO: %d", Mesh.VBO);
                ImGui::Text("EBO: %d", Mesh.EBO);
                ImGui::Text("Attribute Count: %d", Mesh.Attributes.size());
                ImGui::Text("Index Count: %d", Mesh.Indices.size());
            }
        }
    }
}
#endif

void DrawGUI(renderer &Renderer, camera &MainCamera, model &Model)
{
    Model;

    ImGui::Begin("Toasty Settings");

    // GUI_Model(Model);

    if (ImGui::CollapsingHeader("Renderer settings"))
    {
        ImGui::ColorEdit4("Background Color", glm::value_ptr(Renderer.BackgroundColor));
    }

    if (ImGui::CollapsingHeader("Input")) {}

    if(ImGui::CollapsingHeader("Camera controls"))
    {
        ImGui::InputFloat3("Position", glm::value_ptr(MainCamera.Position));
        ImGui::InputFloat3("Front", glm::value_ptr(MainCamera.Front));
        ImGui::InputFloat3("Up", glm::value_ptr(MainCamera.Up));
        ImGui::Text("Yaw: %3.3f", MainCamera.Yaw);
    }
    // ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    // ImGui::Checkbox("Another Window", &show_another_window);
    // ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    // ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
    // if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    //     counter++;
    // ImGui::SameLine();
    // ImGui::Text("counter = %d", counter);
    // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();

    ImGui::ShowDemoWindow();
}

int main(int Argc, char ** Argv)
{
    Argc; Argv;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_ShowCursor(SDL_ENABLE);

    i32 Width = 1280;
    i32 Height = 720;
    window Window = CreateOpenGLWindow("Toasty!", Width, Height);
    renderer Renderer = CreateRenderer(Window);

    // SetupDearImGui(&Window);

    clock    MainClock = CreateClock();
    keyboard Keyboard = CreateKeyboard();
    f32      MouseSensitivity = 5.0f;
    mouse    Mouse = CreateMouse(MouseSensitivity);
    camera   MainCamera = CreateCamera(1280, 720, glm::vec3(0.0f, 0.0f, 5.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    shader MainShader = CreateShader("shaders/test.glsl");

    // model MyModel = CreateModel("models/cube/cube.obj");
    model MyModel = CreateModel("models/teapot/teapot.obj");
    // model MyModel = CreateModel("models/sphere.obj");
    // model MyModel = CreateModel("models/holodeck/holodeck.obj");
    // model MyModel = CreateModel("models/mori_knob/testObj.obj");

    b32 Running = 1;
    while(Running)
    {
        UpdateClock(&MainClock);
        ProcessEventQueue(&Running);
        UpdateKeyboard(&Keyboard);
        UpdateMouse(&Mouse);

        if(KeyIsNotPressed(&Keyboard, SDL_SCANCODE_LSHIFT))
        {
            SDL_SetRelativeMouseMode(SDL_TRUE);
            SDL_ShowCursor(SDL_DISABLE);
            UpdateCamera(&MainCamera, &Keyboard, &Mouse, MainClock.DeltaTime);
        }
        else
        {
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_ShowCursor(SDL_ENABLE);
        }

        { // Render things

            // TODO(Jorge): Render using glBindVertexBuffer, glVertexAttribBinding, glVertexAttribFormat

            RenderNewFrame(&Renderer, &MainCamera);

            glm::mat4 Model = glm::mat4(1.0f);
            Model = glm::scale(Model, glm::vec3(0.05f, 0.05f, 0.05f));
            SetUniform(MainShader, "Model", Model);
            ActivateShader(MainShader);

            for(auto &Mesh : MyModel.Meshes)
            {
                glBindVertexArray(Mesh.VAO);
                glBindBuffer(GL_ARRAY_BUFFER, Mesh.VBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.EBO);

                for(auto &Texture : Mesh.Textures)
                {
                    if(Texture.Type == TextureType_Diffuse)
                    {
                        BindTexture2D(0, Texture.Handle);
                    }
                    else
                    {
                        switch(Texture.Type)
                        {
                            case TextureType_Null:
                            {
                                printf("Texture Not Binded: %s, Type: TextureType_Null\n", Texture.Filename.c_str());
                            } break;
                            case TextureType_Specular:
                            {
                                printf("Texture Not Binded: %s, Type: TextureType_Specular\n", Texture.Filename.c_str());
                            } break;
                            case TextureType_Ambient:
                            {
                                printf("Texture Not Binded: %s, Type: TextureType_Ambient\n", Texture.Filename.c_str());
                            } break;
                            case TextureType_Emissive:
                            {
                                printf("Texture Not Binded: %s, Type: TextureType_Emissive\n", Texture.Filename.c_str());
                            } break;
                            default:
                            {
                                printf("Texture Not Binded: %s, Type: NO_TYPE_EXISTS \n", Texture.Filename.c_str());
                            } break;
                        }
                    }
                }

                glDrawElements(GL_TRIANGLES, (i32)Mesh.Indices.size(), GL_UNSIGNED_INT, 0);
            }

            // DrawGUI(Renderer, MainCamera, MyModel);

            RenderEndFrame(&Renderer);
        }
    }

    // Cleanup
    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplSDL2_Shutdown();
    // ImGui::DestroyContext();

    SDL_GL_DeleteContext(Window.Context);
    SDL_DestroyWindow(Window.Handle);
    SDL_Quit();

    return 0;
}
