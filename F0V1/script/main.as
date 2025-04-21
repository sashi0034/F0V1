float rad2deg(float rad) {
    return rad * 180.0 / 3.14159265358979323846;
}

float deg2rad(float deg) {
    return deg * 3.14159265358979323846 / 180.0;
}

void main() {
    println("Hello, World!");

    int c = 0;

    Point p(100, 50);

    PixelShader ps("./asset/shader/default2d.hlsl");
    VertexShader vs("./asset/shader/default2d.hlsl");
    Texture texture("./asset/image/mii.png", ps, vs);

    PixelShader modelPS("./asset/shader/model_pixel.hlsl");
    VertexShader modelVS("./asset/shader/model_vertex.hlsl");
    Model model("./asset/model/robot_head.obj", modelPS, modelVS);

    const auto viewMat = Mat4x4Ref::LookAt(
        Vec3(0.0f, 0.0f, -5.0f),
        Vec3(0.0f, 0.0f, 0.0f),
        Vec3(0.0f, 1.0f, 0.0f)
    );

    Graphics3D::SetViewMatrix(viewMat);

    const auto projectionMat = Mat4x4Ref::PerspectiveFov(
        deg2rad(90.0f),
        Scene::Size().horizontalAspectRatio(),
        0.1f,
        100.0f
    );

    Graphics3D::SetProjectionMatrix(projectionMat);

    auto worldMat = Mat4x4Ref::Identity().rotateY(deg2rad(45.0f));

    while (System::Update()) {
        p.x += System::DeltaTime() * 100.0f;

        if (KeySpace.pressed()) {
            texture.drawAt(p);
        }

        if (KeyA.pressed()) {
            worldMat.translate(Float3(-1, 0, 0) * System::DeltaTime());
        } else if (KeyD.pressed()) {
            worldMat.translate(Float3(1, 0, 0) * System::DeltaTime());
        }

        {
            worldMat.rotateY(deg2rad(System::DeltaTime() * 90));
            const Transformer3D t3d(worldMat);
            model.draw();
        }
    }
}
