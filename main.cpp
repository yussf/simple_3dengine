#include<SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#define WINDOW_HEIGTH 360
#define WINDOW_WIDTH 480
#define DEBUG_MODE 0
using namespace std;
using namespace std::chrono;

struct vec3d
{
	float x,y,z = {0};

	vec3d operator+(const vec3d& a) const
    {
		vec3d r;
		r.x = x + a.x;
		r.y = y + a.y;
		r.z = z + a.z;
        return r;
    }
	vec3d operator-(const vec3d& a) const
	{
		vec3d r;
		r.x = x - a.x;
		r.y = y - a.y;
		r.z = z - a.z;
		return r;
	}
	vec3d operator+(const float d) const
	{
		vec3d r;
		r.x = x + d;
		r.y = y + d;
		r.z = z + d;
		return r;
	}
	vec3d operator*(const float& k) const
	{
		vec3d r;
		r.x = x*k;
		r.y = y*k;
		r.z = z*k;
		return r;
	}
	float operator*(const vec3d& a) const
	{
		float r = 0;
		r += x * a.x;
		r += y * a.y;
		r += z * a.z;
		return r;
	}
	vec3d operator^(const vec3d& a) const
	{
		vec3d r;
		r.x = y*a.z - z*a.y;
		r.y = z*a.x - x*a.z;
		r.z = x*a.y - y*a.x;
		return r;
	}
	double getNorm(){
		return sqrtf(x*x + y*y + z*z);
	}
	vec3d normalize(){
		float norm = this->getNorm();
		x /= norm; y /= norm; z /= norm;
	}
	string toString(){
		return "("+to_string(x)+","+to_string(y)+","+to_string(z)+")";
	}
};
struct triangle
{
	vec3d d[3];
};
struct mesh
{
	vector<triangle> T;
};
struct matrix4x4
{
	float coef[4][4] = {0};
};
struct matrix3x3
{
	float coef[3][3] = {0};
};
matrix4x4 projectionMarix;
vec3d CamVec;
float global_step;
int print;
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
void translateTriangle(triangle &T, triangle &transT, float offset)
{
	vec3d vOffset = {0,0,offset};
	for (int i = 0; i<3; i++){
		transT.d[i] = T.d[i] + vOffset;
	}
}
vec3d getNormal(triangle &T){
	vec3d normal, line1, line2;

	line1 = T.d[1] - T.d[0];
	line2 = T.d[2] - T.d[0];

	normal = line1^line2;
	normal.normalize();
	
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
	for (auto T : mesh.T){
		triangle projT, transT, rotatedTX, rotatedTXZ;
		rotateTriangle(T, rotatedTX, XRotation);
		rotateTriangle(rotatedTX, rotatedTXZ, ZRotation);
		translateTriangle(rotatedTXZ, transT, 3.0f);
		vec3d normal = getNormal(transT);
#if DEBUG_MODE
		if (print){
			cout << "triangle" << endl;
			cout << normal.toString() << endl;
			cout << normal * (transT.d[1] - CamVec) << endl;
			cout << "===========================" << endl;
		}
		if (i == 3) print = 0;
#endif
		if (normal * (transT.d[0] - CamVec) < 0)
		{
			projectTriangle(transT, projT);
			scaleTriangle(projT);
			drawTriangle(projT, renderer);
		}
	}
}
int main(int argc, char* argv[])
{
	int mode;
	if (argc > 1){
		mode = atoi(argv[1]);
	}else{
		mode = 0;
	}
	auto initial_time = system_clock::now();
	mesh cube;
	print = 0;
	cube.T = {
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
				if (mode == 0){
				auto current_time = system_clock::now();
				auto elapsed = duration<float>(current_time - initial_time);
                routine(cube,renderer,elapsed.count());
				}else if(mode == 1){
					routine(cube,renderer,global_step);
				}
                SDL_RenderPresent(renderer);

                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        done = SDL_TRUE;
                    }
					else if(event.type == SDL_KEYDOWN){
                        switch(event.key.keysym.sym)
                        {
                            case SDLK_UP:
                            break;

                            case SDLK_DOWN:
                            break;

                            case SDLK_LEFT:
							global_step -= 0.05;
							print = 1;
                            break;

                            case SDLK_RIGHT:
							global_step += 0.05;
							print = 1;
                            break;

                            default:
                            break;
                        }
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