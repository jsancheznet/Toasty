#pragma once

#include "model.h"

void ProcessMesh(const aiScene *Scene, const model *Model, const aiMesh *Mesh, std::vector<mesh> &Meshes)
{
    mesh Result = {};
    Result.Name = Mesh->mName.C_Str();

    if(!Mesh->HasPositions())      { printf("Mesh %s: has no positions\n", Result.Name.c_str()); }
    if(!Mesh->HasNormals())        { printf("Mesh %s: has no normals\n", Result.Name.c_str()); }
    if(!Mesh->HasTextureCoords(0)) { printf("Mesh %s: has no texture coordinates\n", Result.Name.c_str()); }

    for(unsigned int j = 0; j < Mesh->mNumVertices; j++)
    {
        vertex Attributes;
        Attributes.Positions          = glm::vec3(Mesh->mVertices[j].x, Mesh->mVertices[j].y, Mesh->mVertices[j].z); // Vertices
        Attributes.Normals            = glm::vec3(Mesh->mNormals[j].x, Mesh->mNormals[j].y, Mesh->mNormals[j].z); // Normals
        Attributes.TextureCoordinates = glm::vec2(Mesh->mTextureCoords[0][j].x, Mesh->mTextureCoords[0][j].y); // TextureCoordinates
        Result.Attributes.push_back(Attributes);
    }

    // Extract Faces/Indices
    if(Mesh->HasFaces())
    {
        for(unsigned int j = 0; j < Mesh->mNumFaces; j++)
        {
            aiFace Face = Mesh->mFaces[j];
            for(unsigned int k = 0; k < Face.mNumIndices; k++)
            {
                Result.Indices.push_back(Face.mIndices[k]);
            }
        }
    }

    if(Scene->HasMaterials())
    {
        // Material documentation: http://assimp.sourceforge.net/lib_html/materials.html
        aiMaterial* Material = Scene->mMaterials[Mesh->mMaterialIndex];

        // Material Name
        aiString MaterialName;
        if(Material->Get(AI_MATKEY_NAME, MaterialName) == AI_SUCCESS) { Result.Material.Name = MaterialName.C_Str(); }

        // Get Materials Colors, Ambient, Diffuse, Specular
        aiColor3D Color;
        if(Material->Get(AI_MATKEY_COLOR_AMBIENT, Color) == AI_SUCCESS)  { Result.Material.AmbientColor = glm::vec3(Color.r, Color.g, Color.b); }
        if(Material->Get(AI_MATKEY_COLOR_DIFFUSE, Color) == AI_SUCCESS)  { Result.Material.DiffuseColor = glm::vec3(Color.r, Color.g, Color.b); }
        if(Material->Get(AI_MATKEY_COLOR_SPECULAR, Color) == AI_SUCCESS) { Result.Material.SpecularColor = glm::vec3(Color.r, Color.g, Color.b); }

        Material->Get(AI_MATKEY_SHININESS, Result.Material.Shininess); // Shininess
        Material->Get(AI_MATKEY_REFRACTI, Result.Material.RefractionIndex); // Index of refraction, optical density
        Material->Get(AI_MATKEY_SHADING_MODEL, Result.Material.IlluminationModel); //

        //
        // Load textures
        //
        // Assimp different texture types:
        // http://assimp.sourceforge.net/lib_html/material_8h.html#a7dd415ff703a2cc53d1c22ddbbd7dde0

        aiString TextureFilename;
        if(Material->GetTexture(aiTextureType_DIFFUSE, 0, &TextureFilename) == AI_SUCCESS)
        {
            std::string Filepath = Model->Path + '/' + TextureFilename.C_Str();
            texture Texture = CreateTexture(Filepath.c_str(), TextureType_Diffuse);
            Result.Textures.push_back(Texture);
        }
        else
        {
            printf("Material %s has no diffuse texture\n", TextureFilename.C_Str());
        }

        if(Material->GetTexture(aiTextureType_SPECULAR, 0, &TextureFilename) == AI_SUCCESS)
        {
            std::string Filepath = Model->Path + '/' + TextureFilename.C_Str();
            texture Texture = CreateTexture(Filepath.c_str(), TextureType_Specular);
            Result.Textures.push_back(Texture);
        }
        else
        {
            printf("Material %s has no specular texture\n", Result.Material.Name.c_str());
        }

        if(Material->GetTexture(aiTextureType_AMBIENT, 0, &TextureFilename) == AI_SUCCESS)
        {
            std::string Filepath = Model->Path + '/' + TextureFilename.C_Str();
            texture Texture = CreateTexture(Filepath.c_str(), TextureType_Ambient);
            Result.Textures.push_back(Texture);
        }
        else
        {
            printf("Material %s has no ambient texture\n", Result.Material.Name.c_str());
        }

    }
    else
    {
        printf("Scene %s does not have materials\n", Scene->mName.C_Str());
    }

    Meshes.push_back(Result);
}

void ProcessNode(const aiScene *Scene, model *Model, const aiNode *Node)
{
    // Set the name for the model or mesh
    if(Node->mParent == NULL)
    {
        Model->Name = Node->mName.C_Str();
    }
    else
    {
        aiString Name = Node->mName;
    }

    // Extract meshes
    int MeshCount = Node->mNumMeshes;
    for(int i = 0; i < MeshCount; i++)
    {
        // Node->mMeshes is an array that contains indices to the Scene->mMeshes array
        ProcessMesh(Scene, Model, Scene->mMeshes[Node->mMeshes[i]], Model->Meshes);
    }

    int ChildrenCount = Node->mNumChildren;
    for(int i = 0; i < ChildrenCount; i++)
    {
        ProcessNode(Scene, Model, Node->mChildren[i]);
    }
}

void UploadToGPU(model *Model)
{
    glGenVertexArrays(1, &Model->VAO);
    glBindVertexArray(Model->VAO);

    for(auto &Mesh : Model->Meshes)
    {
        glGenBuffers(1, &Mesh.VBO);
        glGenBuffers(1, &Mesh.EBO);

        glBindBuffer(GL_ARRAY_BUFFER, Mesh.VBO);
        glBufferData(GL_ARRAY_BUFFER, Mesh.Attributes.size() * sizeof(Mesh.Attributes[0]), &Mesh.Attributes[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Mesh.Indices.size() *  sizeof(Mesh.Indices[0]), &Mesh.Indices[0], GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)0);
        glEnableVertexAttribArray(0);
        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(3 * sizeof(f32)));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(6 * sizeof(f32)));
        glEnableVertexAttribArray(2);
    }

    glBindVertexArray(0);
}

model CreateModel(std::string Filename)
{
    model Result = {};

    // Stores the filepath on the model struct
    Result.Path = Filename.substr(0, Filename.find_last_of('/'));

    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(Filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals);
    if (Scene == nullptr)
    {
        printf("%s\n", Importer.GetErrorString());
        return Result;
    }

    // Let's process the scene, one node at a time
    ProcessNode(Scene, &Result, Scene->mRootNode);

    // Upload everything to GPU
    UploadToGPU(&Result);

    return Result;
}
