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
    const PixelShader pixelShader{ShaderParams{.filename = L"asset/shader/basic_pixel.hlsl", .entryPoint = "PS"}};
    const VertexShader vertexShader{ShaderParams{.filename = L"asset/shader/basic_vertex.hlsl", .entryPoint = "VS"}};

    Image image{Size{16, 16}};
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

    const Texture noiseTexture{
        TextureParams{.source = image, .pixelShader = pixelShader, .vertexShader = vertexShader}
    };

    const Texture pngTexture{
        TextureParams{.source = L"asset/image/mii.png", .pixelShader = pixelShader, .vertexShader = vertexShader}
    };

    Mat4x4 worldMat = Mat4x4::Identity().rotatedY(45.0_deg);

    const Mat4x4 viewMat = Mat4x4::LookAt(Vec3{0, 0, -5}, Vec3{0, 0, 0}, Vec3{0, 1, 0});

    const Mat4x4 projectionMat = Mat4x4::PerspectiveFov(
        90.0_deg,
        Scene::Size().horizontalAspectRatio(),
        1.0f,
        10.0f
    );

    const PixelShader modelPS{ShaderParams{.filename = L"asset/shader/model_pixel.hlsl", .entryPoint = "PS"}};
    const VertexShader modelVS{ShaderParams{.filename = L"asset/shader/model_vertex.hlsl", .entryPoint = "VS"}};

    const Model model{
        ModelParams{
            .filename = "asset/model/robot_head.obj", // "asset/model/cinnamon.obj"
            .pixelShader = modelPS,
            .vertexShader = modelVS,
        }
    };

    Graphics3D::SetViewMatrix(viewMat);
    Graphics3D::SetProjectionMatrix(projectionMat);

    int count{};
    while (System::Update())
    {
        worldMat = worldMat.rotatedY(Math::ToRadians(System::DeltaTime() * 90));
        const Transformer3D t3d{worldMat};

        model.draw();

        // count++;
        // if (count % 120 < 60)
        // {
        //     pngTexture.draw();
        // }
        // else
        // {
        //     noiseTexture.draw();
        // }
    }
}
