void main() {
    println("Hello, World!");

    int c = 0;

    while (System::Update()) {
        c++;
        if (c % 100 == 0) {
            println("Count: " + c);
        }
    }
}
