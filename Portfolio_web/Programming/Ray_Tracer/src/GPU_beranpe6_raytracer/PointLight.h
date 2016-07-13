
//----------------------------------------------------------------------------------------
/**
 * \file       PointLight.h
 * \author     Petr Beránek
 * \date       2016/01/04
 * \brief      File contains PointLight class and struct for compact PointLight.
 *
 *	Also contains methods for compacting PointLights and PointLight buffer.
 *
*/
//----------------------------------------------------------------------------------------

#ifndef POINTLIGHT
#define POINTLIGHT

#include "Col.h"
#include "Vec3.h"
#include <vector>

struct PointLightCompact
{
	float pos[3];
	float col[3];
	float intensity;
	float a0;
	float a1;
	float a2;
};

/**
Its a pointlight.
*/
class PointLight
{
public:
	Vec3 pos;
	Col diffuse;
	float intensity;
	float a0;
	float a1;
	float a2;

	PointLight::PointLight(Vec3 position, Col& dColor, float i, float at0, float at1, float at2)
	{
		diffuse = Col(COL_WHITE);
		diffuse.setCol(dColor);
		pos = position;
		intensity = i;
		a0 = at0;
		a1 = at1;
		a2 = at2;

	}
	/**
	compute color of light modified by the distance and intensity
	*/
	void giveColor(Col& out, float dist)
	{
		float f = intensity / (((a2 * dist + a1) * dist) + a0); 
		out.r = diffuse.r * f;
		out.g = diffuse.g * f;
		out.b = diffuse.b * f;
	}
};

/**
Its a collection of pointlights
*/
class LightCol
{
public:
	std::vector<PointLight *> lights;
	int count;

	LightCol::LightCol()
	{
		count = 0;
	}

	void addLight(PointLight& light)
	{
		lights.push_back(&light);
		count++;
	}
	/**
	Fill array by copying and compacting point lights from this collecton.
	*/
	void fillCompactArray(PointLightCompact *out)
	{
		PointLight *l;
		PointLightCompact *lC;
		std::vector<PointLight *>::iterator itL = lights.begin();
		int i = 0;
		for(; itL != lights.end(); itL++)
		{
			l = *itL;
			lC = out + i;
			
			lC->pos[0] = l->pos.x;
			lC->pos[1] = l->pos.y;
			lC->pos[2] = l->pos.z;
			
			lC->col[0] = l->diffuse.r;
			lC->col[1] = l->diffuse.g;
			lC->col[2] = l->diffuse.b;
			
			lC->intensity = l->intensity;
			lC->a0 = l->a0;
			lC->a1 = l->a1;
			lC->a2 = l->a2;

			i++;
		}
	}
};

LightCol GLOBAL_BUFFER_LIGHT;

#endif