#pragma once

// #include <iostream> // including iostream breaks the build somehow, what is going on?
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "typedefs.h"
#include "texture.h"

struct vertex
{
    glm::vec3 Positions;
    glm::vec3 Normals;
    glm::vec2 TextureCoordinates;
};

struct material
{
    std::string Name;        // Material Name
    glm::vec3 AmbientColor;  // Ka
    glm::vec3 DiffuseColor;  // Kd
    glm::vec3 SpecularColor; // Ks
    f32 Shininess;           // Ns
    f32 RefractionIndex;     // Ni, also called optical density
    i32 IlluminationModel;   // illum
};

struct mesh
{
    u32 VBO;
    u32 EBO;

    std::string Name;
    std::vector<vertex> Attributes;
    std::vector<u32> Indices;
    material Material;
    std::vector<texture> Textures;
};

struct model
{
    u32 VAO;
    std::string Name;
    std::vector<mesh> Meshes;
    std::string Path;
};
