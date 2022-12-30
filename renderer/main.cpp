#include <iostream>

#include "src/scene.h"

using namespace std;

int main() {
    Renderer renderer;
#ifdef _WIN32
    renderer.loadTungstenJSON("C:/Users/Christian/source/repos/roulette/renderer/scene.json");
#elif __APPLE__
    Renderer.loadTungstenJSON("/Users/roblesch/Projects/roulette/renderer/scene.json");
#endif
    cout << "Hello CMake." << endl;
    return 0;
}
