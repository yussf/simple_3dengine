#define DEBUG 0
#if DEBUG == 0
	#include "engine3d.h" 
#else
	#include "debugger.h"
#endif

int main(int argc, char* argv[])
{
	int mode = 0;
	if (argc > 1) mode = atoi(argv[1]);
#if DEBUG == 0
	engine3d instance3d;
	instance3d.fg_rate 			= 2000;
	instance3d.coef_reduction 	= 0.08;
	instance3d.coef_translation = 8;
	instance3d.draw_edges	 	= 1;
	if(!instance3d.init(mode)){
		instance3d.load_mesh("objs/axis.obj");
		instance3d.start();
	}
#else
	debugger debug;
	if(!debug.init(mode)) debug.start();
#endif
    return 0;
}