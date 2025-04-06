void main() {
    println("Hello, World!");

    int c = 0;

    Point p(100, 50);

    PixelShader ps("./asset/shader/default2d.hlsl", "PS");
    VertexShader vs("./asset/shader/default2d.hlsl", "VS");
    TextureSource source("./asset/image/mii.png");
    Texture texture(source, ps, vs);

    while (System::Update()) {
        // println("x: " + p.x + ", y: " + p.y);
        texture.drawAt(Vec2(p.x, p.y));

        p.x++;
        c = p.x;

        if (KeySpace.down()) {
            println("Count: " + c);
            p = p / 2;
        }
    }
}
