#include "pch.h"

#include "ZG/Buffer3D.h"

#include "ZG/PipelineState.h"
#include "ZG/Shader.h"
#include "ZG/System.h"

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

    while (System::Update())
    {
        const ScopedPipelineState scopedPipelineState{pipelineState};

        buffer3D.Draw();
    }
}
