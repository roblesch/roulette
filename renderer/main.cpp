#include <iostream>

#include "src/renderer.h"

#include <filesystem>

int main(int argc, char *argv[]) {
    Renderer renderer;
    renderer.loadTungstenJSON(argv[1]);
    std::filesystem::remove("out.png");
    renderer.render();
#ifdef _WIN32
    system("out.png");
#elif __APPLE__
    system("open out.png");
#endif
    return 0;
}
