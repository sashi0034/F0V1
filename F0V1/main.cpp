#include "pch.h"

#include "ZG/Buffer3D.h"
#include "ZG/Graphics3D.h"
#include "ZG/Image.h"
#include "ZG/Mat4x4.h"

#include "ZG/Shader.h"
#include "ZG/System.h"
#include "ZG/Texture.h"

#include "ZG/Math.h"
#include "ZG/Model.h"
#include "ZG/Scene.h"
#include "ZG/Transformer3D.h"

using namespace ZG;

namespace
{
}

void Main()
{
    // const Buffer3D buffer3D{
    //     Buffer3DParams{
    //         .vertexes = {
    //             {{-0.5f, -0.9f, 0.0f}, {0.0f, 1.0f}}, //左下
    //             {{-0.0f, 0.9f, 0.0f}, {0.0f, 0.0f}}, //左上
    //             {{0.5f, -0.9f, 0.0f}, {1.0f, 1.0f}}, //右下
    //             {{0.0f, 0.9f, 0.0f}, {1.0f, 0.0f}}, //右上
    //         },
    //         .indices = {0, 1, 2, 2, 1, 3}
    //     }
    // };

    const PixelShader pixelShader{ShaderParams{.filename = L"asset/basic_pixel.hlsl", .entryPoint = "PS"}};
    const VertexShader vertexShader{ShaderParams{.filename = L"asset/basic_vertex.hlsl", .entryPoint = "VS"}};

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

    const TextureBlob noiseBlob{image};

    const TextureBlob pngBlob{L"asset/mii.png"};

    const Texture noiseTexture{
        TextureParams{.blob = noiseBlob, .pixelShader = pixelShader, .vertexShader = vertexShader}
    };

    const Texture pngTexture{
        TextureParams{.blob = pngBlob, .pixelShader = pixelShader, .vertexShader = vertexShader}
    };

    Mat4x4 worldMat = Mat4x4::Identity().rotatedY(45.0_deg);

    const Mat4x4 viewMat = Mat4x4::LookAt(Vec3{0, 0, -5}, Vec3{0, 0, 0}, Vec3{0, 1, 0});

    const Mat4x4 projectionMat = Mat4x4::PerspectiveFov(
        90.0_deg,
        Scene::Size().horizontalAspectRatio(),
        1.0f,
        10.0f
    );

    const Model model{
        ModelParams{
            .filename = "asset/robot_head.obj",
            .pixelShader = pixelShader,
            .vertexShader = vertexShader,
        }
    };

    Graphics3D::SetViewMatrix(viewMat);
    Graphics3D::SetProjectionMatrix(projectionMat);

    int count{};
    while (System::Update())
    {
        worldMat = worldMat.rotatedY(Math::ToRadians(System::DeltaTime() * 90));
        const Transformer3D t3d{worldMat};

        count++;
        if (count % 120 < 60)
        {
            pngTexture.draw();
        }
        else
        {
            noiseTexture.draw();
        }
    }
}
