
#ifndef RAY
#define RAY

#define MAX_DEPTH 4

#define RAY_PRIMARY 0
//#define RAY_SHADOW 1
//#define RAY_REFRACTION 2

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

#define MAX_DIST std::numeric_limits<float>::max()

//Sphere testSphere;

class RayTask
{
public:
	int depth;
	Vec3 *start;
	Vec3 *end;
	int pixelOffset;
	float length;

	Col *rayColor;
	Col *colorContribution;
	/*
	bool intersected;
	Vec3 *impact;
	Vec3 *impactNormal;
	Vec3 *impactVector;
	Col *impactColContrib;
	float impactRefraction;
	float reflection;
	float transparency;
	*/
	int rayType;
	Bitmap *bitmap;
	
	RayBuffer *buffer;
	LightCol *lightCol;
	
	

	RayTask::RayTask(Vec3 s, Vec3 e, Bitmap *bit, int offset, Col& base, Col& contribution, RayBuffer& b, LightCol& l, int d, int type, float len)
	{
		start = new Vec3(s);
		end = new Vec3(e);
		bitmap = bit;
		pixelOffset = offset;
		rayColor = new Col(COL_DEFAULT);
		rayColor->setCol(base);
		colorContribution = &contribution;
		//impactColContrib = new Col(COL_WHITE);
		rayType = type;
		buffer = &b;
		lightCol = &l;
		depth = d;
		//intersected = false;
		//transparency = 1;
		length = len;
	}

	bool isClosest(Vec3 point)
	{
		Vec3 temp = point.sub(*start);
		float dist = temp.length();

		if(dist < length)
		{
			return true;
		}else{
			return false;
		}
	}
	bool isCloser(float f)
	{
		if(f < length)
		{
			return true;
		}else{
			return false;
		}
	}
	
	void finishShadowTask()
	{
			Col colorFinal = Col(COL_WHITE);
			Col::multiplySet(*rayColor, *colorContribution, colorFinal);

			bitmap->pixels[pixelOffset].x = (unsigned char)std::min(255.0f,colorFinal.r * 255 + bitmap->pixels[pixelOffset].x );
			bitmap->pixels[pixelOffset].y = (unsigned char)std::min(255.0f,colorFinal.g * 255 + bitmap->pixels[pixelOffset].y );
			bitmap->pixels[pixelOffset].z = (unsigned char)std::min(255.0f,colorFinal.b * 255 + bitmap->pixels[pixelOffset].z );
			bitmap->pixels[pixelOffset].w = 255;
	}
	/*
	void finishPrimaryTask()
	{
		//printf("finishing primary\n");
		if(!intersected || depth == MAX_DEPTH)
		{
			
			Col colorFinal = Col(COL_WHITE);
			Col::multiplySet(*rayColor, *colorContribution, colorFinal);

			bitmap->pixels[pixelOffset].x = (unsigned char)std::min(255.0f,colorFinal.r * 255 + bitmap->pixels[pixelOffset].x );
			bitmap->pixels[pixelOffset].y = (unsigned char)std::min(255.0f,colorFinal.g * 255 + bitmap->pixels[pixelOffset].y );
			bitmap->pixels[pixelOffset].z = (unsigned char)std::min(255.0f,colorFinal.b * 255 + bitmap->pixels[pixelOffset].z );
			bitmap->pixels[pixelOffset].w = 255;
			
			
		}else{

			RayTask *t;
			std::vector<PointLight *>::iterator it;
			Vec3 *pStart;
			Vec3 *pEnd;
			Vec3 *vDir;
			float dist;
			Col *newColContrib;
			Col *newCol;
			PointLight *l;

			float lightModification;
			float contribModification;

			//shadow
			for(it = lightCol->lights.begin() ; it != lightCol->lights.end(); ++it)
			{
				l = (*it);
				pStart = new Vec3(*impact);
				vDir = new Vec3(l->pos);
				vDir->subLoc(*pStart);
				dist = vDir->length();
				vDir->normalizeLoc();

				pStart->addMultipliedLoc(*vDir, 0.01f);

				lightModification = (l->intensity) / (l->fallOff * dist * dist);
				newCol = new Col(*(l->diffuse));
				newCol->mulLoc(lightModification);

				contribModification = std::max(0.0f, impactNormal->dot(*vDir));
				
				newColContrib = new Col(*colorContribution);
				newColContrib->multiply(*impactColContrib);
				newColContrib->multiply(*impactColContrib);
				//newColContrib->multiply();
				newColContrib->mulLoc(contribModification * transparency); //add transparency here in next iteration

				//if(transparency < 1){continue;}

				t = new RayTask(*pStart, l->pos, bitmap, pixelOffset, *newCol, *newColContrib, *buffer, *lightCol, depth + 1, RAY_PRIMARY, dist);
				buffer->addRayTask(*t);
			}
			bool TIR = false;
			//refraction
			if(transparency < 1)
			{
				float ref;
				Vec3 *norm = new Vec3(*impactNormal);
				float dot = impactVector->dot(*norm);

				if(dot < 0)
				{
					ref = 1/impactRefraction;
				}else{
					ref = impactRefraction;
					dot = -dot;
					norm->mul(-1.0f);
				}

				float s2 = (ref*ref)*(1-(dot*dot));
				float sqr = 1 - s2;
				if(sqr < 0)
				{
					TIR = true;
				}else{
					sqr = -ref * dot - sqrt(sqr);
					
					//vDir = new Vec3(*impactVector);
					vDir = new Vec3();
					vDir->addMultipliedLoc(*impactVector, ref);
					vDir->addMultipliedLoc(*norm, sqr);

					pStart = new Vec3(*impact);
					pStart->addMultipliedLoc(*vDir,0.01f);
					pEnd = new Vec3(*pStart);
					pEnd->addLoc(*vDir);

					
					//Col *inside = new Col(COL_BLACK);
					//inside->addMultipliedLoc(Col(COL_WHITE),1-transparency);
					//inside->addMultipliedLoc(*impactColContrib,transparency);
					
					
					Col *inside = new Col(COL_WHITE);

					newColContrib = new Col(*colorContribution);
					newColContrib->multiply(*inside);
					//t = new RayTask(*pStart, *pEnd, bitmap, pixelOffset, *rayColor, *newColContrib, *buffer, *lightCol, depth , RAY_PRIMARY, MAX_DIST);
					t = new RayTask(*pStart, *pEnd, bitmap, pixelOffset, Col(COL_DEFAULT), *newColContrib, *buffer, *lightCol, depth + 1, RAY_PRIMARY, MAX_DIST);
					buffer->addRayTask(*t);
				}
			}
			
			//reflection
			if(reflection > 0)
			{
				Vec3 *reflected = new Vec3();
				Vec3::reflect(*impactVector, *impactNormal, *reflected);
				
				pStart = new Vec3(*impact);
				vDir = new Vec3(*reflected);
				pStart->addMultipliedLoc(*vDir,0.01f);
				pEnd = new Vec3(*pStart);
				pEnd->addLoc(*vDir);

				newColContrib = new Col(*colorContribution);
				if(TIR){
					newColContrib->mulLoc(reflection);
				}else{
					newColContrib->mulLoc(reflection * transparency);
				}
				
				t = new RayTask(*pStart, *pEnd, bitmap, pixelOffset,Col(COL_DEFAULT), *newColContrib, *buffer, *lightCol, depth + 1, RAY_PRIMARY, MAX_DIST);
				buffer->addRayTask(*t);
			}
		}
	}
	*/
	void finishTask()
	{
			//finishPrimaryTask();
	}
	
	void execute(KDTree& tree)
	{


		std::vector<Collidable *>::iterator it;
		std::vector<Collidable *> *vec;
		Collidable *c;
		KDTree *actual;
		int type;
		bool collided = false;
		bool collidedNow;

		Vec3 point;
		Vec3 dir;

		Vec3 impact;
		Vec3 normal;
		float len = length;
		Material *mat;

		dir.set(*end);
		dir.subLoc(*start);


		point.set(*start);

		if(depth < MAX_DEPTH)
		{
			if(!tree.isPointInTree(point))
			{
				tree.findEnterPoint(*start, dir, point);
				point.addMultipliedLoc(dir, STEP_SIZE);
			}

			while(tree.isPointInTree(point))
			{
				actual = tree.findNodeByPoint(point);

				
				vec = actual->items;
				//printf("objects %d \n", vec->size());
				it = vec->begin();
				Sphere *s;
				Triangle *t;
				for(;it < vec->end();it++)
				{
					c = (*it);
					type = c->collidableType();
					switch(type)
					{
					case COLLID_SPHERE:
						//printf("sph\n");
						s = dynamic_cast<Sphere *> (c);
						collidedNow = this->collideSphere(*s, impact, normal, len);
						break;
					case COLLID_TRIANGLE:
						//printf("tri\n");
						t = dynamic_cast<Triangle *> (c);
						collidedNow = this->collideTriangle(*t, impact, normal, len);
						break;
					case COLLID_POINT:
						//printf("point\n");
						break;
					default:
						//printf("none\n");
						break;
					}

					if(collidedNow)
					{
						length = len;
						mat = &c->mat;
						collided = true;
					}
				}

				// no need to search through further boxes -> break;
				if(collided) {break;}
					
				actual->findExitPoint(*start, dir, point);
				point.addMultipliedLoc(dir, STEP_SIZE);
			}
				
		}	

		

		if(!collided)
		{
			Col colorFinal = Col(COL_WHITE);
			Col::multiplySet(*rayColor, *colorContribution, colorFinal);

			bitmap->pixels[pixelOffset].x = (unsigned char)std::min(255.0f,colorFinal.r * 255 + bitmap->pixels[pixelOffset].x );
			bitmap->pixels[pixelOffset].y = (unsigned char)std::min(255.0f,colorFinal.g * 255 + bitmap->pixels[pixelOffset].y );
			bitmap->pixels[pixelOffset].z = (unsigned char)std::min(255.0f,colorFinal.b * 255 + bitmap->pixels[pixelOffset].z );
			bitmap->pixels[pixelOffset].w = 255;
			return;
		}else{
			//printf("hit\n");
		}

		/*
		RayTask *t;
		Vec3 *pStart;
		Vec3 *pEnd;
		Vec3 *vDir;
		float dist;
		Col *newColContrib;
		Col *newCol;

		float lightModification;
		float contribModification;

		*/
		std::vector<PointLight *>::iterator lIt;
		PointLight *l;
		
		RayTask *t;
		Vec3 pStart = Vec3();
		Vec3 pEnd = Vec3();
		Vec3 vDir = Vec3();
		Col shadowContrib = Col(COL_BLACK);
		float rayLength;
		float d;

		/*
		bitmap->pixels[pixelOffset].x =  (unsigned char)std::min(255.0f,255*mat->diff->r);
		bitmap->pixels[pixelOffset].y = (unsigned char)std::min(255.0f,255*mat->diff->g);
		bitmap->pixels[pixelOffset].z = (unsigned char)std::min(255.0f,255*mat->diff->b);
		bitmap->pixels[pixelOffset].w = 255;
		*/
		
		for(lIt = lightCol->lights.begin() ; lIt != lightCol->lights.end(); ++lIt)
		{
			l = (*lIt);

			vDir.set(l->pos);
			vDir.subLoc(impact);
			rayLength = vDir.length();
			vDir.normalizeLoc();
			pEnd.set(l->pos);
			pStart.set(impact);
			pStart.addMultipliedLoc(vDir, 0.01f);

			l->giveColor(shadowContrib, rayLength);
			d = std::min(0.0f, normal.dot(vDir));

			shadowContrib.multiply(*mat->diff);
			//shadowContrib.mulLoc(d);
			//shadowContrib.multiply(*mat->kD);
			//shadowContrib.multiply(*colorContribution);

			t = new RayTask(pStart, pEnd, bitmap, pixelOffset, shadowContrib, *colorContribution, *buffer, *lightCol, MAX_DEPTH, RAY_PRIMARY, rayLength);
			buffer->addRayTask(*t);
			/*

			lightModification = (l->intensity) / (l->fallOff * dist * dist);
			newCol = new Col(*(l->diffuse));
			newCol->mulLoc(lightModification);

			contribModification = std::max(0.0f, impactNormal->dot(*vDir));

			newColContrib = new Col(*colorContribution);
			newColContrib->multiply(*impactColContrib);
			newColContrib->multiply(*impactColContrib);
			//newColContrib->multiply();
			newColContrib->mulLoc(contribModification * transparency); //add transparency here in next iteration

			//if(transparency < 1){continue;}

			t = new RayTask(*pStart, l->pos, bitmap, pixelOffset, *newCol, *newColContrib, *buffer, *lightCol, depth + 1, RAY_PRIMARY, dist);
			buffer->addRayTask(*t);
			*/
		}



		
	}
	/*
	void executeOld(KDTree& tree)
	{
		std::vector<Collidable *> *vec;
		std::vector<Collidable *>::iterator it;
		Collidable *c;
		bool collided = false;
		int type;
		Vec3 *tmpPoint;
		KDTree *actualNode = new KDTree();

		Vec3 *dir = new Vec3(*end);
		dir->subLoc(*start);
		printf("init execution \n");
		if(tree.isPointInTree(*start))
		{
			printf("init in tree\n");
			tmpPoint = start;
		}else{
			printf("init not in tree\n");
			tmpPoint = new Vec3();
			tree.findEnterPoint(*start,*dir,*tmpPoint);
			tmpPoint->addMultipliedLoc(*dir, STEP_SIZE);
		}
		printf("cycle starts \n");
		int t = 0;
		while(t < 5)
		{
			t++;
			printf("entry point %f %f %f \n",tmpPoint->x,tmpPoint->y,tmpPoint->z);
			//ray left tree space
			if(!tree.isPointInTree(*tmpPoint)) {printf("outside \n");break;}
			tree.findNodeByPoint(*tmpPoint, actualNode);
			//printf("node dim %f %f %f %f %f %f \n",actualNode->xMin,actualNode->xMax,actualNode->yMin,actualNode->yMax,actualNode->zMin,actualNode->zMax);
			//printf("nd %d \n",actualNode);
			//printf("it %d \n",actualNode->items);
			vec = actualNode->items;
			// tree list routine
			it = vec->begin();
			for(;it < vec->end();it++)
			{
				c = (*it);
				type = c->collidableType();
				//printf("colliding with %f %f %f \n", c->xMin, c->yMin, c->zMin);
				switch(type)
				{
				case COLLID_SPHERE:
					collided |=  this->collideSphere(*(dynamic_cast<Sphere *> (c)));
					break;
				case COLLID_TRIANGLE:
					collided |=  this->collideTriangle(*(dynamic_cast<Triangle *> (c)));
					break;
				case COLLID_POINT:
					break;
				}
			}
			// no need to search through further boxes -> break;
			if(collided) {break;}
			
			tmpPoint = new Vec3();
			actualNode->findExitPoint(*start,*dir,*tmpPoint);
			tmpPoint->addMultipliedLoc(*dir, STEP_SIZE);
			printf("exitPoint %f %f %f \n",tmpPoint->x,tmpPoint->y,tmpPoint->z);
			//return;
		}
		//finish the ray
		finishTask();
	}
	*/
	/*
	void setIntersection(float dist, Vec3& normal, Vec3& hit, Vec3& impactVec, Col& clr, float refl, float tran, float refr)
	{
		if(dist < length)
		{
			length = dist;

			impact = new Vec3();
			impactNormal = new Vec3();
			impactVector = new Vec3();
			//if(rayType == RAY_SHADOW){
			//	rayColor = new Col(COL_BLACK);
			//}

			impact->addLoc(hit);
			impactNormal->addLoc(normal);
			impactVector->addLoc(impactVec);
			impactVector->normalizeLoc();
			impactColContrib = new Col(clr);
			impactRefraction = refr;

			//colorContribution->multiply(clr);
			reflection = refl;
			transparency = tran;

			intersected = true;
		}
	}
	*/
	bool collideSphere(Sphere& sph, Vec3& impact, Vec3& normal, float& len)
	{
		float b,c,D;
		Vec3 temp;

		Vec3 dir = end->sub(*start);
		dir.normalizeLoc();

		temp = start->sub(sph.loc);
		b = 2 * dir.dot(temp);

		c = temp.dot(temp) - (sph.radius*sph.radius);

		D = b*b - 4*c;
		if(D > 0)
		{
			//printf("---two--- hits\n");
			// 2 solutions
			float Ds, t1, t2;
			Vec3 I1, I2;
			Ds = sqrt(D);

			t1 = (-b + Ds)/(2);
			t2 = (-b - Ds)/(2);

			Vec3 V1 = dir.mul(t1);	
			I1 = V1.add(*start);		

			Vec3 V2 = dir.mul(t2);
			I2 = V2.add(*start);		

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

				if(dir.dot(*n) < 0)
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

				if(dir.dot(*n) < 0)
				{
					finalI = &I2;
					finalV = &V2;
					tF = t2;
				}
			}
			if(finalI == NULL){return false;}
			
			Vec3 norm = finalI->sub(sph.loc);
			norm.normalizeLoc();

			len = tF;
			impact.set(*finalI);
			normal.set(norm);

			return true;
		}else if(D < 0){
			//no solution
			//printf("one hit\n");
			return false;
		}else{
			//one solution
			//printf("no hit\n");
			return false;
		}
		return false;
	}

	bool collideTriangle(Triangle& tr, Vec3& impact, Vec3& normal, float& len)
	{
		Vec3 *e1, *e2, *pvec, *tvec, *qvec, *dir;
		
		e1 = new Vec3(*tr.v1);
		e1->subLoc(*tr.v0);
		e2 = new Vec3(*tr.v2);
		e2->subLoc(*tr.v0);

		pvec = new Vec3();
		dir = new Vec3(*end);
		dir->subLoc(*start);
		dir->normalizeLoc();

		Vec3::cross(*dir, *e2, *pvec);

		float det = e1->dot(*pvec);

		if(det > -EPSILON && det < EPSILON)
		{
			//printf("det %f \n",det);
			//printf("cull initial\n");
			return false;
		}

		tvec = new Vec3(*start);
		tvec->subLoc(*tr.v0);

		float u = tvec->dot(*pvec);
		//printf("det %f u %f \n",det, u);
		if(u < 0.0f || u > det)
		{
			//printf("cull second\n");
			return false;
		}
		//printf("passed second cull \n");
		qvec = new Vec3();
		Vec3::cross(*tvec, *e1, *qvec);

		float v = dir->dot(*qvec);

		if(v < 0.0f || u + v > det)
		{
			//printf("cull final\n");
			return false;
		}

		float t = e2->dot(*qvec);
		det = 1/det;
		u *= det;
		v *= det;
		t *= det;

		if(t < 0.0f || t > len)
		{
			//printf("yep, it was this bug\n");
			return false;
		}

		
		Vec3 norm = Vec3();
		Vec3 imp = Vec3();
		Vec3::triLerp(imp, u, v, *tr.v0, *tr.v1, *tr.v2);
		Vec3::triLerp(norm, u, v, *tr.n0, *tr.n1, *tr.n2);

		len = t;
		impact.set(imp);
		normal.set(norm);

		return true;

		/*
		float tran = tr.mat->transparency;
		Col *c = new Col(*(tr.mat->diff));
		c->mulLoc(tran);
		c->addMultipliedLoc(Col(COL_WHITE),1-tran);
		//norm = new Vec3(0.0f, 0.0f, 1.0f);

		//printf("n %f %f %f --- %f %f %f --- \n",norm->x,norm->y,norm->z,dir->x,dir->y,dir->z);
		setIntersection(t, *norm, *imp, *dir, *c, tr.mat->reflection, tr.mat->transparency, tr.mat->refraction);
		//printf("finished\n");
		return true;
		*/
	}
	
};

	

#endif
