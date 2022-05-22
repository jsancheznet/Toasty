#pragma once

#include "window.h"
#include "renderer.h"
#include "camera.h"


void UploadCameraMatrices(u32 MatricesUBO, glm::mat4 *Perspective, glm::mat4 *Orthographic, glm::mat4 *View)
{
    Assert(Perspective);
    Assert(Orthographic);
    Assert(View);

    glBindBuffer(GL_UNIFORM_BUFFER, MatricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), Perspective);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), Orthographic);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*2, sizeof(glm::mat4), View);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UpdateCameraViewMatrix(u32 MatricesUBO, glm::mat4 *View)
{
    Assert(View);
    Assert(MatricesUBO != 0);

    // NOTE: MatricesUBO consist of 3 Matrices, Projection,
    // Orthographic and View. View is the third on the struct, that's
    // why the offset is sizeof(mat4) * 2
    u32 Offset = sizeof(glm::mat4) * 2;

    glBindBuffer(GL_UNIFORM_BUFFER, MatricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, Offset, sizeof(glm::mat4), View);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

renderer
CreateRenderer(window Window)
{
    renderer Result = {};

    Result.Window = Window;

    { // Create Matrices Uniform buffer object, made to hold the camera matrices
        glGenBuffers(1, &Result.MatricesUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, Result.MatricesUBO);
        i32 UniformMatrixCount = 3;
        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * UniformMatrixCount, NULL, GL_STATIC_DRAW);
        // glBindBufferBase links MatricesUBO to binding point 0 in the active shader, do i call this for each shader? only once? on each frame?
        // TODO: Test BindBufferBase to be called only once
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, Result.MatricesUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    return Result;
}

void RendererDearImGui_BeginFrame()
{
    // Start the Dear ImGui frame
    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplSDL2_NewFrame();
    // ImGui::NewFrame();
}

void RendererDearImGui_EndFrame()
{
    // ImGui::Render();
    // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void RenderNewFrame(renderer *Renderer, camera *Camera)
{
    Renderer; // NOTE: Delete me once we use the Renderer variable for anything

    // CornflowerBlue 100, 149, 237
    glClearColor(Renderer->BackgroundColor.r, Renderer->BackgroundColor.g, Renderer->BackgroundColor.b, Renderer->BackgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
    RendererDearImGui_BeginFrame();

    UploadCameraMatrices(Renderer->MatricesUBO, &Camera->Perspective, &Camera->Orthographic, &Camera->View);
}

void RenderEndFrame(renderer *Renderer)
{
    RendererDearImGui_EndFrame();
    SDL_GL_SwapWindow(Renderer->Window.Handle);
}

void EnableWireframeMode()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void DisableWireframeMode()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void SetUniform(shader Shader, const char *VariableName, glm::mat4 Value)
{
    i32 Location = glGetUniformLocation(Shader, VariableName);

    if(Location == GL_INVALID_VALUE)
    {
        printf("SetUniform Error, Location is not a valid value\n");
    }
    else if(Location == GL_INVALID_OPERATION)
    {
        printf("SetUniform Error, Invalid Operation\n");
    }

    glUniformMatrix4fv(Location, 1, GL_FALSE, glm::value_ptr(Value));
}
