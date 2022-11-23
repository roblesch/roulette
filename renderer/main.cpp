#include <iostream>

#include "src/scene.h"

using namespace std;

int main()
{
	Scene scene = Scene::FromMitsubaXML("C:/Users/Christian/source/repos/roulette/renderer/scene_v3.xml");
	cout << "Hello CMake." << endl;
	return 0;
}
