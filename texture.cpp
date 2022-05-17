#pragma once

#include <string>

#include "texture.h"

texture CreateTexture(std::string Filename, texture_type TextureType = TextureType_Null)
{
    texture Result = {};

    Result.Type = TextureType;

    i32 RequestedChannelCount = 0;
    i32 FlipVertically = 1;
    stbi_set_flip_vertically_on_load(FlipVertically);
    u8 *Data = stbi_load(Filename.c_str(), &Result.Width, &Result.Height, &Result.ChannelCount, RequestedChannelCount);
    if(Data)
    {
        glGenTextures(1, &Result.Handle);
        glBindTexture(GL_TEXTURE_2D, Result.Handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if(Result.ChannelCount == 3)
        {
            Result.Format = GL_RGB;
            Result.InternalFormat = GL_SRGB;
        }
        else if(Result.ChannelCount == 4)
        {
            Result.Format = GL_RGBA;
            // NOTE: If anything looks weird when drawing textures,
            // maybe toggle between GL_SRGB and GL_SRGB_ALPHA. I'm too
            // lazy to check opengl docs right now.
            // Result->InternalFormat = GL_SRGB;
            Result.InternalFormat = GL_SRGB_ALPHA;
        }
        else
        {
            fprintf(stderr, "%s Texture format is not GL_RGB or GL_RGBA\n", Filename.c_str());
            stbi_image_free(Data);
            Result.Type = TextureType_Null;
        }

        i32 MipMapDetailLevel = 0;
        // REMINDER: Textures to be used for data should not be uploaded as GL_SRGB!
        // NOTE: InternalFormat is the format we want to store the data, Format is the input format
        glTexImage2D(GL_TEXTURE_2D, MipMapDetailLevel, Result.InternalFormat, Result.Width, Result.Height, 0, Result.Format, GL_UNSIGNED_BYTE, Data);
        // NOTE(Jorge): Set custom MipMaps filtering values here!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(Data);
    }
    else
    {
        printf("Could not load image file: %s\n", Filename.c_str());
        Result.Type = TextureType_Null;
    }

    return Result;
}
