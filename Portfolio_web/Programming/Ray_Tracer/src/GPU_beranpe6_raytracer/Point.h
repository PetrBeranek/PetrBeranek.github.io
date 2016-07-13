
//----------------------------------------------------------------------------------------
/**
 * \file       Point.h
 * \author     Petr Beránek
 * \date       2016/01/04
 * \brief      File contains Point class
 *
 * Points were used for debugging of KDTree building. 
 *
*/
//----------------------------------------------------------------------------------------

#ifndef POINT
#define POINT

#include "Collidable.h"

class Point : public Collidable
{
public:
	Vec3 loc;
	
	Point::Point() : Collidable()
	{
		loc = Vec3();
	}
	
	Point::Point(Vec3 position) : Collidable()
	{
		loc = Vec3(position);
	}

	Point::Point(float x, float y, float z) : Collidable()
	{
		loc = Vec3(x, y, z);
	}

	virtual int collidableType()
	{
		return COLLID_POINT;
	}

	virtual void precomputeBounds()
	{
		xMax = loc.x;
		xMin = loc.x;
		yMax = loc.y;
		yMin = loc.y;
		zMax = loc.z;
		zMin = loc.z;
		boundsPrecomputed = true;
	}

};

#endif