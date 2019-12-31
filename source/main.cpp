#include <SDL2/SDL.h>
#include <algorithm>
#include <chrono>
#include "engine3d.h"

using namespace std;
using namespace std::chrono;

/*
void filling_debuging(mesh &mesh, SDL_Renderer* renderer){
	for (auto T : mesh.T){
		scaleTriangle(T);
		//drawTriangle(T,renderer);
		fillTriangle(T,{255,255,0},renderer);
	}
}
*/
int main(int argc, char* argv[])
{
	int mode;
	if (argc > 1) mode = atoi(argv[1]);

	engine3d demo;
	if(!demo.init(mode)) demo.start();
	
    return 0;
}