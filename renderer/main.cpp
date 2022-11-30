#include <iostream>

#include "src/scene.h"

using namespace std;

int main()
{
#ifdef _WIN32
	Scene scene = Scene::FromMitsubaXML("C:/Users/Christian/source/repos/roulette/renderer/scene_v3.xml");
#elif __APPLE__
    Scene scene = Scene::FromMitsubaXML("/Users/roblesch/Projects/roulette/renderer/scene_v3.xml");
#endif
	cout << "Hello CMake." << endl;
	return 0;
}
