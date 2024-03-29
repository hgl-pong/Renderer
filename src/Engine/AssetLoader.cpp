#include "Common/pch.h"
#include <Engine/AssetLoader.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <SOIL2/SOIL2.h>
void ModelLoader::ReadFile(const std::string &filename, Model &model)
{
    auto it = m_models.find(filename);
    if (it != m_models.end())
    {
        model = it->second;
        return;
    }

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    if (!scene)
    {
        HLOG_ERROR("Loading model %s failed\n", filename.c_str());
        return;
    }
    HLOG_INFO("Loading model %s\n", filename.c_str());
    model.m_Meshes.resize(scene->mNumMeshes);
    for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[i];
        auto &newMesh = model.m_Meshes[i];
        HLOG_INFO("Loading mesh %s\n", mesh->mName.C_Str());
        HLOG_INFO("Number of vertices: %d\n", mesh->mNumVertices);
        HLOG_INFO("Number of faces: %d\n", mesh->mNumFaces);
        HLOG_INFO("Has normals: %d\n", mesh->HasNormals());
        HLOG_INFO("Has tangents and bitangents: %d\n", mesh->HasTangentsAndBitangents());
        HLOG_INFO("Number of texture coordinates: %d\n", mesh->GetNumUVChannels());
        newMesh.mVertices.resize(mesh->mNumVertices);
        newMesh.mIndices.resize(mesh->mNumFaces * 3);
        for (uint32_t j = 0; j < mesh->mNumVertices; ++j)
        {
            aiVector3D vertex = mesh->mVertices[j];
            newMesh.mVertices.push_back(Vector3f(vertex.x, vertex.y, vertex.z));
        }
        if (mesh->HasNormals())
        {
            newMesh.mNormals.resize(mesh->mNumVertices);
            for (uint32_t j = 0; j < mesh->mNumVertices; ++j)
            {
                aiVector3D normal = mesh->mNormals[j];
                newMesh.mNormals.push_back(Vector3f(normal.x, normal.y, normal.z));
            }
        }
        if (mesh->HasTangentsAndBitangents())
        {
            newMesh.mTangents.resize(mesh->mNumVertices);
            for (uint32_t j = 0; j < mesh->mNumVertices; ++j)
            {
                aiVector3D tangent = mesh->mTangents[j];
                newMesh.mTangents.push_back(Vector4f(tangent.x, tangent.y, tangent.z, 0.0f));
            }
        }
        for (uint32_t j = 0; j < mesh->GetNumUVChannels(); ++j)
        {
            if (mesh->HasTextureCoords(j))
            {
                newMesh.mTexCoordsArray.push_back(std::vector<Vector2f>(mesh->mNumVertices));
                for (uint32_t k = 0; k < mesh->mNumVertices; ++k)
                {
                    aiVector3D texCoord = mesh->mTextureCoords[j][k];
                    newMesh.mTexCoordsArray[j].push_back(Vector2f(texCoord.x, texCoord.y));
                }
            }
            else
                break;
        }
        for (uint32_t j = 0; j < mesh->mNumFaces; ++j)
        {
            aiFace face = mesh->mFaces[j];
            for (uint32_t k = 0; k < face.mNumIndices; ++k)
            {
                newMesh.mIndices.push_back(face.mIndices[k]);
            }
        }
        newMesh.mColor = RandomColor();
    }
    m_models.emplace(std::make_pair(filename, model));
}

void ImageLoader::ReadFile(const std::string &filename, Image &image)
{
    auto it = m_images.find(filename);
    if (it != m_images.end())
    {
        image = it->second;
        return;
    }

    int width, height, channels;
    unsigned char *pixels = SOIL_load_image(filename.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
    if (!pixels)
    {
        HLOG_ERROR("Loading image %s failed\n", filename.c_str());
        return;
    }
    HLOG_INFO("Loading image %s\n", filename.c_str());
    HLOG_INFO("width: %d, height: %d, channels: %d\n", width, height, channels);
    image.mWidth = width;
    image.mHeight = height;
    image.mChannels = channels;
    image.mPixels.reserve(width * height * channels);
    for (int i = 0; i < width * height * channels; ++i)
    {
        image.mPixels.push_back(pixels[i] / 255.0f);
    }
    SOIL_free_image_data(pixels);
    m_images.emplace(std::make_pair(filename, image));
}

void MaterialLoader::ReadFile(const std::string& filename, IMaterial& asset)
{}