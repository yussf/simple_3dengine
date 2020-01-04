/*
	Author 		: Youssef Doubli
	Github 		: github.com/yussf
	Email		: doubli@eurecom.fr
	Last update : 3/01/2020

*/
#define DEBUG 0
#if DEBUG == 0
	#include "engine3d.h" 
#else
	#include "debugger.h"
#endif

int main(int argc, char* argv[])
{
	string mesh_path;
	if(argc == 2)
	{
		mesh_path = argv[1];
	}else
	{
		cout << "please specifiy the .obj file to render.\nDefault mesh has been selected." << endl;
		mesh_path = "objs/mountains.obj";
	}
#if DEBUG == 0
	engine3d instance3d;
	instance3d.fg_rate 			= 2000;
	instance3d.coef_reduction 	= 0.08;
	instance3d.coef_translation = 8;
	instance3d.draw_edges	 	= 0;
	if(!instance3d.init()){
		instance3d.load_mesh(mesh_path);
		instance3d.start();
	}
#else
	debugger debug;
	if(!debug.init()) debug.start();
#endif
    return 0;
}