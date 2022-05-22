#pragma once

#include "typedefs.h"
#include "camera.h"

camera
CreateCamera(i32 Width, i32 Height, glm::vec3 Position, glm::vec3 Front, glm::vec3 Up)
{
    camera Result = {}; // Zeroes out everything, including Acceleration and Velocity

    Result.Position = Position;
    Result.Front = Front;
    Result.Up = Up;

    f32 CameraSpeed = 5.0f;
    f32 Fov = 90.0f;
    f32 Near = 0.1f;
    f32 Far = 1000.0f;
    Result.Speed = CameraSpeed;
    Result.Fov = Fov;
    Result.Near = Near;
    Result.Far = Far;

    Width;Height;
    Result.View = glm::lookAt(Position, Front, Up);
    Result.Perspective = glm::perspective(glm::radians(Fov), (f32)Width / (f32)Height, Near, Far);
    Result.Orthographic = glm::ortho(-Width/2.0f, Width/2.0f, -Height/2.0f, Height/2.0f, Near, Far);

    // This was previously on the game loop, it needs to set this values to avoid a camera jump in the first frame
    // Implementation Reference: https://learnopengl.com/Getting-started/Camera
    Result.Yaw = -90.0f; // Set the Yaw to -90 so the mouse faces to 0, 0, 0 in the first frame X
    Result.Pitch = 0.0f;

    return Result;
}

void UpdateCamera(camera *Camera, keyboard *Keyboard, mouse *Mouse, f64 DeltaTime)
{
    // TODO: Use Newton movement equations for camera movement
    Camera->Yaw += Mouse->RelX * Mouse->Sensitivity * (f32)DeltaTime;
    Camera->Pitch += -Mouse->RelY * Mouse->Sensitivity * (f32)DeltaTime; // reversed since y-coordinates range from bottom to top

    if (Camera->Pitch > 89.0f)
    {
        Camera->Pitch = 89.0f;
    }
    else if (Camera->Pitch < -89.0f)
    {
        Camera->Pitch = -89.0f;
    }

    glm::vec3 Front;
    Front.x = glm::cos(glm::radians(Camera->Yaw)) * glm::cos(glm::radians(Camera->Pitch));
    Front.y = glm::sin(glm::radians(Camera->Pitch));
    Front.z = glm::sin(glm::radians(Camera->Yaw)) * glm::cos(glm::radians(Camera->Pitch));
    Camera->Front = glm::normalize(Front);

    // Original movement code
    // FrameSpeed exists so Camera->Speed * DeltaTime is only computed once
    f32 FrameSpeed = Camera->Speed * (f32)DeltaTime;
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_W))
    {
        // Camera->Position = Vec3_Add(Camera->Position, Vec3_Scale(Camera->Front, FrameSpeed));
        Camera->Position = Camera->Position + (Camera->Front * FrameSpeed);
    }
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_S))
    {
        // Camera->Position = Vec3_Subtract(Camera->Position, Vec3_Scale(Camera->Front, FrameSpeed));
        Camera->Position = Camera->Position - (Camera->Front * FrameSpeed);
    }
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_A))
    {
        // Camera->Position = Vec3_Subtract(Camera->Position, Vec3_Normalize(Vec3_Scale(Vec3_Cross(Camera->Front, Camera->Up), FrameSpeed)));
        Camera->Position = (Camera->Position - (glm::normalize(glm::cross(Camera->Front, Camera->Up) * FrameSpeed)));
    }
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_D))
    {
        // Camera->Position = Vec3_Add(Camera->Position, Vec3_Normalize(Vec3_Scale(Vec3_Cross(Camera->Front, Camera->Up), FrameSpeed)));
        Camera->Position = (Camera->Position + (glm::normalize(glm::cross(Camera->Front, Camera->Up) * FrameSpeed)));
    }
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_SPACE))
    {
        Camera->Position.y += FrameSpeed;
    }
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_LCTRL))
    {
        Camera->Position.y -= FrameSpeed;
    }

    // Update Camera's view matrix
    // Camera->View = Mat4_LookAt(Camera->Position, Vec3_Add(Camera->Position, Camera->Front), Camera->Up);
    Camera->View = glm::lookAt(Camera->Position, (Camera->Position + Camera->Front), Camera->Up);
}
