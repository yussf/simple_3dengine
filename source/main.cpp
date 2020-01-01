#include <SDL2/SDL.h>
#include <algorithm>
#include <chrono>
//#include "engine3d.h" 
#include "debugger.h"
using namespace std;
using namespace std::chrono;


int main(int argc, char* argv[])
{
	int mode;
	if (argc > 1) mode = atoi(argv[1]);

	engine3d instance3d;
	instance3d.coef_reduction = 0.01;
	instance3d.coef_translation = 8;
	if(!instance3d.init(mode)){
		instance3d.load_mesh("objs/teapot.obj");
		instance3d.start();
	}
	/*fillTriangleDebugger debugger;
	if(!debugger.init(mode)) debugger.start();*/
    return 0;
}