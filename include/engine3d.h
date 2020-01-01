#include "SDL_wrapper.h"
#include <fstream>
#include <sstream>
#include <vector>
class engine3d: public SDL_wrapper
{
public:	
	float global_step;
	mesh _mesh;
	matrix4x4 projectionMarix 	= createProjectionMatrix();
	vec3d CamVec 				= {0.0f,0.0f,0.0f};
	vec3d light_source 			= {0.0f,0.0f,-1.0f};
	float coef_translation		= 3.0f;
	
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

		T.d[0].x *= 0.5*get_Wwidth();
		T.d[0].y *= 0.5*get_Wheigth();
		T.d[1].x *= 0.5*get_Wwidth();
		T.d[1].y *= 0.5*get_Wheigth();
		T.d[2].x *= 0.5*get_Wwidth();
		T.d[2].y *= 0.5*get_Wheigth();
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
	void rotateTriangle(triangle &T, triangle &rotatedT, matrix2x2 &mat)
	{
		rotatedT = {T.d[0]*mat, T.d[1]*mat, T.d[2]*mat};
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
	matrix2x2 createRotationMatrix(float angle)
	{
		matrix2x2 mat;
		mat.coef[0][0] = cosf(angle);
		mat.coef[1][1] = cosf(angle);
		mat.coef[0][1] = -sinf(angle);
		mat.coef[1][0] = sinf(angle);
		return mat;
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
	void load_mesh(string path)
	{
		mesh res;
		ifstream file_in(path);
		vector<vec3d> vertices;
		string indicator, a, b, c;
		for (string line; getline(file_in, line);){
			stringstream sstream(line);
			sstream >> indicator >> a >> b >> c;

			//useful for debugging 
			//cout << indicator << " | " << a << " | " << b << " | " << c << endl;
			
			if (indicator == "v")
			{
				vec3d p = {stof(a), stof(b), stof(c)};
				vertices.push_back(p);

			}else if(indicator == "f")
			{
				int v[3] = {stoi(a), stoi(b), stoi(c)};
				triangle T = {vertices[v[0] - 1],vertices[v[1] - 1],vertices[v[2] - 1]};
				res.T.push_back(T);
			}
		}
		_mesh = res;
	}
	int on_create() override 
	{
		load_mesh("objs/cube.obj");
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
			translateTriangle(rotatedTXZ, transT, coef_translation);
			vec3d normal = getNormal(transT);
			if (normal * (transT.d[0] - CamVec) < 0)
			{
				projectTriangle(transT, projT);
				scaleTriangle(projT);
				float L = abs(light_source*normal);
				draw_triangle(projT);
				fill_triangle(projT,L);
			}
		}
		return 0;
	}
	int on_update(int step, float reduction_coef) override
	{
		if (mode == 1) on_update(step*reduction_coef);
		return 0;
	}
	int on_keydown(SDL_Keycode key) override
	{
		switch(key)
		{
			case SDLK_RIGHT : 
				stepIncrement();
				break;
			case SDLK_LEFT 	: 
				stepIncrement(-1);
				break;
			default:
				break;
		}
		return 0;
	}

};