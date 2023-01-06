#include <iostream>

#include "src/renderer.h"

#include <filesystem>

int main(int argc, char *argv[]) {
    Renderer renderer;
    renderer.loadTungstenJSON(argv[1]);
    std::filesystem::remove("out.png");
    renderer.render();
    system("out.png");
    return 0;
}
