#include "SDL_wrapper.h"
class engine3d: public SDL_wrapper
{
	
	matrix4x4 projectionMarix = createProjectionMatrix();
	mesh _mesh;
	vec3d CamVec;
	float global_step;
	
	void vecxMatrix(vec3d &x, vec3d &y, matrix4x4 &p)
	{
		float w;
		y.x = x.x*p.coef[0][0] + x.y*p.coef[1][0] + x.z*p.coef[2][0] + p.coef[3][0];
		y.y = x.x*p.coef[0][1] + x.y*p.coef[1][1] + x.z*p.coef[2][1] + p.coef[3][1];
		y.z = x.x*p.coef[0][2] + x.y*p.coef[1][2] + x.z*p.coef[2][2] + p.coef[3][2];
		w 	= x.x*p.coef[0][3] + x.y*p.coef[1][3] + x.z*p.coef[2][3] + p.coef[3][3];

		if (w!=0.0f){
			y.x /= w;
			y.y /= w;
			y.z /= w;
		}
	}
	void scaleTriangle(triangle &T)
	{
		T.d[0].x += 1.0f; T.d[0].y += 1.0f;
		T.d[1].x += 1.0f; T.d[1].y += 1.0f;
		T.d[2].x += 1.0f; T.d[2].y += 1.0f;

		T.d[0].x *= 0.5*this->get_Wwidth();
		T.d[0].y *= 0.5*this->get_Wheigth();
		T.d[1].x *= 0.5*this->get_Wwidth();
		T.d[1].y *= 0.5*this->get_Wheigth();
		T.d[2].x *= 0.5*this->get_Wwidth();
		T.d[2].y *= 0.5*this->get_Wheigth();
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
		for (int i = 0; i<3; i++)
		{
			transT.d[i] = T.d[i] + vOffset;
		}
	}
	vec3d getNormal(triangle &T)
	{
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
	matrix4x4 createProjectionMatrix()
	{
		matrix4x4 projectionMarix;
		float fNear 				= 0.1f;
		float fFar 					= 1000.0f;
		float theta 				= 90.0f;
		float fFov 					= 1.0 / tanf((theta * 0.5f / 180.0f) *3.14159f);
		float q 					= (fFar)/(fFar - fNear);
		float fAspectRatio			= (float)window_heigth/(float)window_width;
		projectionMarix.coef[0][0] 	= fFov*fAspectRatio;
		projectionMarix.coef[1][1] 	= fFov;
		projectionMarix.coef[2][2] 	= q;
		projectionMarix.coef[3][2] 	= -fNear*q;
		projectionMarix.coef[2][3] 	= 1;
		return projectionMarix;
	}
	matrix4x4 createRotationMatrix(float angle, char axis)
	{
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
				mat.coef[0][0] = cosf(angle);
				mat.coef[1][1] = 1;
				mat.coef[0][2] = -sinf(angle);
				mat.coef[2][0] = sinf(angle);
				mat.coef[2][2] = cosf(angle);
				mat.coef[3][3] = 1;
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
	int on_create() override 
	{
		
		_mesh.T = {
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
		return 0;
		
	}
	int on_update(float elapsed_time) override
	{
		matrix4x4 ZRotation = createRotationMatrix(elapsed_time*0.5f, 'z');
		matrix4x4 XRotation = createRotationMatrix(elapsed_time*0.5f, 'x');
		for (auto T : _mesh.T)
		{
			triangle projT, transT, rotatedTX, rotatedTXZ;
			rotateTriangle(T, rotatedTX, XRotation);
			rotateTriangle(rotatedTX, rotatedTXZ, ZRotation);
			translateTriangle(rotatedTXZ, transT, 3.0f);
			vec3d normal = getNormal(transT);
			if (normal * (transT.d[0] - CamVec) < 0)
			{
				projectTriangle(transT, projT);
				scaleTriangle(projT);
				draw_triangle(projT, {255,0,0});
				fill_triangle(projT, {255,255,255});
			}
		}
		return 0;
	}
	int on_update(int step, float reduction_coef) override
	{
		if (mode == 1) on_update(step*reduction_coef);
		else return 0;
	}
	int on_keydown(SDL_Keycode key) override
	{
		switch(key)
		{
			case SDLK_RIGHT : 
				stepIncrement();
				break;
			case SDLK_LEFT 	: 
				stepDecrement();
				break;
			default:
				break;
		}
	}
};