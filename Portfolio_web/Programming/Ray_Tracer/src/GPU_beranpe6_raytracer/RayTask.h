
//----------------------------------------------------------------------------------------
/**
 * \file       RayTask.h
 * \author     Petr Beránek
 * \date       2016/01/04
 * \brief      File contains RayTask class.
 *
 * RayTasks represents rays that will be traced (CPU rays). Also contains struct for compact rays (RayCompact).
 *
*/
//----------------------------------------------------------------------------------------


#ifndef RAY
#define RAY

#define MAX_DEPTH 15

#define RAY_PRIMARY 0
#define RAY_SHADOW 1
#define RAY_REFRACTION 2
#define RAY_REFLECTION 3

#define EPSILON 0.000001
#define STEP_SIZE 0.001

#include <limits>

#include "Vec3.h"
#include "Col.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Bitmap.h"
#include "Material.h"
#include "RayBuffer.h"
#include "PointLight.h"
#include "KDTree.h"

#define MAX_DIST FLT_MAX



//Sphere testSphere;

struct RayCompact
{
	float start[3];
	float end[3];
	float color[3];
	float contribution[3];

	float length;

	int depth;
	int offset;
	int orientation;
	int type;
};
/**
It is just ray and some parameters allowing it to be executed with only information what KdTree should be used 
*/
class RayTask
{
public:
	/// depth of ray
	int depth;
	/// origin of ray
	Vec3 start;
	/// end point for ray, yes, direction would be better, but no time to rewtrite it all
	Vec3 end;
	/// offset in color buffer where ray is supposed to store color
	int pixelOffset;
	/// max length of ray
	float length;

	/// color of ray
	Col rayColor;
	/// contribution to final pixel color
	Col colorContribution;
	/// what normals should be used (1 for standard, -1 for reversed(used for ray refracted on sphere that needs to hit backface)) 
	int normalOrientation;

	/// what type of ray it is (primary, shadow, reflected, refracted)
	int rayType;
	
	/// bitmap where ray should store its final color
	Bitmap *bitmap;	
	/// buffer where new rays should be stored
	RayBuffer *buffer;
	/// collection of lights in scene
	LightCol *lightCol;
	
	
	/** 
	Ray constructor

	/param[in] s ray origin
	/param[in] e some point on line in rays direction
	/param[in] bit bitmap to store output color
	/param[in] offset where to store finl color to bitmap
	/param[in] base basic color of ray(in case of shadow rays it is color that will be stored)
	/param[in] contribution how much ray will contribute to color of pixel
	/param[in] b ray buffer
	/param[in] l collection of lights to use
	/param[in] d max length of ray
	/param[in] type type of ray
	*/
	RayTask::RayTask(Vec3 s, Vec3 e, Bitmap& bit, int offset, Col& base, Col& contribution, RayBuffer& b, LightCol& l, int d, int type, float len, int normalOri)
	{
		start = Vec3(s);
		end = Vec3(e);
		bitmap = &bit;
		pixelOffset = offset;
		rayColor = Col(base);
		colorContribution = Col(contribution);
		rayType = type;
		buffer = &b;
		lightCol = &l;
		depth = d;
		length = len;
		normalOrientation = normalOri;
	}
	
	float clip(float f)
{
	if(f > 1.0f){return 1.0f;}
	if(f < 0.0f){return 0.0f;}
	return f;
}

	void getSpecular(PointLight& light, Material& m,  Vec3& vDir, Vec3& vRay, Vec3& normal, Col& out, float dist)
	{
		Vec3 reflected;

		Vec3::reflect(vDir, normal, reflected);
		float d = reflected.dot(vRay);
			
		if (d < 0.0f) { return; }

		d = pow(d, m.shine);
		clip(d);

		light.giveColor(out, dist);
		out.mulLoc(d);
		out.multiply(m.kS);
	}
	/**
	Trace this ray using tree. Use basic way to traverse tree(always search for leaf from root).
	For commented code go to _device_ void executeBasic(...) in Source.cu it is nearly identical(it uses diferent structures for computing, but flow is the same)

	/param[in] tree KD tree with objects to collide
	*/
	void executeBasic(KDTree& tree)
	{
		std::vector<Collidable *>::iterator it;
		std::vector<Collidable *> *vec;
		Collidable *c;
		KDTree *actual;
		int type;
		bool collided = false;
		bool collidedNow;
		
		bool refractionReverse = false;
		bool refractionReverseNow ;
		
		Vec3 pointIn;
		Vec3 pointOut;
		Vec3 dir;

		Vec3 dist;

		Vec3 impact;
		Vec3 normal;
		float len = length;
		float boxBoundLength;
		Material *mat;

		int found = -1; 

		Col colorMod;
		float transp;


		dir.set(end);
		dir.subLoc(start);
		dir.normalizeLoc();


		pointIn.set(start);

		if(depth <= MAX_DEPTH)
		{
			if(!tree.isPointInTree(pointIn))
			{
				found = tree.findEnterPoint(start, dir, pointIn);
				pointIn.addMultipliedLoc(dir, STEP_SIZE);
				if(found == -1){
					pointIn.set(start);
				}
			}

			while(tree.isPointInTree(pointIn))
			{
				DEBUG_DEPTH = 0;
				actual = tree.findNodeByPoint(pointIn);
				
				found = actual->findExitPoint(start, dir, pointOut);
				pointOut.addMultipliedLoc(dir, STEP_SIZE);

				dist.set(pointOut);
				dist.subLoc(start);
				boxBoundLength = dist.length();
				
				vec = actual->items;
				it = vec->begin();
				Sphere *s;
				Triangle *t;
				float tempL;
				for(;it < vec->end();it++)
				{
					c = (*it);
					type = c->collidableType();
					switch(type)
					{
					case COLLID_SPHERE:
						s = dynamic_cast<Sphere *> (c);
						collidedNow = this->collideSphere(*s, impact, normal, len, boxBoundLength);
						refractionReverseNow = true;
						break;
					case COLLID_TRIANGLE:
						t = dynamic_cast<Triangle *> (c);
						collidedNow = this->collideTriangle(*t, impact, normal, len, boxBoundLength);
						refractionReverseNow = false;
						break;
					case COLLID_POINT:
						break;
					default:
						break;
					}

					if(collidedNow)
					{
						mat = Material::getMaterial(c->matIndex);
						if(rayType == RAY_SHADOW && mat->transparency < 1.0f)
						{
							transp = mat->transparency;
							colorMod = Col(mat->diff);
							colorMod.multiply(mat->kD);
							colorMod.mulLoc(transp);
							colorMod.addMultipliedLoc(Col(COL_WHITE),1.0f-transp);
							rayColor.multiply(colorMod);
						}else{
							collided = true;
							length = len;
							refractionReverse = refractionReverseNow;
						}

					}
				}

				if(collided) {break;}
				if(found != -1)
				{
					pointIn.set(pointOut);
				}else{
					break;
				}
			}
				
		}	
		

		if(!collided)
		{
			Col colorFinal = Col(COL_WHITE);
			Col::multiplySet(rayColor, colorContribution, colorFinal);

			bitmap->pixels[pixelOffset].x = (unsigned char)std::min(255.0f,colorFinal.r * 255 + bitmap->pixels[pixelOffset].x );
			bitmap->pixels[pixelOffset].y = (unsigned char)std::min(255.0f,colorFinal.g * 255 + bitmap->pixels[pixelOffset].y );
			bitmap->pixels[pixelOffset].z = (unsigned char)std::min(255.0f,colorFinal.b * 255 + bitmap->pixels[pixelOffset].z );
			bitmap->pixels[pixelOffset].w = 255;
			return;
		}

		if(rayType == RAY_SHADOW){return;}

		if(depth < MAX_DEPTH)
			{
			std::vector<PointLight *>::iterator lIt;
			PointLight *l;
			
			RayTask *t;
			Vec3 pStart = Vec3();
			Vec3 pEnd = Vec3();
			Vec3 vDir = Vec3();
			Vec3 vRay = Vec3();
			Vec3 vRef = Vec3();
			Col shadowContrib = Col(COL_BLACK);
			Col specular = Col(COL_BLACK);
			Col colContrib = Col(COL_WHITE);
			float rayLength;
			float d;
			
			vDir.set(end);
			vDir.subLoc(start);
			vDir.normalizeLoc();
			//shadow rays
			for(lIt = lightCol->lights.begin() ; lIt != lightCol->lights.end(); ++lIt)
			{
				l = (*lIt);
				
				specular = Col(COL_BLACK);
				vRay.set(l->pos);
				vRay.subLoc(impact);
				rayLength = vRay.length();
				vRay.normalizeLoc();
				pEnd.set(l->pos);
				pStart.set(impact);
				pStart.addMultipliedLoc(vRay, 0.01f);
				
				l->giveColor(shadowContrib, rayLength);
				d = clip(normal.dot(vRay));
				
				shadowContrib.multiply(mat->diff);
				getSpecular(*l, *mat, vDir, vRay, normal, specular, rayLength);
				colContrib.setCol(colorContribution);
				colContrib.multiply(mat->kD);
				colContrib.mulLoc(d);
				colContrib.mulLoc(mat->transparency);
				shadowContrib.addLoc(specular);
				t = new RayTask(pStart, pEnd, *bitmap, pixelOffset, shadowContrib, colContrib, *buffer, *lightCol, depth, RAY_SHADOW, rayLength, 1);
				t->executeBasic(tree);
				delete t;
			}
			
			bool TIR = false;
			Col refrContrib = Col();
			//refracted rays
			if(mat->transparency < 1)
				{
					float dot = vDir.dot(normal);
					float ref;
					Vec3 tmpNorm = Vec3(normal);
					
					if(dot < 0)
					{
						ref = 1/mat->refraction;
					}else{
						ref = mat->refraction;
						dot = -dot;
						tmpNorm.mulLoc(-1.0f);
					}
					
					float s2 = (ref*ref)*(1-(dot*dot));
					float sqr = 1 - s2;
					
					
					if(sqr < 0)
					{
						TIR = true;
					}else{
						sqr = -ref * dot - sqrt(sqr);
						Vec3 newDir = Vec3();
						newDir.addMultipliedLoc(vDir, ref);
						newDir.addMultipliedLoc(tmpNorm, sqr);
						
						
						pStart = Vec3(impact);
						pStart.addMultipliedLoc(newDir,0.01f);
						pEnd = Vec3(pStart);
						pEnd.addLoc(newDir);
						
						Col inside = Col(COL_WHITE);
						
						refrContrib = Col(colorContribution);
						refrContrib.multiply(inside);
						
						int rev = (refractionReverse) ? -1 : 1;
						t = new RayTask(pStart, pEnd, *bitmap, pixelOffset, Col(COL_DEFAULT), refrContrib, *buffer, *lightCol, depth + 1, RAY_PRIMARY, MAX_DIST, rev * normalOrientation);
						buffer->addRayTask(*t);
					}
				}
			
			//reflected rays
			if(mat->reflection > 0)
			{
				vRef = Vec3();
				Vec3::reflect(dir, normal, vRef);
				
				pStart.set(impact);
				pStart.addMultipliedLoc(vRef,STEP_SIZE);
				pEnd.set(pStart);
				pEnd.addLoc(vRef);
				//printf("IMP %f %f %f \n",impact.x,impact.y,impact.z);
				//printf("REV %f %f %f   %f %f %f \n",pStart.x,pStart.y,pStart.z,pEnd.x,pEnd.y,pEnd.z);
				
				Col reflectionContrib = Col(colorContribution);
				reflectionContrib.multiply(mat->kS);
				
				if(TIR){
					reflectionContrib.mulLoc(mat->reflection);
				}else{
					reflectionContrib.mulLoc(mat->reflection * mat->transparency);
				}
				
				t = new RayTask(pStart, pEnd, *bitmap, pixelOffset, Col(COL_DEFAULT), reflectionContrib, *buffer, *lightCol, depth + 1, RAY_PRIMARY, MAX_DIST, 1);
				t->executeBasic(tree);
				delete t;
			}
		}

		
	}
	/**
	Trace this ray using tree. Use advanced way to traverse tree(use neighborhood nodes when exiting to start new search for leaf).
	For commented code go to _device_ void executeBasic(...) in Source.cu it is nearly identical(it uses diferent structures for computing, but flow is the same)

	/param[in] tree KD tree with objects to collide
	*/
	void executeNeighbor(KDTree& tree)
	{
		std::vector<Collidable *>::iterator it;
		std::vector<Collidable *> *vec;
		Collidable *c;
		KDTree *actual;
		KDTree *subTree;
		int type;
		bool collided = false;
		bool collidedNow;
		
		bool refractionReverse = false;
		bool refractionReverseNow ;
		
		Vec3 pointIn;
		Vec3 pointOut;
		Vec3 dir;

		Vec3 dist;

		Vec3 impact;
		Vec3 normal;
		float len = length;
		float boxBoundLength;
		Material *mat;

		int found = -1; 

		Col colorMod;
		float transp;


		dir.set(end);
		dir.subLoc(start);
		dir.normalizeLoc();

		subTree = &tree;
		pointIn.set(start);

		if(depth <= MAX_DEPTH)
		{
			if(!tree.isPointInTree(pointIn))
			{
				found = tree.findEnterPoint(start, dir, pointIn);
				pointIn.addMultipliedLoc(dir, STEP_SIZE);
				if(found == -1){
					pointIn.set(start);
				}
			}

			while(tree.isPointInTree(pointIn))
			{
				DEBUG_DEPTH = 0;
				actual = subTree->findNodeByPoint(pointIn);
				
				found = actual->findExitPoint(start, dir, pointOut);
				pointOut.addMultipliedLoc(dir, STEP_SIZE);

				dist.set(pointOut);
				dist.subLoc(start);
				boxBoundLength = dist.length();
				
				vec = actual->items;
				it = vec->begin();
				Sphere *s;
				Triangle *t;
				float tempL;
				for(;it < vec->end();it++)
				{
					c = (*it);
					type = c->collidableType();
					switch(type)
					{
					case COLLID_SPHERE:
						s = dynamic_cast<Sphere *> (c);
						collidedNow = this->collideSphere(*s, impact, normal, len, boxBoundLength);
						refractionReverseNow = true;
						break;
					case COLLID_TRIANGLE:
						t = dynamic_cast<Triangle *> (c);
						collidedNow = this->collideTriangle(*t, impact, normal, len, boxBoundLength);
						refractionReverseNow = false;
						break;
					case COLLID_POINT:
						break;
					default:
						break;
					}

					if(collidedNow)
					{
						mat = Material::getMaterial(c->matIndex);
						if(rayType == RAY_SHADOW && mat->transparency < 1.0f)
						{
							transp = mat->transparency;
							colorMod = Col(mat->diff);
							colorMod.multiply(mat->kD);
							colorMod.mulLoc(transp);
							colorMod.addMultipliedLoc(Col(COL_WHITE),1.0f-transp);
							rayColor.multiply(colorMod);
						}else{
							collided = true;
							length = len;
							refractionReverse = refractionReverseNow;
						}

					}
				}

				if(collided) {break;}
				if(found != -1)
				{
					subTree = actual->neighbors[found];
					if(subTree == NULL){break;}
					pointIn.set(pointOut);
				}else{
					break;
				}
			}
				
		}	

		if(!collided)
		{
			Col colorFinal = Col(COL_WHITE);
			Col::multiplySet(rayColor, colorContribution, colorFinal);

			bitmap->pixels[pixelOffset].x = (unsigned char)std::min(255.0f,colorFinal.r * 255 + bitmap->pixels[pixelOffset].x );
			bitmap->pixels[pixelOffset].y = (unsigned char)std::min(255.0f,colorFinal.g * 255 + bitmap->pixels[pixelOffset].y );
			bitmap->pixels[pixelOffset].z = (unsigned char)std::min(255.0f,colorFinal.b * 255 + bitmap->pixels[pixelOffset].z );
			bitmap->pixels[pixelOffset].w = 255;
			return;
		}

		if(rayType == RAY_SHADOW){return;}

		if(depth < MAX_DEPTH)
			{
			std::vector<PointLight *>::iterator lIt;
			PointLight *l;
			
			RayTask *t;
			Vec3 pStart = Vec3();
			Vec3 pEnd = Vec3();
			Vec3 vDir = Vec3();
			Vec3 vRay = Vec3();
			Vec3 vRef = Vec3();
			Col shadowContrib = Col(COL_BLACK);
			Col specular = Col(COL_BLACK);
			Col colContrib = Col(COL_WHITE);
			float rayLength;
			float d;
			
			vDir.set(end);
			vDir.subLoc(start);
			vDir.normalizeLoc();
			//shadow rays
			for(lIt = lightCol->lights.begin() ; lIt != lightCol->lights.end(); ++lIt)
			{
				l = (*lIt);
				
				specular = Col(COL_BLACK);
				vRay.set(l->pos);
				vRay.subLoc(impact);
				rayLength = vRay.length();
				vRay.normalizeLoc();
				pEnd.set(l->pos);
				pStart.set(impact);
				pStart.addMultipliedLoc(vRay, 0.01f);
				
				l->giveColor(shadowContrib, rayLength);
				d = clip(normal.dot(vRay));
				
				shadowContrib.multiply(mat->diff);
				getSpecular(*l, *mat, vDir, vRay, normal, specular, rayLength);
				colContrib.setCol(colorContribution);
				colContrib.multiply(mat->kD);
				colContrib.mulLoc(d);
				colContrib.mulLoc(mat->transparency);
				shadowContrib.addLoc(specular);
				t = new RayTask(pStart, pEnd, *bitmap, pixelOffset, shadowContrib, colContrib, *buffer, *lightCol, depth, RAY_SHADOW, rayLength, 1);
				t->executeNeighbor(tree);
				delete t;
			}
			
			bool TIR = false;
			Col refrContrib = Col();
			//refracted rays
			if(mat->transparency < 1)
				{
					float dot = vDir.dot(normal);
					float ref;
					Vec3 tmpNorm = Vec3(normal);
					
					if(dot < 0)
					{
						ref = 1/mat->refraction;
					}else{
						ref = mat->refraction;
						dot = -dot;
						tmpNorm.mulLoc(-1.0f);
					}
					
					float s2 = (ref*ref)*(1-(dot*dot));
					float sqr = 1 - s2;
					
					
					if(sqr < 0)
					{
						TIR = true;
					}else{
						sqr = -ref * dot - sqrt(sqr);
						Vec3 newDir = Vec3();
						newDir.addMultipliedLoc(vDir, ref);
						newDir.addMultipliedLoc(tmpNorm, sqr);
						
						
						pStart = Vec3(impact);
						pStart.addMultipliedLoc(newDir,0.01f);
						pEnd = Vec3(pStart);
						pEnd.addLoc(newDir);
						
						Col inside = Col(COL_WHITE);
						
						refrContrib = Col(colorContribution);
						refrContrib.multiply(inside);
						
						int rev = (refractionReverse) ? -1 : 1;
						t = new RayTask(pStart, pEnd, *bitmap, pixelOffset, Col(COL_DEFAULT), refrContrib, *buffer, *lightCol, depth + 1, RAY_PRIMARY, MAX_DIST, rev * normalOrientation);
						buffer->addRayTask(*t);
					}
				}
			
			//reflected rays
			if(mat->reflection > 0)
			{
				vRef = Vec3();
				Vec3::reflect(dir, normal, vRef);
				
				pStart.set(impact);
				pStart.addMultipliedLoc(vRef,STEP_SIZE);
				pEnd.set(pStart);
				pEnd.addLoc(vRef);
				
				Col reflectionContrib = Col(colorContribution);
				reflectionContrib.multiply(mat->kS);
				
				if(TIR){
					reflectionContrib.mulLoc(mat->reflection);
				}else{
					reflectionContrib.mulLoc(mat->reflection * mat->transparency);
				}
				
				t = new RayTask(pStart, pEnd, *bitmap, pixelOffset, Col(COL_DEFAULT), reflectionContrib, *buffer, *lightCol, depth + 1, RAY_PRIMARY, MAX_DIST, 1);
				t->executeNeighbor(tree);
				delete t;
			}
		}

		
	}
	/**
	Collide sphere with ray

	/param[in] ray ray to collide
	/param[in] spheres buffer of spheres
	/param[in] index index of sphere from spheres
	/param[in] impact output point where collision happened
	/param[in] normal output vector, normal at impact point
	/param[in] len max distance where collision can happen( max length of ray )
	/param[in] bound max distance where collision can happen( end of KD tree box )
	*/
	bool collideSphere(Sphere& sph, Vec3& impact, Vec3& normal, float& len, float bound)
	{
		float b,c,D;
		Vec3 temp;

		Vec3 dir = Vec3(end);
		dir.subLoc(start);
		dir.normalizeLoc();

		temp = Vec3(start);
		temp.subLoc(sph.loc);

		b = 2 * dir.dot(temp);

		c = temp.dot(temp) - (sph.radius*sph.radius);

		D = b*b - 4*c;

		if(D > 0)
		{
			// 2 solutions
			float Ds, t1, t2;
			Vec3 I1, I2;
			Ds = sqrt(D);

			t1 = 0.5f * (-b - Ds);
			t2 = 0.5f * (-b + Ds);
	
			Vec3 V1 = Vec3(dir);
			V1.mulLoc(t1);

			I1 = Vec3(V1);
			I1.addLoc(start);

			Vec3 V2 = Vec3(dir);
			V2.mulLoc(t2);

			I2 = Vec3(V2);
			I2.addLoc(start);	

			//hit location
			Vec3 *finalI = NULL;
			//ray direction
			Vec3 *finalV = NULL;
			float tF = len;
			
			Vec3 *n;
			if(t1 > 0 && t1 < tF)
			{
				n = new Vec3(I1);
				n->subLoc(sph.loc);
				n->normalizeLoc();

				if(normalOrientation * dir.dot(*n) < 0)
				{
					finalI = &I1;
					finalV = &V1;
					tF = t1;
				}
			}
			if(t2 > 0 && t2 < tF)
			{
				n = new Vec3(I2);
				n->subLoc(sph.loc);
				n->normalizeLoc();

				if(normalOrientation * dir.dot(*n) < 0)
				{
					finalI = &I2;
					finalV = &V2;
					tF = t2;
				}
			}
			if(finalI == NULL){return false;}
			
			Vec3 norm = Vec3(*finalI);
			norm.subLoc(sph.loc);
			norm.normalizeLoc();

			//oudside KDBox
			if(tF > bound){return false;}

			len = tF;
			impact.set(*finalI);
			normal.set(norm);

			return true;
		}else if(D < 0){
			//no solution
			return false;
		}else{
			//one solution
			return false;
		}
		return false;
	}
	/**
	Collide ray with triangle
	
	/param[in] ray ray to collide
	/param[in] triangles buffer of triangles
	/param[in] index index of triangle from triangles
	/param[in] impact output point where collision happened
	/param[in] normal output vector, normal at impact point
	/param[in] len max distance where collision can happen( max length of ray )
	/param[in] bound max distance where collision can happen( end of KD tree box )
	*/
	bool collideTriangle(Triangle& tr, Vec3& impact, Vec3& normal, float& len, float bound)
	{
		Vec3 e1, e2, pvec, tvec, qvec, dir;
		
		e1 = Vec3(tr.v1);
		e1.subLoc(tr.v0);
		e2 = Vec3(tr.v2);
		e2.subLoc(tr.v0);

		pvec = Vec3();
		dir = Vec3(end);
		dir.subLoc(start);
		dir.normalizeLoc();

		Vec3::cross(dir, e2, pvec);

		float det = e1.dot(pvec);
		if(det > -EPSILON && det < EPSILON)
		{
			return false;
		}

		tvec = Vec3(start);
		tvec.subLoc(tr.v0);

		float u = tvec.dot(pvec);
		if(u < 0.0f || u > det)
		{
			return false;
		}
		qvec = Vec3();
		Vec3::cross(tvec, e1, qvec);

		float v = dir.dot(qvec);
		
		if(v < 0.0f || u + v > det)
		{
			return false;
		}

		float t = e2.dot(qvec);
		det = 1/det;
		u *= det;
		v *= det;
		t *= det;
		
		if(t < 0.0f || t > len)
		{
			return false;
		}

		
		Vec3 norm = Vec3();
		Vec3 imp = Vec3();
		Vec3::triLerp(imp, u, v, tr.v0, tr.v1, tr.v2);
		Vec3::triLerp(norm, u, v, tr.n0, tr.n1, tr.n2);

		
		//oudside KDBox
		if(t > bound){return false;}

		len = t;
		impact.set(imp);
		normal.set(norm);

		return true;
	}
	
};

#endif
