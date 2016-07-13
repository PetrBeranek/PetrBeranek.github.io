

//----------------------------------------------------------------------------------------
/**
 * \file       Camera.h
 * \author     Petr Beránek
 * \date       2016/01/04
 * \brief      File contains class Camera used to represent camera in scene.
 *
 *
*/
//----------------------------------------------------------------------------------------


#ifndef CAM
#define CAM

#include <cmath>

#include "Vec3.h"
#include "Structures.h"
#include "Bitmap.h"
#include "RayTask.h"
#include "RayBuffer.h"

class PinholeCamera
{
public:
	/// position
	Vec3 vLoc;
	/// upvector
	Vec3 vUp;
	/// fwd vector
	Vec3 vFwd;
	/// right vector
	Vec3 vRight;
	/// distance of projection plane
	float focalDepth;
	/// size of projection plane
	float projectionPlaneWidth, projectionPlaneHeight;

	PinholeCamera::PinholeCamera()
	{
		vLoc = Vec3();
		vUp = Vec3(0.0f, 1.0f, 0.0f);
		vFwd = Vec3(0.0f, 0.0f, -1.0f);
		vRight = Vec3(1.0f, 0.0f, 0.0f);
		focalDepth = 1;
	}

	PinholeCamera::PinholeCamera(Vec3 origin, Vec3 target, Vec3 up, float f, float w, float h)
	{
		vLoc = origin;
		up = Vec3(0.0f, 1.0f, 0.0f);
		
		vFwd = Vec3();
		vFwd.addLoc(target);
		vFwd.subLoc(origin);
		vFwd.normalizeLoc();

		vRight = Vec3();
		Vec3::cross(vFwd, up, vRight);
		vRight.normalizeLoc();

		projectionPlaneHeight = h;
		projectionPlaneWidth = w;

		vUp = Vec3();
		Vec3::cross(vRight, vFwd, vUp);
		vUp.normalizeLoc();

		focalDepth = f;
	}
	/**
	Modify where camera is and where it is pointed at.
	*/
	void lookAt(Vec3 origin, Vec3 target, Vec3 up)
	{
		vLoc = origin;
		
		vFwd = Vec3();
		vFwd.addLoc(target);
		vFwd.subLoc(origin);
		vFwd.normalizeLoc();

		Vec3::cross(vFwd, up, vRight);
		vRight.normalizeLoc();

		Vec3::cross(vRight, vFwd, vUp);
		vUp.normalizeLoc();
	}
	/**
	Build rays for CPU
	*/
	void buildRays(Bitmap& bMap, RayBuffer& buffer, LightCol& lightCol)
	{
		int w = bMap.width;
		int h = bMap.height;
		Vec3 fOffset = Vec3(vFwd);
		fOffset.mulLoc(focalDepth);
		Vec3 cornerLU, left, up;
		float dW, dH;
		dW = projectionPlaneWidth/w;
		dH = projectionPlaneHeight/h;
		left = Vec3(vRight);
		left.mulLoc(projectionPlaneWidth/2);

		left.negativeLoc();
		up = Vec3(vUp);
		up.mulLoc(projectionPlaneHeight/2);

		cornerLU = Vec3(vLoc);
		cornerLU.addLoc(fOffset);

		cornerLU.addLoc(left);
		cornerLU.addLoc(up);
		
		left.negativeLoc();
		up.negativeLoc();
		left.normalizeLoc();
		up.normalizeLoc();

		Vec3 temp;
		RayTask *t;
		Col clr;
		int r;
		for(int rN = 0; rN < h ;rN++)
		{
			r = h-rN-1;
			temp = Vec3(cornerLU);
			temp.addMultipliedLoc(up, dH*rN);

			for(int c = 0; c < w ;c++)
			{
				temp.addMultipliedLoc(left,dW);
				clr = Col(1.0f,1.0f,1.0f);
				t = new RayTask(vLoc,temp,bMap, r*w+c, Col(COL_DEFAULT), clr, buffer, lightCol, 1, RAY_PRIMARY, MAX_DIST, 1);
				buffer.addRayTask(*t);
			}
		}
	}
	/**
	Build rays for GPU (just compacted CPU version)
	*/
	void buildRays(Bitmap& bMap, RayCompact *buffer, int size)
	{
		int w = bMap.width;
		int h = bMap.height;
		Vec3 fOffset = Vec3(vFwd);
		fOffset.mulLoc(focalDepth);
		Vec3 cornerLU, left, up;
		float dW, dH;
		dW = projectionPlaneWidth/w;
		dH = projectionPlaneHeight/h;
		left = Vec3(vRight);
		left.mulLoc(projectionPlaneWidth/2);
		left.negativeLoc();
		up = Vec3(vUp);
		up.mulLoc(projectionPlaneHeight/2);

		cornerLU = Vec3(vLoc);
		cornerLU.addLoc(fOffset);
		cornerLU.addLoc(left);
		cornerLU.addLoc(up);
		
		left.negativeLoc();
		up.negativeLoc();
		left.normalizeLoc();
		up.normalizeLoc();

		Vec3 temp;
		RayCompact *ray;
		Col clr;
		int r;
		int index = 0;
		for(int rN = 0; rN < h ;rN++)
		{
			r = h-rN-1;
			temp = Vec3(cornerLU);
			temp.addMultipliedLoc(up, dH*rN);
			for(int c = 0; c < w ;c++)
			{
				if(index >= size)
				{
					printf("RayBuffer is full");
					return;
				}
				ray = buffer+index;

				ray->start[0] = vLoc.x;
				ray->start[1] = vLoc.y;
				ray->start[2] = vLoc.z;

				temp.addMultipliedLoc(left, dW);
				
				ray->end[0] = temp.x;
				ray->end[1] = temp.y;
				ray->end[2] = temp.z;

				ray->offset = r*w+c;

				ray->color[0] = Col(COL_DEFAULT).r;
				ray->color[1] = Col(COL_DEFAULT).g;
				ray->color[2] = Col(COL_DEFAULT).b;
				
				ray->contribution[0] = 1.0f;
				ray->contribution[1] = 1.0f;
				ray->contribution[2] = 1.0f;

				ray->depth = 0;
				ray->length = MAX_DIST;
				ray->orientation = 1;
				ray->type = RAY_PRIMARY;

				index++;
			}
		}
	}

	/**
	Build just one ray through center of projection plane (used for DEBUG) CPU version
	*/
	void buildCentralRay(Bitmap& bMap, RayCompact *buffer, int size)
	{
		RayCompact *ray;
		ray = buffer;

		ray->start[0] = vLoc.x;
		ray->start[1] = vLoc.y;
		ray->start[2] = vLoc.z;

		ray->end[0] = vLoc.x + vFwd.x;
		ray->end[1] = vLoc.y + vFwd.y;
		ray->end[2] = vLoc.z + vFwd.z;

		ray->color[0] = Col(COL_DEFAULT).r;
		ray->color[1] = Col(COL_DEFAULT).g;
		ray->color[2] = Col(COL_DEFAULT).b;

		ray->contribution[0] = 1.0f;
		ray->contribution[1] = 1.0f;
		ray->contribution[2] = 1.0f;

		ray->depth = 0;
		ray->length = MAX_DIST;
		ray->orientation = 1;
		ray->type = RAY_PRIMARY;
	}
	
	/**
	Build just one ray through center of projection plane (used for DEBUG) GPU version
	*/
	void buildCentralRay(Bitmap& bMap, RayBuffer& buffer, LightCol& lightCol)
	{
		Vec3 temp = Vec3(vLoc);
		temp.addLoc(vFwd);
		RayTask *t;
		Col c = Col(1.0f,1.0f,1.0f);
		t = new RayTask(vLoc,temp,bMap, 0, Col(COL_DEFAULT), c, buffer, lightCol, 1, RAY_PRIMARY, MAX_DIST, 1);
		buffer.addRayTask(*t);
	}
};

#endif
