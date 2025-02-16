#include "pch.h"
#include "Texture.h"

#include <d3d12.h>

#include "AssertObject.h"
#include "Buffer3D.h"
#include "System.h"
#include "Utils.h"
#include "detail/EngineCore.h"
#include "detail/EngineStackState.h"
#include "detail/PipelineState.h"

using namespace ZG;
using namespace ZG::detail;

namespace
{
    Buffer3D makeVertexesAndIndicis()
    {
        return Buffer3D{
            Buffer3DParams{
                .vertexes = {
                    {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, //左下
                    {{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, //左上
                    {{1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, //右下
                    {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, //右上
                },
                .indices = {0, 1, 2, 2, 1, 3}
            }
        };
    }

    PipelineState makePipelineState(const TextureParams& options)
    {
        // TODO: キャッシュする?
        return PipelineState{
            PipelineStateParams{
                .pixelShader = options.pixelShader,
                .vertexShader = options.vertexShader,
                .vertexInput = {
                    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT},
                    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT}
                },
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
}

struct TextureBlob::Impl
{
    ComPtr<ID3D12Resource> m_textureBuffer{};
    ComPtr<ID3D12DescriptorHeap> m_descriptorHeap{};
    DXGI_FORMAT m_format{};

    Impl(const std::wstring& filename)
    {
        DirectX::TexMetadata metadata{};
        DirectX::ScratchImage scratchImage{};
        const auto loadResult =
            LoadFromWICFile(filename.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, scratchImage);

        if (FAILED(loadResult))
        {
            System::ModalError(L"failed to load texture");
            throw std::runtime_error("failed to load texture"); // FIXME
        }

        // 生データを取得
        const auto rawImage = scratchImage.GetImage(0, 0, 0);

        // -----------------------------------------------
        // アップロード用中間バッファの設定
        D3D12_HEAP_PROPERTIES uploadBufferDesc{};
        uploadBufferDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
        uploadBufferDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        uploadBufferDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        uploadBufferDesc.CreationNodeMask = 0;
        uploadBufferDesc.VisibleNodeMask = 0;

        D3D12_RESOURCE_DESC resourceDesc{};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment = 0;
        resourceDesc.Width = AlignedSize(rawImage->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * rawImage->height;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        // アップロード用中間バッファの作成
        ID3D12Resource* uploadBuffer{};
        AssertWin32{"failed to create commited resource"sv}
            | EngineCore.GetDevice()->CreateCommittedResource(
                &uploadBufferDesc,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ, // CPUからの書き込みが可能な状態にする
                nullptr,
                IID_PPV_ARGS(&uploadBuffer));

        // -----------------------------------------------
        // テクスチャバッファ設定
        D3D12_HEAP_PROPERTIES textureHeapProperties{};
        textureHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
        textureHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        textureHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        textureHeapProperties.CreationNodeMask = 0;
        textureHeapProperties.VisibleNodeMask = 0;

        resourceDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
        resourceDesc.Width = metadata.width;
        resourceDesc.Height = metadata.height;
        resourceDesc.DepthOrArraySize = metadata.arraySize;
        resourceDesc.MipLevels = metadata.mipLevels;
        resourceDesc.Format = metadata.format;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

        // テクスチャバッファ作成
        AssertWin32{""sv}
            | EngineCore.GetDevice()->CreateCommittedResource(
                &textureHeapProperties,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_COPY_DEST, // コピー先として使用する
                nullptr,
                IID_PPV_ARGS(&m_textureBuffer)
            );

        // -----------------------------------------------
        // アップロード用中間バッファに生データをコピー
        uint8_t* imageMap{};
        AssertWin32{"failed to map upload buffer"sv}
            | uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&imageMap));

        uint8_t* srcAddress = rawImage->pixels;
        const auto srcPitch = AlignedSize(rawImage->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
        for (size_t y = 0; y < rawImage->height; ++y)
        {
            std::memcpy(imageMap, srcAddress, rawImage->rowPitch);
            srcAddress += rawImage->rowPitch;
            imageMap += srcPitch;
        }

        uploadBuffer->Unmap(0, nullptr);

        // -----------------------------------------------
        // アップロード用中間バッファからテクスチャバッファにコピー
        // アップロード用中間バッファにはフットプリントを指定し、テクスチャバッファにはインデックスを指定する
        D3D12_TEXTURE_COPY_LOCATION dstCopyLocation{};
        dstCopyLocation.pResource = m_textureBuffer.Get();
        dstCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dstCopyLocation.SubresourceIndex = 0;

        D3D12_TEXTURE_COPY_LOCATION srcCopyLocation{};
        srcCopyLocation.pResource = uploadBuffer;
        srcCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT; // Footprint: メモリ占有領域に関する情報

        // FIXME: GetCopyableFootprints を使うほうがいい?
        srcCopyLocation.PlacedFootprint.Offset = 0;
        srcCopyLocation.PlacedFootprint.Footprint.Width = static_cast<UINT>(metadata.width);
        srcCopyLocation.PlacedFootprint.Footprint.Height = static_cast<UINT>(metadata.height);
        srcCopyLocation.PlacedFootprint.Footprint.Depth = static_cast<UINT>(metadata.depth);
        srcCopyLocation.PlacedFootprint.Footprint.RowPitch =
            static_cast<UINT>(AlignedSize(rawImage->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));
        srcCopyLocation.PlacedFootprint.Footprint.Format = rawImage->format;

        const auto commandList = EngineCore.GetCommandList();
        commandList->CopyTextureRegion(&dstCopyLocation, 0, 0, 0, &srcCopyLocation, nullptr);

        // TODO: 描画ルーチンの中で実行された場合に対応
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = m_textureBuffer.Get();
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

        commandList->ResourceBarrier(1, &barrier);
        commandList->Close();

        EngineCore.FlushCommandList();

        m_format = metadata.format;
    }

    Impl(const Image& image)
    {
        D3D12_HEAP_PROPERTIES heapProperties{};
        heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
        heapProperties.CreationNodeMask = 0;
        heapProperties.VisibleNodeMask = 0;

        D3D12_RESOURCE_DESC resourceDesc{};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceDesc.Alignment = 0;
        resourceDesc.Width = image.size().x;
        resourceDesc.Height = image.size().y;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        AssertWin32{"failed to create commited resource"sv}
            | EngineCore.GetDevice()->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_COPY_DEST,
                nullptr,
                IID_PPV_ARGS(&m_textureBuffer));

        AssertWin32{"failed to write to subresource"sv}
            | m_textureBuffer->WriteToSubresource(
                0,
                nullptr, // リソース全体領域をコピー
                image.data(),
                image.size().x * sizeof(ColorU8),
                image.size_in_bytes());

        m_format = resourceDesc.Format;
    }
};

struct Texture::Impl
{
    TextureBlob m_blob;

    PipelineState m_pipelineState;

    Buffer3D m_buffer3D = makeVertexesAndIndicis();

    ComPtr<ID3D12Resource> m_constantBuffer{};

    SceneState_b0* m_mappedCB0{};

    Impl(const TextureBlob& blob, const TextureParams& options) :
        m_blob(blob),
        m_pipelineState(makePipelineState(options))
    {
        createDescriptorHeap(m_blob.p_impl->m_format);
    }

    // TODO: 行列の更新をできるようにする

    void createDescriptorHeap(DXGI_FORMAT format)
    {
        // 定数バッファ作成
        using namespace DirectX;

        const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(AlignedSize(sizeof(SceneState_b0), 256));
        AssertWin32{"failed to create commited resource"sv}
            | EngineCore.GetDevice()->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&m_constantBuffer));

        AssertWin32{"failed to map constant buffer"sv}
            | m_constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedCB0));
        // TODO: Unmap?

        // ディスクリプタヒープの作成
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
        descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        descriptorHeapDesc.NodeMask = 0;
        descriptorHeapDesc.NumDescriptors = 2; // SRV と CBV の2つ
        descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        AssertWin32{"failed to create descriptor heap"sv}
            | EngineCore.GetDevice()->CreateDescriptorHeap(
                &descriptorHeapDesc, IID_PPV_ARGS(&m_blob.p_impl->m_descriptorHeap));

        // CBV 作成
        auto basicHeapHandle = m_blob.p_impl->m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = static_cast<UINT>(m_constantBuffer->GetDesc().Width);

        EngineCore.GetDevice()->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

        // テクスチャビュー (SRV) 作成
        basicHeapHandle.ptr += EngineCore.GetDevice()->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MipLevels = 1;

        EngineCore.GetDevice()->CreateShaderResourceView(
            m_blob.p_impl->m_textureBuffer.Get(), &srvDesc, basicHeapHandle);
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
        commandList->SetDescriptorHeaps(
            1, m_blob.p_impl->m_descriptorHeap.GetAddressOf());
        commandList->SetGraphicsRootDescriptorTable(
            0, m_blob.p_impl->m_descriptorHeap->GetGPUDescriptorHandleForHeapStart());

        m_buffer3D.Draw();
    }
};

namespace ZG
{
    TextureBlob::TextureBlob(const std::wstring& filename) :
        p_impl{std::make_shared<Impl>(filename)}
    {
    }

    TextureBlob::TextureBlob(const Image& image) :
        p_impl{std::make_shared<Impl>(image)}
    {
    }

    Texture::Texture(const TextureParams& params) :
        p_impl{std::make_shared<Impl>(params.blob, params)}
    {
    }

    void Texture::draw() const
    {
        p_impl->Draw();
    }
}
