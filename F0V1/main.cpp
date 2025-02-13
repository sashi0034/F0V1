#include "pch.h"

#include "ZG/Buffer3D.h"
#include "ZG/Image.h"

#include "ZG/PipelineState.h"
#include "ZG/Shader.h"
#include "ZG/System.h"
#include "ZG/Texture.h"

using namespace ZG;

namespace
{
}

void Main()
{
    const Buffer3D buffer3D{
        Buffer3DParams{
            .vertexes = {
                {{-0.5f, -0.9f, 0.0f}, {0.0f, 1.0f}}, //左下
                {{-0.1f, 0.9f, 0.0f}, {0.0f, 0.0f}}, //左上
                {{0.5f, -0.9f, 0.0f}, {1.0f, 1.0f}}, //右下
                {{0.1f, 0.9f, 0.0f}, {1.0f, 0.0f}}, //右上
            },
            .indices = {0, 1, 2, 2, 1, 3}
        }
    };

    const PixelShader pixelShader{ShaderParams{.filename = L"asset/basic_pixel.hlsl", .entryPoint = "PS"}};
    const VertexShader vertexShader{ShaderParams{.filename = L"asset/basic_vertex.hlsl", .entryPoint = "VS"}};
    const PipelineState pipelineState{PipelineStateParams{.pixelShader = pixelShader, .vertexShader = vertexShader}};

    Image image{Size{256, 256}};
    for (int x = 0; x < image.size().x; ++x)
    {
        for (int y = 0; y < image.size().y; ++y)
        {
            auto& pixel = image[Point{x, y}];
            pixel.r = rand() % 256;
            pixel.g = rand() % 256;
            pixel.b = rand() % 256;
            pixel.a = 255;
        }
    }

    Texture texture{image};

    while (System::Update())
    {
        const ScopedPipelineState scopedPipelineState{pipelineState};

        buffer3D.Draw();
    }
}
