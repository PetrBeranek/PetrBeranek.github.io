
//----------------------------------------------------------------------------------------
/**
 * \file       Sphere.h
 * \author     Petr Beránek
 * \date       2016/01/04
 * \brief      File contains Sphere class
 *
 * Also contains sphere buffer, struct for compact spheres and method to compactize spheres.
 *
*/
//----------------------------------------------------------------------------------------

#ifndef SPHERE
#define SPHERE

#include "Col.h"
#include "Material.h"
#include "Collidable.h"

struct SphereCompact
{
	float pos[3];
	float r;
	int mat;
};

class Sphere;

int countSpheres = 0;
std::vector<Sphere *> GLOBAL_BUFFER_SPHERE;

class Sphere : public Collidable
{
public:
	Vec3 loc;
	float radius;
	
	Sphere::Sphere() : Collidable()
	{
		loc = Vec3();
		matIndex = 0;
		radius = 1.0f;

		id = countSpheres;
		GLOBAL_BUFFER_SPHERE.push_back(this);
		countSpheres++;
	}

	Sphere::Sphere(Vec3 position, float f) : Collidable()
	{
		loc = position;
		matIndex = 0;
		radius = f;

		id = countSpheres;
		GLOBAL_BUFFER_SPHERE.push_back(this);
		countSpheres++;
	}

	Sphere::Sphere(Vec3 position, float rad, int matId) : Collidable()
	{
		loc = position;
		matIndex = matId;
		radius = rad;

		id = countSpheres;
		GLOBAL_BUFFER_SPHERE.push_back(this);
		countSpheres++;
	}

	virtual int collidableType()
	{
		return COLLID_SPHERE;
	}

	virtual void precomputeBounds()
	{
		xMax = loc.x + radius;
		xMin = loc.x - radius;
		yMax = loc.y + radius;
		yMin = loc.y - radius;
		zMax = loc.z + radius;
		zMin = loc.z - radius;
		boundsPrecomputed = true;
	}
	/**
	fill array by copying and compacting sphere buffer
	*/
	static void fillCompactArray(SphereCompact *out)
	{
		SphereCompact *sC;
		Sphere *s;

		int i;

		i = 0;
		std::vector<Sphere *>::iterator itS = GLOBAL_BUFFER_SPHERE.begin();
		for(; itS != GLOBAL_BUFFER_SPHERE.end();itS++)
		{
			s = *itS;
			sC = out + i;
			sC->pos[0] = s->loc.x;
			sC->pos[1] = s->loc.y;
			sC->pos[2] = s->loc.z;
			
			sC->r = s->radius;

			sC->mat = s->matIndex;

			i++;
		}
	}
};

#endif