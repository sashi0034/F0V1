void main() {
    println("Hello, World!");

    int c = 0;

    Point p(100, 50);

    while (System::Update()) {
        p.x++;
        c = p.x;

        if (KeySpace.down()) {
            println("Count: " + c);
            p = p / 2;
        }
    }
}
