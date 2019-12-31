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
class fillTriangleDebugger : public engine3d
{
	int on_create() override
	{
		_mesh.T = {
			{0.0f, 0.0f, 0.0f,    0.0f, 0.7f, 0.0f,    0.4f, 0.4f, 0.0f}
		};
		return 0;
	}
	int on_update(int step, float reduction_coef) override
	{
		matrix2x2 mat = createRotationMatrix(step*reduction_coef);
		triangle rotatedT;
		for (auto T : _mesh.T)
		{
			rotateTriangle(T,rotatedT,mat);
			scaleTriangle(rotatedT);
			draw_triangle(rotatedT);
			fill_triangle(rotatedT);
		}
		return 0;
	}
};

int main(int argc, char* argv[])
{
	int mode;
	if (argc > 1) mode = atoi(argv[1]);

	/*engine3d demo;

	if(!demo.init(mode)) demo.start();
	*/

	fillTriangleDebugger debugger;
	if(!debugger.init(mode)) debugger.start();
    return 0;
}