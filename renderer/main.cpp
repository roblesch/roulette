#include <iostream>

#include "src/renderer.h"

using namespace std;

int main(int argc, char *argv[]) {
    Renderer renderer;
    renderer.loadTungstenJSON(argv[1]);
    renderer.render();
    system("out.png");
    return 0;
}
