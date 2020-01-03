#include "engine3d.h"
class debugger : public engine3d
{
	mesh mesh_history;
	int nClipped;
	rgb cmap_3[3] = {{255,0,0},{0,255,0},{0,0,255}};
	void check_state()
	{
		for (auto T : mesh_history.T)
		{
			cout << (string)T << endl;
			vector<pt> nodes = {(pt)T.d[0], (pt)T.d[1], (pt)T.d[2]};
			line line1 = nodes[0] + nodes[2];
			line line2 = nodes[0] + nodes[1];
			line line3 = nodes[1] + nodes[2];
			cout << "red : " << (string) line1 << "| green : " << (string) line2 << "| blue : " << (string) line3 << "|" << endl;
			cout << "nclipped=" << nClipped << endl;
            cout << "=========== END ===========" << endl;
			draw_triangle(T,cmap_3);
		}
		
		
	}
	int on_create() override
	{
		_mesh.T = {
			{0.0f, 0.0f, 0.0f,    0.0f, 0.7f, 0.0f,    0.4f, 0.4f, 0.0f}
		};
		return 0;
	}
	int on_update(float elapsed) override
	{
		rgb c[2];
		c[0] = {255,0,0};
		c[1] = {0,255,0,0};
		matrix2x2 mat = createRotationMatrix(coef_reduction);
		triangle rotatedT;
		for (auto T : _mesh.T)
		{
			mesh_history.T.clear();
			rotateTriangle(T,rotatedT,mat);
			scaleTriangle(rotatedT);
			cout << "original :" << (string)rotatedT << endl;
			plane nearPlane = {{450,650,0},{1,0,0}};
			triangle clippedT[2];
			nClipped = clipTriangle(nearPlane,rotatedT,clippedT[0],clippedT[1]);
			fill_triangle(rotatedT);
			for (int j=0; j<nClipped;j++)
			{
				cout << "clipped :" << (string)clippedT[j] << endl;
				draw_triangle(clippedT[j],c[j]);
				//fill_triangle(clippedT[j],c[j]);
			}
			
			mesh_history.T.push_back(rotatedT);
		}
		return 0;
	}
	int on_keydown(SDL_Keycode key) override
	{
		//keep super on_keydown()
		engine3d::on_keydown(key);
		if (key == SDLK_SPACE) check_state();
		return 0;
	}
};

