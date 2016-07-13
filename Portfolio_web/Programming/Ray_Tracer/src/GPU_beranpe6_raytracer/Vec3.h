
//----------------------------------------------------------------------------------------
/**
 * \file       Vector.h
 * \author     Petr Beránek
 * \date       2016/01/04
 * \brief      File contains Vec3 class that represents 3D vector
 *
 *
*/
//----------------------------------------------------------------------------------------


#ifndef VEC3
#define VEC3

#include <cmath>
#include <stdio.h>
/**
It if 3D vector
*/
class Vec3
{
public:
	float x, y, z;
	
	Vec3::Vec3 ()
	{
		x = y = z = 0;
	}

	Vec3::Vec3 (Vec3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
	
	Vec3::Vec3 (float a, float b, float c)
	{
		x = a;
		y = b;
		z = c;
	}
	
	void clear()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	void set(Vec3& p)
	{
		x = p.x;
		y = p.y;
		z = p.z;
	}

	float length ()
	{
		return sqrt(x*x + y*y + z*z);
	}
	
	float dot(Vec3 o)
	{
		return x*o.x + y*o.y + z*o.z; 
	}
	
	void addLoc (Vec3 o)
	{
		x += o.x;
		y += o.y;
		z += o.z;
	}

	void addMultipliedLoc (Vec3 o, float s)
	{
		x = x+ s*o.x;
		y = y+ s*o.y;
		z = z+ s*o.z;
	}
	
	void subLoc (Vec3 o)
	{
		x -= o.x;
		y -= o.y;
		z -= o.z;
	}
	
	void mulLoc (float s)
	{
		x *= s;
		y *= s;
		z *= s;
	}
	void normalizeLoc ()
	{
		float m = length();
		x /= m;
		y /= m;
		z /= m;
	}

	void negativeLoc ()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	static void reflect(Vec3& dirIn, Vec3& norIn, Vec3& refOut)
	{
		Vec3::copyValues(dirIn,refOut);
		refOut.normalizeLoc();

		float mult = -refOut.dot(norIn);

		refOut.addMultipliedLoc(norIn,mult*2);
		refOut.normalizeLoc();
	}

	static void copyValues(Vec3& in, Vec3& out)
	{
		out.x = in.x;
		out.y = in.y;
		out.z = in.z;
	}

	static void cross(Vec3& in1, Vec3& in2, Vec3& out)
	{
		out.x = in1.y * in2.z - in1.z * in2.y;
		out.y = in1.z * in2.x - in1.x * in2.z;
		out.z = in1.x * in2.y - in1.y * in2.x;
	}
	/**
	Interpolate vector using barycentric coordinates (interpolation of vectors parameters of points in triangle)
	*/
	static void triLerp(Vec3& out, float u, float v, Vec3& in0, Vec3& in1, Vec3& in2)
	{
		//printf(" u %f v %f x %f\n",u, v, out.x);
		out.addMultipliedLoc(in1, u);
		out.addMultipliedLoc(in2, v);
		out.addMultipliedLoc(in0, (1.0f-u-v));
	}
};

#endif