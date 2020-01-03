#include "engine3d.h" 
//#include "debugger.h"


int main(int argc, char* argv[])
{
	int mode = 0;
	if (argc > 1) mode = atoi(argv[1]);
	
	engine3d instance3d;
	instance3d.fg_rate 			= 2000;
	instance3d.coef_reduction 	= 0.08;
	instance3d.coef_translation = 8;
	instance3d.draw_edges	 	= 0;
	instance3d.min_L			= 0;
	if(!instance3d.init(mode)){
		instance3d.load_mesh("objs/teapot.obj");
		instance3d.start();
	}
	
	/*fillTriangleDebugger debugger;
	if(!debugger.init(mode)) debugger.start();*/

	// debugger debug;
	// if(!debug.init(mode)) debug.start();
    return 0;
}