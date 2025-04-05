void main() {
    println("Hello, World!");

    int c = 0;

    while (System::Update()) {
        c++;
        if (KeySpace.down()) {
            println("Count: " + c);
        }
    }
}
