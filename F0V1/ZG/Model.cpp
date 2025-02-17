#include "pch.h"
#include "Model.h"

#include "Array.h"
#include "Value2D.h"
#include "Value3D.h"

using namespace ZG;

namespace
{
    struct Face
    {
        Array<int> vertexIndices;
        Array<int> uvIndices;
        Array<int> normalIndices;
    };

    struct ModelData
    {
        Array<Vec3> vertexes;
        Array<Vec2> texCoords;
        Array<Vec3> normals;
        Array<Face> faces;
    };

    ModelData loadObj(const std::string& filename)
    {
        tinyobj::attrib_t attrib{};
        Array<tinyobj::shape_t> shapes{};
        Array<tinyobj::material_t> materials{};

        if (not tinyobj::LoadObj(&attrib, &shapes, &materials, nullptr, nullptr, filename.c_str()))
        {
            // TODO: handle error
            throw std::runtime_error("failed to load obj file");
        }

        ModelData model;

        // 頂点座標の取得
        for (size_t i = 0; i < attrib.vertices.size(); i += 3)
        {
            Vec3 vertex = {attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]};
            model.vertexes.push_back(vertex);
        }

        // UV座標の取得
        for (size_t i = 0; i < attrib.texcoords.size(); i += 2)
        {
            Vec2 texCoord = {attrib.texcoords[i], attrib.texcoords[i + 1]};
            model.texCoords.push_back(texCoord);
        }

        // 法線の取得
        for (size_t i = 0; i < attrib.normals.size(); i += 3)
        {
            Vec3 normal = {attrib.normals[i], attrib.normals[i + 1], attrib.normals[i + 2]};
            model.normals.push_back(normal);
        }

        // 面データの取得
        for (const auto& shape : shapes)
        {
            size_t indexOffset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
            {
                const int fv = shape.mesh.num_face_vertices[f]; // 1つの面の頂点数（三角形なら3）
                Face face;
                for (int v = 0; v < fv; v++)
                {
                    tinyobj::index_t idx = shape.mesh.indices[indexOffset + v];
                    face.vertexIndices.push_back(idx.vertex_index);
                    if (idx.texcoord_index >= 0) face.uvIndices.push_back(idx.texcoord_index);
                    if (idx.normal_index >= 0) face.normalIndices.push_back(idx.normal_index);
                }
                model.faces.push_back(face);
                indexOffset += fv;
            }
        }

        return model;
    }
}

struct Model::Impl
{
    ModelData m_modelData{};

    Impl(const ModelParams& params) :
        m_modelData(loadObj(params.filename))
    {
    }
};

namespace ZG
{
    Model::Model(const ModelParams& params) :
        p_impl(std::make_shared<Impl>(params))
    {
    }
}
