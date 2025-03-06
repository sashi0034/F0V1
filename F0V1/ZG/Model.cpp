#include "pch.h"
#include "Model.h"

#include "Array.h"
#include "AssertObject.h"
#include "Graphics3D.h"
#include "IndexBuffer.h"
#include "Mat4x4.h"
#include "Utils.h"
#include "Value2D.h"
#include "Value3D.h"
#include "VertexBuffer.h"
#include "detail/EngineCore.h"
#include "detail/EngineStackState.h"
#include "detail/PipelineState.h"

using namespace ZG;

namespace
{
    struct ModelVertex
    {
        Float3 position;
        Float3 normal;
        Float2 texCoord;
    };

    struct IndexKey
    {
        uint16_t v;
        uint16_t t;
        uint16_t n;
        uint16_t padding;

        bool operator==(const IndexKey& other) const
        {
            return (v == other.v) && (t == other.t) && (n == other.n);
        }
    };

    struct IndexKeyHasher
    {
        size_t operator()(const IndexKey& key) const
        {
            static_assert(sizeof(size_t) == sizeof(IndexKey));
            return *reinterpret_cast<const size_t*>(&key);
        }
    };

    struct ModelMaterial
    {
        alignas(16) Float3 ambient; // アンビエント色
        alignas(16) Float3 diffuse; // 拡散反射色
        alignas(16) Float3 specular; // 鏡面反射色
        alignas(16) float shininess; // 光沢
    };

    struct ShapeData
    {
        Array<ModelVertex> vertexBuffer{};
        Array<uint16_t> indexBuffer{};
        uint16_t materialIndex{};
    };

    struct ModelData
    {
        Array<ShapeData> shapes{};
        Array<std::string> materialNames;
        Array<ModelMaterial> materials{};
    };

    ModelData loadObj(const std::string& filename)
    {
        tinyobj::attrib_t attrib{};
        Array<tinyobj::shape_t> shapes{};
        Array<tinyobj::material_t> materials{};

        const auto baseDir = std::filesystem::path(filename).parent_path().string();
        if (not tinyobj::LoadObj(&attrib, &shapes, &materials, nullptr, nullptr, filename.c_str(), baseDir.c_str()))
        {
            // TODO: handle error
            throw std::runtime_error("failed to load obj file");
        }

        // 頂点座標の取得
        Array<Float3> positions{};
        for (size_t i = 0; i < attrib.vertices.size(); i += 3)
        {
            positions.emplace_back(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]);
        }

        // UV座標の取得
        Array<Float2> texCoords{};
        for (size_t i = 0; i < attrib.texcoords.size(); i += 2)
        {
            texCoords.emplace_back(attrib.texcoords[i], attrib.texcoords[i + 1]);
        }

        // 法線の取得
        Array<Float3> normals{};
        for (size_t i = 0; i < attrib.normals.size(); i += 3)
        {
            normals.emplace_back(attrib.normals[i], attrib.normals[i + 1], attrib.normals[i + 2]);
        }

        // 面データの取得
        ModelData modelData{};
        for (const auto& shape : shapes)
        {
            ShapeData shapeData{};
            shapeData.materialIndex = shape.mesh.material_ids.empty() ? 0 : shape.mesh.material_ids[0];

            std::unordered_map<IndexKey, unsigned int, IndexKeyHasher> indexMap{};

            size_t indexOffset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
            {
                const int fv = shape.mesh.num_face_vertices[f]; // 1つの面の頂点数（三角形なら3）

                for (int v = 0; v < fv; ++v)
                {
                    const auto& index = shape.mesh.indices[indexOffset + v];
                    IndexKey key{};
                    key.v = index.vertex_index; // TODO: uint16 オーバーフロー対策?
                    key.t = index.texcoord_index;
                    key.n = index.normal_index;

                    const auto iter = indexMap.find(key);
                    if (iter == indexMap.end())
                    {
                        ModelVertex vertex{};
                        vertex.position = positions[index.vertex_index];
                        if (index.texcoord_index >= 0)
                        {
                            vertex.texCoord = texCoords[index.texcoord_index];
                        }
                        if (index.normal_index >= 0)
                        {
                            vertex.normal = normals[index.normal_index];
                        }

                        const auto newIndex = shapeData.vertexBuffer.size();
                        shapeData.vertexBuffer.push_back(vertex);
                        indexMap[key] = newIndex;
                        shapeData.indexBuffer.push_back(newIndex);
                    }
                    else
                    {
                        shapeData.indexBuffer.push_back(iter->second);
                    }
                }

                indexOffset += fv;
            }

            modelData.shapes.push_back(shapeData);
        }

        // Material 情報の変換
        for (const auto& m : materials)
        {
            modelData.materialNames.push_back(m.name);

            ModelMaterial modelMat;
            modelMat.ambient = Float3(m.ambient[0], m.ambient[1], m.ambient[2]);
            modelMat.diffuse = Float3(m.diffuse[0], m.diffuse[1], m.diffuse[2]);
            modelMat.specular = Float3(m.specular[0], m.specular[1], m.specular[2]);
            modelMat.shininess = m.shininess;
            modelData.materials.push_back(modelMat);
        }

        return modelData;
    }

    using namespace ZG;
    using namespace ZG::detail;

    PipelineState makePipelineState(const ModelParams& params)
    {
        // TODO: キャッシュする?
        return PipelineState{
            PipelineStateParams{
                .pixelShader = params.pixelShader,
                .vertexShader = params.vertexShader,
                .vertexInput = {
                    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT},
                    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT},
                    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT}
                },
                .hasDepth = true,
                .srvCount = 1,
                .cbvCount = 1,
                .uavCount = 1 // FIXME: 0 にしたい?
            }
        };
    }

    struct SceneState_b0
    {
        Mat4x4 worldMat;
        Mat4x4 viewMat;
        Mat4x4 projectionMat;
    };

    struct ShapeBuffer
    {
        VertexBuffer<ModelVertex> vertexBuffer;
        IndexBuffer indexBuffer;
    };
}

struct Model::Impl
{
    ModelData m_modelData{};
    Array<Array<ShapeBuffer>> m_shapes{};
    PipelineState m_pipelineState;

    ComPtr<ID3D12DescriptorHeap> m_descriptorHeap{};
    ComPtr<ID3D12Resource> m_cb0{};
    SceneState_b0* m_mappedCB0{};

    Array<ComPtr<ID3D12Resource>> m_cb1List{};

    Impl(const ModelParams& params) :
        m_modelData(loadObj(params.filename)),
        m_pipelineState(makePipelineState(params))
    {
        m_shapes.resize(m_modelData.materials.size());
        for (auto& shape : m_modelData.shapes)
        {
            ShapeBuffer shapeBuffer{};
            shapeBuffer.vertexBuffer = VertexBuffer(shape.vertexBuffer);
            shapeBuffer.indexBuffer = IndexBuffer{shape.indexBuffer};
            m_shapes[shape.materialIndex].push_back(shapeBuffer);
        }

        createDescriptorHeap();
    }

    void createDescriptorHeap()
    {
        // 定数バッファ作成
        using namespace DirectX;

        // CB0
        const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const auto resourceDesc0 = CD3DX12_RESOURCE_DESC::Buffer(AlignedSize(sizeof(SceneState_b0), 256));
        AssertWin32{"failed to create commited resource"sv}
            | EngineCore.GetDevice()->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc0,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&m_cb0));

        AssertWin32{"failed to map constant buffer"sv}
            | m_cb0->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedCB0));
        // TODO: Unmap?

        // ディスクリプタヒープの作成
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
        descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        descriptorHeapDesc.NodeMask = 0;
        descriptorHeapDesc.NumDescriptors = 1 + m_modelData.materials.size();
        descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        AssertWin32{"failed to create descriptor heap"sv}
            | EngineCore.GetDevice()->CreateDescriptorHeap(
                &descriptorHeapDesc, IID_PPV_ARGS(&m_descriptorHeap));

        // CBV 作成
        auto heapHandle = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
        const auto incrementSize =
            EngineCore.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        // CB0 View
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = m_cb0->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = static_cast<UINT>(m_cb0->GetDesc().Width);
        EngineCore.GetDevice()->CreateConstantBufferView(&cbvDesc, heapHandle);

        // -----------------------------------------------

        // CB1 View
        m_cb1List.resize(m_modelData.materials.size());
        for (int i = 0; i < m_modelData.materials.size(); i++)
        {
            heapHandle.ptr += incrementSize;

            const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            const auto resourceDesc1 = CD3DX12_RESOURCE_DESC::Buffer(AlignedSize(sizeof(ModelMaterial), 256));
            AssertWin32{"failed to create commited resource"sv}
                | EngineCore.GetDevice()->CreateCommittedResource(
                    &heapProperties,
                    D3D12_HEAP_FLAG_NONE,
                    &resourceDesc1,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&m_cb1List[i]));

            ModelMaterial* mappedCB1;
            AssertWin32{"failed to map constant buffer"sv}
                | m_cb1List[i]->Map(0, nullptr, reinterpret_cast<void**>(&mappedCB1));

            // マテリアル情報を転送
            *mappedCB1 = m_modelData.materials[i];

            m_cb1List[i]->Unmap(0, nullptr);

            // CBV 作成
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
            cbvDesc.BufferLocation = m_cb1List[i]->GetGPUVirtualAddress();
            cbvDesc.SizeInBytes = static_cast<UINT>(m_cb1List[i]->GetDesc().Width);
            EngineCore.GetDevice()->CreateConstantBufferView(&cbvDesc, heapHandle);
        }
    }

    void Draw() const
    {
        SceneState_b0 sceneState{};
        sceneState.worldMat = EngineStackState.GetWorldMatrix().mat;
        sceneState.viewMat = EngineStackState.GetViewMatrix().mat;
        sceneState.projectionMat = EngineStackState.GetProjectionMatrix().mat;
        *m_mappedCB0 = sceneState;

        m_pipelineState.CommandSet();

        const auto commandList = EngineCore.GetCommandList();

        // カメラ行列設定
        commandList->SetDescriptorHeaps(
            1, m_descriptorHeap.GetAddressOf());
        commandList->SetGraphicsRootDescriptorTable(
            0, m_descriptorHeap->GetGPUDescriptorHandleForHeapStart());

        // マテリアル設定
        auto materialHandle = m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();
        const auto materialIncrementSize =
            EngineCore.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        for (size_t materialId = 0; materialId < m_shapes.size(); ++materialId)
        {
            materialHandle.ptr += materialIncrementSize;

            commandList->SetGraphicsRootDescriptorTable(1, materialHandle);

            for (auto& shape : m_shapes[materialId])
            {
                Graphics3D::DrawTriangles(shape.vertexBuffer, shape.indexBuffer);
            }
        }
    }
};

namespace ZG
{
    Model::Model(const ModelParams& params) :
        p_impl(std::make_shared<Impl>(params))
    {
    }

    void Model::draw() const
    {
        p_impl->Draw();
    }
}
