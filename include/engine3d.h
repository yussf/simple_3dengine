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
	vec3d eye_vec 				= {0.0f,0.0f,0.0f};
	vec3d dir_vec				= {0.0f,0.0f,1.0f};
	vec3d up_vec				= {0.0f,1.0f,0.0f};
	vec3d light_source 			= {0.0f,0.0f,-1.0f};
	vec3d dforward_vec			= {0.0f,0.0f,-1.0f};
	float coef_translation		= 3.0f;
	float dcam_const			= 0.5f;
	float min_L					= 0.2f;
	bool draw_edges				= 0.0f;
	float yaw					= 0.0f;
	float dyaw					= 0.1f;
	float velocity				= 0.2f;
	float xmeshRot, ymeshRot, zmeshRot;
	int ctrlPressed 			= 0;
	int x0, y0, xf, yf;

	vec3d vecxMatrix(vec3d &x, matrix4x4 &p)
	{
		vec3d y;
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
		return y;
	}
	int clipTriangle(plane &p, triangle &inT, triangle &outT1, triangle &outT2)
	{
		vec3d* in_points[3]; 
		vec3d* out_points[3];
		int in_count = 0;
		int out_count = 0;
		int d[3];
		//calculate distances
		for(int i=0;i<3;i++) d[i] = p.getDist(inT.d[i]);
		//check if vertices are inside or outside
		for(int i=0;i<3;i++){
			if(d[i]>=0) in_points[in_count++] = &inT.d[i];
			else out_points[out_count++] = &inT.d[i];
		}
		vec3d interT1, interT2;
		switch (in_count)
		{
		case 0	:
			return 0;
			break;
		case 3	:
			outT1 = inT;
			return 1;
			break;
		case 1	:
			interT1 = p.getIntersectionWithRay(*in_points[0],*out_points[0]);
			interT2 = p.getIntersectionWithRay(*in_points[0],*out_points[1]);
			outT1.d[0] = *in_points[0];
			outT1.d[1] = interT1;
			outT1.d[2] = interT2;
			return 1;
			break;
		case 2	:
			interT1 = p.getIntersectionWithRay(*in_points[0],*out_points[0]);
			interT2 = p.getIntersectionWithRay(*in_points[1],*out_points[0]);
			//polarity might be wrong
			//TODO
			outT1.d[0] = *in_points[0];
			outT1.d[1] = *in_points[1];
			outT1.d[2] = interT1;
			outT2.d[0] = *in_points[0];
			outT2.d[1] = interT1;
			outT2.d[2] = interT2;
			return 2;
			break;
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
	triangle transformTriangle(triangle &T, matrix4x4 &P, vec3d b = {0,0,0})
	{
		triangle transfromedT;
		transfromedT.d[0] = vecxMatrix(T.d[0], P) + b;
		transfromedT.d[1] = vecxMatrix(T.d[1], P) + b;
		transfromedT.d[2] = vecxMatrix(T.d[2], P) + b;
		return transfromedT;
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
				mat.coef[1][2] = -sinf(angle);
				mat.coef[2][1] = sinf(angle);
				mat.coef[2][2] = cosf(angle);
				mat.coef[3][3] = 1;
				break;
			case 'y' :
				mat.coef[0][0] = cosf(angle);
				mat.coef[1][1] = 1;
				mat.coef[0][2] = sinf(angle);
				mat.coef[2][0] = -sinf(angle);
				mat.coef[2][2] = cosf(angle);
				mat.coef[3][3] = 1;
				break;
			case 'z' :
				mat.coef[0][0] = cosf(angle);
				mat.coef[0][1] = -sinf(angle);
				mat.coef[1][0] = sinf(angle);
				mat.coef[1][1] = cosf(angle);
				mat.coef[2][2] = 1;
				mat.coef[3][3] = 1;
				break;
		}
		return mat;
	}
	matrix4x4 createEyeMatrix(){
		matrix4x4 mat;
		mat.coef[0][0] = 1.0f;
		mat.coef[1][1] = 1.0f;
		mat.coef[2][2] = 1.0f;
		mat.coef[3][3] = 1.0f;
		return mat;
	}
	matrix4x4 createLookAtMatrix(vec3d &eye, vec3d &target, vec3d &up)
	{
		vec3d zaxis = target - eye;
		zaxis.normalize();
		vec3d xaxis = up ^ zaxis;
		xaxis.normalize();
		vec3d yaxis = zaxis ^ xaxis;
		matrix4x4 viewMatrix{{	
				{xaxis.x,		yaxis.x,		zaxis.x,		0},
				{xaxis.y,		yaxis.y,		zaxis.y,		0},
				{xaxis.z,		yaxis.z,		zaxis.z,		0},
				{-(xaxis*eye),	-(yaxis*eye),	-(zaxis*eye),	1}
			}};
		return viewMatrix;
	}
	matrix4x4 createPointAtMatrix(vec3d &eye, vec3d &target, vec3d &up)
	{
		vec3d zaxis = eye - target;
		zaxis.normalize();
		vec3d xaxis = up ^ zaxis;
		xaxis.normalize();
		vec3d yaxis = zaxis ^ xaxis;
		matrix4x4 camMatrix{{	
				{xaxis.x,		xaxis.y,		xaxis.z,		0},
				{yaxis.x,		yaxis.y,		yaxis.z,		0},
				{zaxis.x,		zaxis.y,		zaxis.z,		0},
				{eye.x,			eye.y,			eye.z,			1}
			}};
		return camMatrix;
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
				vec3d p = {-stof(a), -stof(b), stof(c)};
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
		matrix4x4 XRotation 	= createRotationMatrix(xmeshRot, 'x');
		matrix4x4 YRotation 	= createRotationMatrix(ymeshRot, 'y');
		matrix4x4 ZRotation 	= createRotationMatrix(zmeshRot, 'z');
		matrix4x4 worldMatrix 	= ZRotation*YRotation*XRotation;
		//worldMatrix = createEyeMatrix()
		matrix4x4 yawMatrix		= createRotationMatrix(yaw,'y');
		vec3d offset_vec		= {0.0f,0.0f,coef_translation};
		vec3d target_vec 		= {0.0f,0.0f,1.0f};
		dir_vec 				= vecxMatrix(target_vec,yawMatrix);
		target_vec				= eye_vec + dir_vec;
		matrix4x4 viewMatrix 	= createLookAtMatrix(eye_vec,target_vec,up_vec);
		vector<triangle> visibleTriangles;

		for (auto T : _mesh.T)
		{
			triangle projT, worldT, viewT;
			//world transformation
			worldT = transformTriangle(T,worldMatrix,offset_vec);
			
			//checking if triangle is visible or not
			vec3d normal = getNormal(worldT);
			if (normal * (worldT.d[1] - eye_vec) < 0)
			{
				//view transformation
				viewT = transformTriangle(worldT, viewMatrix);

				//clipping transformation
				plane nearPlane = {{0.0f,0.0f,1.0f},{0.0f,0.0f,1.0f}};
				triangle clippedT[2];
				int nClipped = clipTriangle(nearPlane,viewT,clippedT[0],clippedT[1]);
				//if (nClipped == 0) cout << "Told ya" << endl;

				for (int j=0;j<nClipped;j++)
				{					
					//projection transformation
					projT = transformTriangle(clippedT[j], projectionMarix);

					//scaling for the window
					scaleTriangle(projT);

					//determining color shades
					projT.L = abs(light_source*normal) + min_L;

					//add to the drawing stack to be sorted by z
					visibleTriangles.push_back(projT);
					
				}
			}
		}
		
		//sorting the stack by z-value
		sort(visibleTriangles.begin(), visibleTriangles.end(), [](triangle &A, triangle &B){
			float za = A.d[0].z + A.d[1].z + A.d[2].z ;
			float zb = B.d[0].z + B.d[1].z + B.d[2].z ;
			return za > zb;
		});
		
		//drawing the triangles on the screen
		for (triangle T : visibleTriangles){
			fill_triangle(T,T.L);
			if (draw_edges) draw_triangle(T,{0,0,0,255});
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
				eye_vec.x += dcam_const;
				break;
			case SDLK_LEFT 	: 
				eye_vec.x -= dcam_const;
				break;
			case SDLK_UP	:
				eye_vec.y -= dcam_const;
				break;
			case SDLK_DOWN	:
				eye_vec.y += dcam_const;
				break;
			case SDLK_q :
				yaw += dyaw;
				break;
			case SDLK_d :
				yaw -= dyaw;
				break;
			case SDLK_z :
				eye_vec += dir_vec*velocity;
				break;
			case SDLK_s	:
				eye_vec -= dir_vec*velocity;
				break;
			case SDLK_LCTRL	:
				ctrlPressed = 1;
			default:
				break;
		}
		return 0;
	}
	int on_keyup(SDL_Keycode key) override
	{
		switch (key)
		{
		case SDLK_LCTRL:
			ctrlPressed = 0;
			break;
		default:
			break;
		}
	}
	int on_wheel(Sint32 y) override
	{
		if (y > 0)
		{
			eye_vec.z += dcam_const;
		}else if (y < 0)
		{
			eye_vec.z -= dcam_const;
		}
		return 0;
	}
	int on_mouse(Uint32 type) override
	{	
		cout << ctrlPressed << endl;;
		float mouse_calibration_coef = 0.001;
		if (type == SDL_MOUSEBUTTONDOWN)
		{
			SDL_GetGlobalMouseState(&x0,&y0);
		}
		else if(type == SDL_MOUSEBUTTONUP)
		{
			SDL_GetGlobalMouseState(&xf, &yf);
			if (!ctrlPressed)
			{
				int dx = xf - x0;
				int dy = yf - y0;
				//cout << dx << "||" << dy << endl;
				//if mouse motion is mostly along the x-axis
				if (dx > dy) 	ymeshRot += (xf - x0)*mouse_calibration_coef;
				else 			xmeshRot += (yf - y0)*mouse_calibration_coef;
			}
			else
			{
				zmeshRot += (yf - y0)*mouse_calibration_coef;
			}
		}
	}

};