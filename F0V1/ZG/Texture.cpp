#include "pch.h"
#include "Texture.h"

#include <d3d12.h>

#include "AssertObject.h"
#include "System.h"
#include "Utils.h"
#include "detail/EngineCore.h"

using namespace ZG;
using namespace ZG::detail;

namespace
{
}

struct Texture::Impl
{
    DirectX::TexMetadata m_metadata{};
    DirectX::ScratchImage m_scratchImage{};

    // ID3D12Resource* m_uploadBuffer{};
    ID3D12Resource* m_textureBuffer{};

    Impl(const TextureParams& params)
    {
        const auto loadResult =
            LoadFromWICFile(params.filename.c_str(), DirectX::WIC_FLAGS_NONE, &m_metadata, m_scratchImage);

        if (FAILED(loadResult))
        {
            System::ModalError(L"failed to load texture");
            throw std::runtime_error("failed to load texture"); // FIXME
        }

        // 生データを取得
        const auto rawImage = m_scratchImage.GetImage(0, 0, 0);

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
        AssertWin32{""sv}
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

        resourceDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(m_metadata.dimension);
        resourceDesc.Width = m_metadata.width;
        resourceDesc.Height = m_metadata.height;
        resourceDesc.DepthOrArraySize = m_metadata.arraySize;
        resourceDesc.MipLevels = m_metadata.mipLevels;
        resourceDesc.Format = m_metadata.format;
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
            std::memcpy(imageMap + y * srcPitch, srcAddress + y * rawImage->rowPitch, rawImage->rowPitch);
            srcAddress += rawImage->rowPitch;
            imageMap += srcPitch;
        }

        uploadBuffer->Unmap(0, nullptr);

        // -----------------------------------------------
        // アップロード用中間バッファからテクスチャバッファにコピー
        // アップロード用中間バッファにはフットプリントを指定し、テクスチャバッファにはインデックスを指定する
        D3D12_TEXTURE_COPY_LOCATION dstCopyLocation{};
        dstCopyLocation.pResource = m_textureBuffer;
        dstCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dstCopyLocation.SubresourceIndex = 0;

        D3D12_TEXTURE_COPY_LOCATION srcCopyLocation{};
        srcCopyLocation.pResource = uploadBuffer;
        srcCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT; // Footprint: メモリ占有領域に関する情報

        // FIXME: GetCopyableFootprints を使うほうがいい?
        srcCopyLocation.PlacedFootprint.Offset = 0;
        srcCopyLocation.PlacedFootprint.Footprint.Width = static_cast<UINT>(m_metadata.width);
        srcCopyLocation.PlacedFootprint.Footprint.Height = static_cast<UINT>(m_metadata.height);
        srcCopyLocation.PlacedFootprint.Footprint.Depth = static_cast<UINT>(m_metadata.depth);
        srcCopyLocation.PlacedFootprint.Footprint.RowPitch =
            static_cast<UINT>(AlignedSize(rawImage->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));
        srcCopyLocation.PlacedFootprint.Footprint.Format = rawImage->format;

        const auto commandList = EngineCore.GetCommandList();
        commandList->CopyTextureRegion(&dstCopyLocation, 0, 0, 0, &srcCopyLocation, nullptr);

        // TODO: 描画ルーチンの中で実行された場合に対応
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = m_textureBuffer;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

        commandList->ResourceBarrier(1, &barrier);
        commandList->Close();

        EngineCore.ExecuteCommandList();
    }
};

namespace ZG
{
    Texture::Texture(const TextureParams& params) : p_impl(std::make_unique<Impl>(params))
    {
    }
}
