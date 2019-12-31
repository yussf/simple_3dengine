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

	/*engine3d demo;

	if(!demo.init(mode)) demo.start();
	*/

	fillTriangleDebugger debugger;
	if(!debugger.init(mode)) debugger.start();
    return 0;
}