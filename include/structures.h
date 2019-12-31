#include <iostream>
#include <string>
#include <vector>
using namespace std;
struct matrix2x2
{
	float coef[2][2] = {0};
};
struct line
{
	float m,b;
	float x = INFINITY;
	float getY(float x)
	{
		return m*x + b;
	}
	float getX(float y)
	{
		if (x != INFINITY) return x;
		if (m !=0) return (y-b)/m;
		return 0;
	}
	string getString()
	{
		return "(m="+to_string(m)+", b="+to_string(b)+")";
	}
};
struct pt
{
	float x,y = {0};
	string getString()
	{
		return "("+to_string(x)+","+to_string(y)+")";
	}
	line operator+(const pt& a)
	{
		if (x == a.x) return {INFINITY, INFINITY, x};
		else
		{
			float m = (float)(a.y - y)/(float)(a.x - x);
			float b = y - m*x;
			return {m,b};
		}
	}

};
struct color
{
	int r,g,b = {255};
	int a = 255;
};
struct vec3d
{
	float x,y,z = {0};

	operator pt() const
	{
		pt r;
		r.x = x;
		r.y = y;
		return r;
	}
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
	vec3d operator*(const matrix2x2& mat)
	{	
		vec3d r;
		r.x = mat.coef[0][0]*x + mat.coef[0][1]*y;
		r.y = mat.coef[1][0]*x + mat.coef[1][1]*y;
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
	double getNorm()
	{
		return sqrtf(x*x + y*y + z*z);
	}
	void normalize()
	{
		float norm = this->getNorm();
		x /= norm; y /= norm; z /= norm;
	}
	string toString()
	{
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