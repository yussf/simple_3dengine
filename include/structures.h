#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#define MIN(a, b) (((a) > (b)) ? (b) : (a))
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
	operator string() const
	{
		return "(m="+to_string(m)+", b="+to_string(b)+")";
	}
};
struct pt
{
	float x,y = {0};
	operator string() const
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
struct rgb
{
	int r,g,b = {255};
	int a = 255;
	operator string() const
	{
		return "(rgb  : "+to_string(r)+","+to_string(g)+","+to_string(b)+","+to_string(a)+")";
	}
	rgb operator*(const float& k) const
	{
		rgb res;
		res.r = MIN(255,r*k);
		res.g = MIN(255,g*k);
		res.b = MIN(255,b*k);
		return res;
	}
	rgb operator+(const float& k) const
	{
		rgb res;
		res.r = MIN(255,r+k);
		res.g = MIN(255,g+k);
		res.b = MIN(255,b+k);
		return res;
	}
};
// intended for better control over the luminance of rgb colors
// not used yet
struct hsl
{
	int h,s,l = {0};
	operator string() const
	{
		return "(hsl : "+to_string(h)+","+to_string(s)+","+to_string(l)+")";
	}
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
	void operator+=(const vec3d& a)
	{
		*this = *this + a;
	}
	void operator-=(const vec3d& a)
	{
		*this = *this - a;
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
	operator string() const
	{
		return "("+to_string(x)+","+to_string(y)+","+to_string(z)+")";
	}
};
struct triangle
{
	vec3d d[3];
	rgb color = {255,255,255,255};
	float L = 1;
	operator string() const
	{
		return "["+(string)d[0]+","+(string)d[1]+","+(string)d[2]+"]";
	}
};
struct mesh
{
	vector<triangle> T;
};
struct matrix4x4
{
	float coef[4][4] = {0};
	matrix4x4 operator*(const matrix4x4 &mat) const
	{
		matrix4x4 res;
		for(int i=0;i<4;i++)	
		{
			for (int j=0;j<4;j++)
			{
				for (int k=0;k<4;k++) res.coef[i][j] += coef[i][k]*mat.coef[k][j];
			}
		}
		return res;
	}
};
struct matrix3x3
{
	float coef[3][3] = {0};
};