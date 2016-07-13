
//----------------------------------------------------------------------------------------
/**
 * \file       Collidable.h
 * \author     Petr Beránek
 * \date       2016/01/04
 * \brief      File contains Collidable class.
 *
 *	Collidable is any object that can be in scene and can be rendered.
 *
*/
//----------------------------------------------------------------------------------------


#ifndef COLLIDABLE
#define COLLIDABLE

#define COLLID_SPHERE	0
#define COLLID_TRIANGLE	1
#define COLLID_POINT	2

#define AXI_X 0
#define AXI_Y 1
#define AXI_Z 2

#include "RayTask.h"


class Collidable{
protected:
public:
	int id;
	int matIndex;
	bool boundsPrecomputed;
	float xMin,xMax,yMin,yMax,zMin,zMax;

	Collidable::Collidable()
	{
		boundsPrecomputed = false;
		xMin = yMin = zMin = std::numeric_limits<float>::max();
		xMax = yMax = zMax = -std::numeric_limits<float>::max();
	}

	virtual int collidable(){return -1;}
	/**
	Compute parameters of AABB for this object
	*/
	virtual void precomputeBounds(){}
	/**
	To identify what object it is
	*/
	virtual int collidableType(){return -1;}
	
	float getMin(int axis)
	{
		if(axis == AXI_X){return xMin;}
		if(axis == AXI_Y){return yMin;}
		return zMin;
	}

	float getMax(int axis)
	{
		if(axis == AXI_X){return xMax;}
		if(axis == AXI_Y){return yMax;}
		return zMax;
	}
};

#endif