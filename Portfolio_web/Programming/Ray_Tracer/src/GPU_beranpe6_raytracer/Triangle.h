
//----------------------------------------------------------------------------------------
/**
 * \file       Triangle.h
 * \author     Petr Beránek
 * \date       2016/01/04
 * \brief      File contains Triangle class.
 *
 * Also contains Triangle buffer, struct for compact triangles and methods to compact triangles.
 *
*/
//----------------------------------------------------------------------------------------


#ifndef TRIANGLE
#define TRIANGLE

#define SIZE_OFFSET 0.01f

#include "Collidable.h"
#include "Vec3.h"
#include "Material.h"
#include <algorithm>

struct TriangleCompact
{
	float v0[3];
	float v1[3];
	float v2[3];
	
	float n0[3];
	float n1[3];
	float n2[3];

	int mat;
};

class Triangle;

std::vector<Triangle *> GLOBAL_BUFFER_TRIANGLE;
int countTriangles = 0;

class Triangle : public Collidable
{
public:
	Vec3 v0;
	Vec3 v1;
	Vec3 v2;
	Vec3 n0;
	Vec3 n1;
	Vec3 n2;

	Triangle::Triangle(Vec3& u0, Vec3& u1, Vec3& u2, int matId) : Collidable()
	{
		v0 = Vec3(u0);
		v1 = Vec3(u1);
		v2 = Vec3(u2);
		matIndex = matId;
		Vec3 tmp1 = Vec3(v1);
		tmp1.subLoc(v0);
		Vec3 tmp2 = Vec3(v2);
		tmp2.subLoc(v0);
		
		Vec3 n = Vec3();
		Vec3::cross(tmp1, tmp2, n);

		Vec3 nTrue = Vec3(n);
		nTrue.normalizeLoc();
		n0 = Vec3(nTrue);
		n1 = Vec3(nTrue);
		n2 = Vec3(nTrue);

		id = countTriangles;
		GLOBAL_BUFFER_TRIANGLE.push_back(this);
		countTriangles++;
	}

	Triangle::Triangle(Vec3& u0, Vec3& u1, Vec3& u2, int matId, Vec3& n) : Collidable()
	{
		v0 = Vec3(u0);
		v1 = Vec3(u1);
		v2 = Vec3(u2);
		matIndex = matId;

		Vec3 nTrue = Vec3(n);
		nTrue.normalizeLoc();
		n0 = Vec3(nTrue);
		n1 = Vec3(nTrue);
		n2 = Vec3(nTrue);

		id = countTriangles;
		GLOBAL_BUFFER_TRIANGLE.push_back(this);
		countTriangles++;
	}

	Triangle::Triangle(Vec3& u0, Vec3& u1, Vec3& u2, int matId, Vec3& nor0, Vec3& nor1, Vec3& nor2) : Collidable()
	{
		v0 = Vec3(u0);
		v1 = Vec3(u1);
		v2 = Vec3(u2);
		
		matIndex = matId;

		n0 = Vec3(nor0);
		n0.normalizeLoc();
		n1 = Vec3(nor1);
		n1.normalizeLoc();
		n2 = Vec3(nor2);
		n2.normalizeLoc();
		Vec3 tmp1 = Vec3(v1);
		tmp1.subLoc(v0);
		Vec3 tmp2 = Vec3(v2);
		tmp2.subLoc(v0);

		Vec3 n = Vec3();
		Vec3::cross(tmp1, tmp2, n);

		id = countTriangles;
		GLOBAL_BUFFER_TRIANGLE.push_back(this);
		countTriangles++;
	}
	

	virtual int collidableType()
	{
		return COLLID_TRIANGLE;
	}

	virtual void precomputeBounds()
	{
		
		xMax = std::max(std::max(v0.x, v1.x),v2.x);
		xMin = std::min(std::min(v0.x, v1.x),v2.x);

		if(xMax - xMin < SIZE_OFFSET*2)
		{
			xMax += SIZE_OFFSET;
			xMin -= SIZE_OFFSET;
		}

		yMax = std::max(std::max(v0.y, v1.y),v2.y);
		yMin = std::min(std::min(v0.y, v1.y),v2.y);
		
		if(yMax - yMin < SIZE_OFFSET*2)
		{
			yMax += SIZE_OFFSET;
			yMin -= SIZE_OFFSET;
		}

		zMax = std::max(std::max(v0.z, v1.z),v2.z);
		zMin = std::min(std::min(v0.z, v1.z),v2.z);
		
		if(zMax - zMin < SIZE_OFFSET*2)
		{
			zMax += SIZE_OFFSET;
			zMin -= SIZE_OFFSET;
		}
		boundsPrecomputed = true;
	}
	/**
	Fill aray bycopying and compacting triangle buffer.
	*/
	static void fillCompactArray(TriangleCompact *out)
	{
		TriangleCompact *tC;
		Triangle *t;

		int i;

		i = 0;
		std::vector<Triangle *>::iterator itT = GLOBAL_BUFFER_TRIANGLE.begin();
		for(; itT != GLOBAL_BUFFER_TRIANGLE.end(); itT++)
		{
			t = *itT;
			tC = out + i;

			tC->v0[0] = t->v0.x;
			tC->v0[1] = t->v0.y;
			tC->v0[2] = t->v0.z;

			tC->v1[0] = t->v1.x;
			tC->v1[1] = t->v1.y;
			tC->v1[2] = t->v1.z;

			tC->v2[0] = t->v2.x;
			tC->v2[1] = t->v2.y;
			tC->v2[2] = t->v2.z;
			
			tC->n0[0] = t->n0.x;
			tC->n0[1] = t->n0.y;
			tC->n0[2] = t->n0.z;

			tC->n1[0] = t->n1.x;
			tC->n1[1] = t->n1.y;
			tC->n1[2] = t->n1.z;

			tC->n2[0] = t->n2.x;
			tC->n2[1] = t->n2.y;
			tC->n2[2] = t->n2.z;

			tC->mat = t->matIndex;

			i++;
		}
	}

};

#endif