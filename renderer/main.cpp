#include <iostream>

#include "src/renderer.h"

#include <filesystem>

int main(int argc, char *argv[]) {
    Renderer renderer;
    renderer.loadTungstenJSON(argv[1]);
    std::filesystem::remove("out.png");
    renderer.render();
#ifdef _WIN32
    //system("albedo_img.png");
    //system("normal_img.png");
    system("oidn_img.png");
    system("traced_img.png");
#elif __APPLE__
    //system("open albedo_img.png");
    //system("open normal_img.png");
    system("open oidn_img.png");
    system("open traced_img.png");
#endif
    return 0;
}
