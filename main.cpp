#include<SDL2/SDL.h>
#include <stdio.h>
#include <vector>
#include <chrono>
#define WINDOW_HEIGTH 1000
#define WINDOW_WIDTH 1000

using namespace std;
using namespace std::chrono;

struct vec3d
{
	float x,y,z;
};
struct triangle
{
	vec3d d[3];
};
struct mesh
{
	vector<triangle> trs;
};
struct matrix4x4
{
	float coef[4][4] = { 0 };
};
struct matrix3x3
{
	float coef[3][3] = { 0 };
};
matrix4x4 projectionMarix;

void vecxMatrix(vec3d &x, vec3d &y, matrix4x4 &p)
{
	float w;
	y.x = x.x*p.coef[0][0] + x.y*p.coef[1][0] + x.z*p.coef[2][0] + p.coef[3][0];
	y.y = x.x*p.coef[0][1] + x.y*p.coef[1][1] + x.z*p.coef[2][1] + p.coef[3][1];
	y.z = x.x*p.coef[0][2] + x.y*p.coef[1][2] + x.z*p.coef[2][2] + p.coef[3][2];
	w = x.x*p.coef[0][3] + x.y*p.coef[1][3] + x.z*p.coef[2][3] + p.coef[3][3];

	if (w!=0.0f){
		y.x/=w; y.y/=w; y.z/=w;
	}
}
void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, SDL_Renderer* renderer)
{
	
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
	SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
	SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
	
}
void drawTriangle(triangle &T, SDL_Renderer* renderer)
{
	drawTriangle(T.d[0].x, T.d[0].y, T.d[1].x, T.d[1].y, T.d[2].x, T.d[2].y, renderer);
}
void scaleTriangle(triangle &T)
{
	T.d[0].x += 1.0f; T.d[0].y += 1.0f;
	T.d[1].x += 1.0f; T.d[1].y += 1.0f;
	T.d[2].x += 1.0f; T.d[2].y += 1.0f;

	T.d[0].x *= 0.5*WINDOW_WIDTH;
	T.d[0].y *= 0.5*WINDOW_HEIGTH;
	T.d[1].x *= 0.5*WINDOW_WIDTH;
	T.d[1].y *= 0.5*WINDOW_HEIGTH;
	T.d[2].x *= 0.5*WINDOW_WIDTH;
	T.d[2].y *= 0.5*WINDOW_HEIGTH;
}
void projectTriangle(triangle &T, triangle &projT)
{
	vecxMatrix(T.d[0], projT.d[0], projectionMarix);
	vecxMatrix(T.d[1], projT.d[1], projectionMarix);
	vecxMatrix(T.d[2], projT.d[2], projectionMarix);
}
void translateTriangle(triangle &T, triangle &transT, float offset){
	transT.d[0].x = T.d[0].x;
	transT.d[0].y = T.d[0].y;
	transT.d[0].z = T.d[0].z + offset;
	transT.d[1].x = T.d[1].x;
	transT.d[1].y = T.d[1].y;
	transT.d[1].z = T.d[1].z + offset;
	transT.d[2].x = T.d[2].x;
	transT.d[2].y = T.d[2].y;
	transT.d[2].z = T.d[2].z + offset;
}
vec3d getNormal(triangle &T){
	vec3d normal, line1, line2;

	line1.x = T.d[1].x - T.d[0].x;
	line1.y = T.d[1].y - T.d[0].y;
	line1.z = T.d[1].z - T.d[0].z;

	line2.x = T.d[2].x - T.d[0].x;
	line2.y = T.d[2].y - T.d[0].y;
	line2.z = T.d[2].z - T.d[0].z;

	normal.x = line1.y*line2.z - line1.z*line2.y;
	normal.y = line1.z*line2.x - line1.x*line2.z;
	normal.z = line1.x*line2.y - line1.y*line2.x;

	float norm = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);

	normal.x /= norm; normal.y /= norm; normal.z /= norm;

	return normal;
}
void rotateTriangle(triangle &T, triangle &rotatedT, matrix4x4 &mat)
{
	vecxMatrix(T.d[0], rotatedT.d[0], mat);
	vecxMatrix(T.d[1], rotatedT.d[1], mat);
	vecxMatrix(T.d[2], rotatedT.d[2], mat);
}
matrix4x4 createRotationMatrix(float angle, char axis){
	matrix4x4 mat;
	switch(axis){
		case 'x' :
			mat.coef[0][0] = 1;
			mat.coef[1][1] = cosf(angle);
			mat.coef[1][2] = sinf(angle);
			mat.coef[2][1] = -sinf(angle);
			mat.coef[2][2] = cosf(angle);
			mat.coef[3][3] = 1;
			break;
		case 'y' :
			mat.coef[0][0] = cos(angle);
			mat.coef[0][2] = sin(angle);
			mat.coef[1][1] = 1;
			mat.coef[2][0] = -sin(angle);
			mat.coef[2][2] = cos(angle);
			break;
		case 'z' :
			mat.coef[0][0] = cosf(angle);
			mat.coef[0][1] = sinf(angle);
			mat.coef[1][0] = -sinf(angle);
			mat.coef[1][1] = cosf(angle);
			mat.coef[2][2] = 1;
			mat.coef[3][3] = 1;
			break;
	}

	return mat;
}
void routine(mesh &mesh, SDL_Renderer* renderer, float elapsed_time)
{
	matrix4x4 ZRotation = createRotationMatrix(elapsed_time*0.5f, 'z');
	matrix4x4 XRotation = createRotationMatrix(elapsed_time*0.5f, 'x');
	for (auto T: mesh.trs){
		triangle projT, transT, rotatedTX, rotatedTXZ;
		rotateTriangle(T, rotatedTX, XRotation);
		rotateTriangle(rotatedTX, rotatedTXZ, ZRotation);
		translateTriangle(rotatedTXZ, transT, 3.0f);
		vec3d normal = getNormal(transT);
		if (normal.z < 0){
			projectTriangle(transT, projT);
			scaleTriangle(projT);
			drawTriangle(projT, renderer);
		}
	}
}
int main(int argc, char* argv[])
{
	auto initial_time = system_clock::now();
	mesh cube;
	cube.trs = {
		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST                                                      
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// NORTH                                                     
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

		// WEST                                                      
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

		// TOP                                                       
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		// BOTTOM                                                    
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
	};


	float fNear = 0.1f;
	float fFar = 1000.0f;
	float theta = 90.0f;
	float fFov = 1.0 / tanf((theta * 0.5f / 180.0f) *3.14159f);
	float q = (fFar)/(fFar - fNear);
	float fAspectRatio = (float)WINDOW_HEIGTH/ (float)WINDOW_WIDTH;

	projectionMarix.coef[0][0] = fFov*fAspectRatio;
	projectionMarix.coef[1][1] = fFov;
	projectionMarix.coef[2][2] = q;
	projectionMarix.coef[3][2] = -fNear*q;
	projectionMarix.coef[2][3] = 1;

	if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;

        if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGTH, 0, &window, &renderer) == 0) {
            SDL_bool done = SDL_FALSE;

            while (!done) {
                SDL_Event event;

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
				auto current_time = system_clock::now();
				auto elapsed = duration<float>(current_time - initial_time);
                routine(cube,renderer,elapsed.count());
                SDL_RenderPresent(renderer);

                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        done = SDL_TRUE;
                    }
                }
            }
        }

        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
    }
    SDL_Quit();
    return 0;
}