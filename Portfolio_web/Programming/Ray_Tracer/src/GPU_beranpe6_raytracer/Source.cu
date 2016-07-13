
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include <pthread.h>

#include <windows.h>

#include "Vec3.h"
#include "Bitmap.h"
#include "RayTask.h"
#include "Collidable.h"
#include "Sphere.h"
#include "Point.h"
#include "Triangle.h"
#include "Col.h"
#include "Camera.h"
#include "Structures.h"
#include "RayBuffer.h"
#include "PointLight.h"
#include "Material.h"
#include "KDTree.h"

#define NOMINMAX

///image resolution
#define IMAGE_WIDTH  500 
///image resolution
#define IMAGE_HEIGHT 500 

///input from user
int INPUT_METHOD;
///input from user
int INPUT_TRAVERSING;
///input from user
int INPUT_SCENE;	

RayBuffer buffer;

Bitmap *GLOBAL_BITMAP = NULL;
float *GLOBAL_PIXEL_BUFFER;
RayCompact *GLOBAL_RAYBUFFER;
///main camera
PinholeCamera cam;			

///compacted KDTree
__device__ KDTreeNodeCompact *CUDA_Nodes;
///compacted Spheres
__device__ SphereCompact *CUDA_Spheres;
///compacted Triangles
__device__ TriangleCompact *CUDA_Triangles;
///compacted  Materials
__device__ MaterialCompact *CUDA_Materials;
///compacted Lights
__device__ PointLightCompact *CUDA_Lights;
///compacted Rays
__device__ RayCompact *CUDA_Rays;
///compacted Triangle Indices
__device__ int *CUDA_Indices_Tri;
///compacted Sphere Indices
__device__ int *CUDA_Indices_Sph;
///output Color Buffer
__device__ float *CUDA_ColorBuffer;


void testBitmap();

/**
Normalize float[3] vector
*/
void normalize(float3 &vec)
{
	float m = sqrt((vec.x*vec.x) + (vec.y*vec.y) + (vec.z*vec.z));
}
/**
Get current time.
*/
long long getTime()
{
    SYSTEMTIME t;
    GetSystemTime(&t);

	long long time = ((((((t.wHour * 60) + t.wMinute) * 60) + t.wSecond) * 1000) + t.wMilliseconds);
	return time;
}
/**
Draw GLOBAL_BITMAP to glut window
*/
void DisplayFunc( void ) {
  glClearColor( 0.0, 1.0, 0.0, 1.0 );
  glClear( GL_COLOR_BUFFER_BIT );
  
  //testBitmap();

  if(GLOBAL_BITMAP != NULL ){
	glDrawPixels( GLOBAL_BITMAP->width, GLOBAL_BITMAP->height, GL_RGBA, GL_UNSIGNED_BYTE, GLOBAL_BITMAP->pixels );
  }

  glutSwapBuffers();
}
/**
Initialize Light collection
*/
void initializeLight()
{
	GLOBAL_BUFFER_LIGHT = LightCol();
}
/**
Initialize bitmap
*/
void initializeBitmap()
{
	GLOBAL_BITMAP = new Bitmap(IMAGE_WIDTH, IMAGE_HEIGHT);
	printf("initialized\n");
}
/**
print compactized KDTree
*/
void printCompactTree()
{
	CompactTree *c = &GLOBAL_COMPACT;
	KDTreeNodeCompact *n = (c->nodes);
	SphereCompact *s = (c->sphere);
	TriangleCompact *t = (c->tri);
	//printf("TRIANGLE BUFFER ADDRESS %d \n",c->tri);
	MaterialCompact *m = (c->mat);
	printf("NODES \n");
	for(int i = 0; i < c->countNode; i++)
	{
		printf("id %d   %d <--> %d %d <--> %d %d <--> %d     s %d t %d     %d \n", n[i].id, n[i].neighbors[0], n[i].neighbors[1], n[i].neighbors[2], n[i].neighbors[3], n[i].neighbors[4], n[i].neighbors[5], n[i].countSph, n[i].countTri, (n[i].isLeaf)?0:1);
		printf("sph from %d next %d tri from %d to %d\n",n[i].offsetSph, n[i].countSph,n[i].offsetTri,n[i].countTri);
	}
	printf("SPHERE INDICES\n");
	for(int i = 0; i < c->sphereIndicesSize; i++)
	{
		printf("%d ",c->sphereIndices[i]);
	}
	printf("SPHERES\n");
	for(int i = 0; i < c->countSph; i++)
	{
		printf("id %d  %f %f %f    %f    mat %d \n", i, s[i].pos[0], s[i].pos[1], s[i].pos[2], s[i].r, s[i].mat);
	}
	printf("TRIANGLE INDICES\n");
	for(int i = 0; i < c->triangleIndicesSize; i++)
	{
		printf("%d ",c->trinagleIndices[i]);
	}
	printf("TRIANGLES\n");
	for(int i = 0; i < c->countTri; i++)
	{
		printf("id %d  %f %f %f   %f %f %f   %f %f %f    mat %d \n", i,t[i].v0[0],t[i].v0[1],t[i].v0[2],t[i].v1[0],t[i].v1[1],t[i].v1[2],t[i].v2[0],t[i].v2[1],t[i].v2[2], s[i].mat);
	}
	printf("MATERIALS\n");
	for(int i = 0; i < c->countMat; i++)
	{
		printf("id %d   %f %f %f \n",i,m[i].diff[0],m[i].diff[1],m[i].diff[2]);
	}
	printf("\nEND\n");

}
/**
test scene
*/
void buildTestBall()
{
	float w,h,f;
	w = 6.0f;
	h = 6.0f;
	f = 4.0f;

	cam = PinholeCamera(Vec3(0.0f,0.0f,0.0f), Vec3(0.0f,0.0f,-5.0f), Vec3(0.0f,1.0f,0.0f),f,w,h);
	buffer = RayBuffer();
	PointLight *l;
	l = new PointLight(Vec3(0.0f,4.0f,0.0f),Col(1.0f,1.0f,1.0f), 50, 1, 1, 1);
	GLOBAL_BUFFER_LIGHT.addLight(*l);
	//l = new PointLight(Vec3(0.0f,-1.0f,-1.0f),Col(1.0f,1.0f,1.0f), 3, 1, 1, 1);
	//GLOBAL_BUFFER_LIGHT.addLight(*l);
	


	std::vector<Collidable *> *vec = new std::vector<Collidable *>();

	Collidable *c;
	
	c = new Sphere(Vec3(-0.750f, 0.0f, -8.0f), 1.0f, Material::getMaterialIndex(1));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(1.250f, 0.0f, -8.0f), 1.0f, Material::getMaterialIndex(2));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(0.0f, 0.0f, -2.0f), 0.30f, Material::getMaterialIndex(3));
	c->precomputeBounds();
	vec->push_back(c);
	

	GLOBAL_TREE.buildTreeStart(*vec);
}
/**
test scene
*/
void buildMassiveScene1()
{
	float w,h,f;
	w = 6.0f;
	h = 6.0f;
	f = 4.0f;

	cam = PinholeCamera(Vec3(0.0f,0.0f,0.0f), Vec3(0.0f,0.0f,-5.0f), Vec3(0.0f,1.0f,0.0f),f,w,h);
	buffer = RayBuffer();
	PointLight *l;
	l = new PointLight(Vec3(0.0f,4.0f,0.0f),Col(1.0f,1.0f,1.0f), 50, 1, 1, 1);
	GLOBAL_BUFFER_LIGHT.addLight(*l);

	std::vector<Collidable *> *vec = new std::vector<Collidable *>();

	Collidable *c;
	int cons = 20;
	float off = 01.0f;
	for(int i = 0; i < cons; i++)
		for(int j = 0; j < cons; j++)
			for(int k = 0; k < cons; k++)
			{
				c = new Sphere(Vec3(-off*cons/2.0f + off * i, -off*cons/2.0f + off * j, -5.0f - k*off), 0.250f, Material::getMaterialIndex(2));
				c->precomputeBounds();
				vec->push_back(c);
			}
	
	GLOBAL_TREE.buildTreeStart(*vec);
}

void buildMassiveScene2()
{
	float w,h,f;
	w = 6.0f;
	h = 6.0f;
	f = 4.0f;

	cam = PinholeCamera(Vec3(0.0f,0.0f,0.0f), Vec3(0.0f,0.0f,-5.0f), Vec3(0.0f,1.0f,0.0f),f,w,h);
	buffer = RayBuffer();
	PointLight *l;
	l = new PointLight(Vec3(0.0f,0.0f,-8.0f),Col(1.0f,1.0f,1.0f), 50, 1, 1, 1);
	GLOBAL_BUFFER_LIGHT.addLight(*l);
	l = new PointLight(Vec3(0.0f,0.0f,0.0f),Col(1.0f,1.0f,1.0f), 150, 1, 1, 1);
	GLOBAL_BUFFER_LIGHT.addLight(*l);

	


	std::vector<Collidable *> *vec = new std::vector<Collidable *>();
	Collidable *c;
	float s = 0.250f;
	float r = 0.15f;
	float g = 0.025;
	int d = 30;
	float spin = 3.0f;
	float b = 3.14f/spin;
	float a = 3.14f/(15.0f);
	float bou = 3.14f/(4.0f);

	float tmp;
	float baseD = 6.0f;

	
	c = new Sphere(Vec3( 0, 0, -baseD ), 0.250f, Material::getMaterialIndex(1));
	c->precomputeBounds();
	vec->push_back(c);
	for(int j = 0; j < spin; j++)
	{
		tmp = j*b;
		for(int i = 1; i <= d; i++)
		{
			c = new Sphere(Vec3( s * i * sin(tmp + a*i), s * i * cos(tmp + a*i), -baseD - 2*sin(0.5f * bou * i) ), r , Material::getMaterialIndex(11));
			c->precomputeBounds();
			vec->push_back(c);
			c = new Sphere(Vec3( -s * i * sin(tmp + a*i), -s * i * cos(tmp + a*i), -baseD - 2*sin(0.5f * bou * i) ), r , Material::getMaterialIndex(11));
			c->precomputeBounds();
			vec->push_back(c);
		}
	}
	for(int j = 0; j < spin; j++)
	{
		tmp = j*b;
		for(int i = 1; i <= d; i++)
		{
			c = new Sphere(Vec3( -s * i * sin(tmp + a*i), s * i * cos(tmp + a*i), -baseD - 2*sin(0.5f * bou * i) ), r , Material::getMaterialIndex(12));
			c->precomputeBounds();
			vec->push_back(c);
			c = new Sphere(Vec3( s * i * sin(tmp + a*i), -s * i * cos(tmp + a*i), -baseD - 2*sin(0.5f * bou * i) ), r , Material::getMaterialIndex(12));
			c->precomputeBounds();
			vec->push_back(c);
		}
	}
	
	GLOBAL_TREE.buildTreeStart(*vec);
}

void buildMassiveScene3()
{
	float w,h,f;
	w = 6.0f;
	h = 6.0f;
	f = 4.0f;

	cam = PinholeCamera(Vec3(0.0f,0.0f,0.0f), Vec3(0.0f,0.0f,-5.0f), Vec3(0.0f,1.0f,0.0f),f,w,h);
	buffer = RayBuffer();
	PointLight *l;
	l = new PointLight(Vec3(0.0f,2.0f,0.0f),Col(1.0f,1.0f,1.0f), 20, 1, 1, 1);
	GLOBAL_BUFFER_LIGHT.addLight(*l);
	l = new PointLight(Vec3(1.0f,-2.0f,0.0f),Col(1.0f,1.0f,1.0f), 10, 1, 1, 1);
	GLOBAL_BUFFER_LIGHT.addLight(*l);

	


	std::vector<Collidable *> *vec = new std::vector<Collidable *>();
	Collidable *c;

	int con = 10;
	
	c = new Sphere(Vec3( 0.0f, 0.0f, 0.0f ), 01.0f, Material::getMaterialIndex(15));
	c->precomputeBounds();
	vec->push_back(c);

	for(int i = 1; i < con; i++)
	{
		c = new Sphere(Vec3( 0.3f * i, 0, -2-2*i ), 01.0f, Material::getMaterialIndex(15));
		c->precomputeBounds();
		vec->push_back(c);
	}
	
	GLOBAL_TREE.buildTreeStart(*vec);
}
/**
test scene
*/
void buildTestScene1()
{
	float w,h,f;
	w = 9.0f;
	h = 9.0f;
	f = 6.0f;

	cam = PinholeCamera(Vec3(0.0f,0.0f,0.0f), Vec3(0.0f,0.0f,-5.0f), Vec3(0.0f,1.0f,0.0f),f,w,h);
	buffer = RayBuffer();
	PointLight *l;
	l = new PointLight(Vec3(1.0f,0.0f,1.0f),Col(1.0f,1.0f,1.0f), 15, 1, 1, 1);
	GLOBAL_BUFFER_LIGHT.addLight(*l);
	l = new PointLight(Vec3(-3.0f,3.0f,-1.0f),Col(1.0f,1.0f,1.0f), 10, 1, 1, 1);
	GLOBAL_BUFFER_LIGHT.addLight(*l);
	l = new PointLight(Vec3(2.0f,0.0f,0.0f),Col(0.30f,0.30f,0.30f), 4, 1, 1, 1);
	GLOBAL_BUFFER_LIGHT.addLight(*l);
	


	std::vector<Collidable *> *vec = new std::vector<Collidable *>();

	Collidable *c;

	c = new Sphere(Vec3(0.0f, .5f, -4.20f), 1.0f, Material::getMaterialIndex(1));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(-0.0f, -1.0f, -3.0f), 0.5f, Material::getMaterialIndex(2));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(-0.0f, -0.0f, -03.0f), 0.21f, Material::getMaterialIndex(3));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(0.7f, 0.30f, -2.5f), 0.5f, Material::getMaterialIndex(4));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(02.0f, 0.00f, -5.0f), 0.70f, Material::getMaterialIndex(4));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(0.9f, 0.00f, -02.50f), 0.50f, Material::getMaterialIndex(7));
	c->precomputeBounds();
	vec->push_back(c);
	

	GLOBAL_TREE.buildTreeStart(*vec);
	/*
	
	GLOBAL_TREE.isLeaf = true;
	GLOBAL_TREE.items = vec;
	GLOBAL_TREE.bounds[0] = -100;
	GLOBAL_TREE.bounds[1] = 100;
	GLOBAL_TREE.bounds[2] = -100;
	GLOBAL_TREE.bounds[3] = 100;
	GLOBAL_TREE.bounds[4] = -100;
	GLOBAL_TREE.bounds[5] = 100;

	GLOBAL_TREE.axis = -1;
	GLOBAL_TREE.depth = 0;
	GLOBAL_TREE.id = 55;
	*/
}
/**
test scene
*/
void buildTestScene2()
{
	float w,h,f;
	w = 9.0f;
	h = 9.0f;
	f = 4.0f;
	
	float con = 2;

	cam = PinholeCamera(Vec3(-0.0f,1.0f,0.0f), Vec3(-0.0f,0.0f,-5.0f), Vec3(0.0f,1.0f,0.0f),f,w,h);
	buffer = RayBuffer();
	PointLight *l;
	l = new PointLight(Vec3(0.0f, 01.0f * (con-0.10f), -con),Col(1.0f,1.0f,1.0f), 15, 1, 1, 1);
	GLOBAL_BUFFER_LIGHT.addLight(*l);
	l = new PointLight(Vec3(0.0f, 0.0f, 0.0f),Col(1.0f,1.0f,1.0f), 2, 1, 1, 0);
	//lights.addLight(*l);
	

	std::vector<Collidable *> *vec = new std::vector<Collidable *>();

	Collidable *c;
	
	//c = new Sphere(Vec3(0.0f, 0.0f, -1.0f), 0.50f, Material::getMaterialIndex(3));
	//c->precomputeBounds();
	//vec->push_back(c);

	c = new Sphere(Vec3(-1.0f, -1.0f, -3.0f), 1.0f, Material::getMaterialIndex(1));
	c->precomputeBounds();
	vec->push_back(c);


	//left
	c = new Triangle( Vec3(-con,-con,con), Vec3(-con,-con,-2*con), Vec3(-con,con,con), Material::getMaterialIndex(8));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Triangle( Vec3(-con,-con,-2*con), Vec3(-con,con,-2*con), Vec3(-con,con,con), Material::getMaterialIndex(8));
	c->precomputeBounds();
	vec->push_back(c);
	//right
	c = new Triangle( Vec3(con,-con,con), Vec3(con,con,con), Vec3(con,-con,-2*con), Material::getMaterialIndex(9));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Triangle( Vec3(con,-con,-2*con), Vec3(con,con,con), Vec3(con,con,-2*con), Material::getMaterialIndex(9));
	c->precomputeBounds();
	vec->push_back(c);
	//back
	c = new Triangle( Vec3(-con,con,-2*con), Vec3(-con,-con,-2*con), Vec3(con,-con,-2*con), Material::getMaterialIndex(10));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Triangle( Vec3(-con,con,-2*con), Vec3(con,-con,-2*con), Vec3(con,con,-2*con), Material::getMaterialIndex(10));
	c->precomputeBounds();
	vec->push_back(c);
	//bottom
	c = new Triangle( Vec3(-con,-con,con), Vec3(con,-con,con), Vec3(con,-con,-2*con), Material::getMaterialIndex(10));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Triangle( Vec3(-con,-con,-2*con), Vec3(-con,-con,con), Vec3(con,-con,-2*con), Material::getMaterialIndex(10));
	c->precomputeBounds();
	vec->push_back(c);
	//top
	c = new Triangle( Vec3(-con,con,con), Vec3(con,con,-2*con), Vec3(con,con,con), Material::getMaterialIndex(10));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Triangle( Vec3(-con,con,-2*con), Vec3(con,con,-2*con), Vec3(-con,con,con), Material::getMaterialIndex(10));
	c->precomputeBounds();
	vec->push_back(c);

	
	//front
	c = new Triangle( Vec3(-con,con,con), Vec3(con,-con,con), Vec3(-con,-con,con), Material::getMaterialIndex(2));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Triangle( Vec3(-con,con,con), Vec3(con,con,con), Vec3(con,-con,con), Material::getMaterialIndex(2));
	c->precomputeBounds();
	vec->push_back(c);
	

	GLOBAL_TREE.buildTreeStart(*vec);
}
/**
test scene
*/
void buildTestScene3()
{
	
	float w,h,f;
	w = 6.0f;
	h = 6.0f;
	f = 4.0f;
	
	
	//cam = PinholeCamera(Vec3(-3.0f,-3.0f,-4.0f), Vec3(-2.0f,-2.0f,-5.0f), Vec3(0.0f,1.0f,0.0f),f,w,h);
	cam = PinholeCamera(Vec3(0.0f,0.0f,1.0f), Vec3(0.0f,0.0f,-6.0f), Vec3(0.0f,1.0f,0.0f),f,w,h);
	buffer = RayBuffer();
	PointLight *l;
	l = new PointLight(Vec3(0.0f, 0.0f, 0.0f),Col(1.0f,1.0f,1.0f), 1, 1, 0, 0);
	GLOBAL_BUFFER_LIGHT.addLight(*l);
	//l = new PointLight(Vec3(0.0f, 0.0f, 0.0f),Col(1.0f,1.0f,1.0f), 2, 1, 1, 0);
	//lights.addLight(*l);
	
	
	std::vector<Collidable *> *vec1 = new std::vector<Collidable *>();
	std::vector<Collidable *> *vec2 = new std::vector<Collidable *>();
	std::vector<Collidable *> *vec = new std::vector<Collidable *>();

	Collidable *c;
	
	c = new Sphere(Vec3(-2.0f, -2.0f, -5.0f), 0.50f, Material::getMaterialIndex(11));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(-2.0f, -2.0f, -7.0f), 0.50f, Material::getMaterialIndex(12));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(-2.0f, -2.0f, -9.0f), 0.50f, Material::getMaterialIndex(13));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(-2.0f, -2.0f, -11.0f), 0.50f, Material::getMaterialIndex(14));
	c->precomputeBounds();
	vec->push_back(c);
	
	c = new Sphere(Vec3(2.0f, -2.0f, -5.0f), 0.50f, Material::getMaterialIndex(11));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(2.0f, -2.0f, -7.0f), 0.50f, Material::getMaterialIndex(12));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(2.0f, -2.0f, -9.0f), 0.50f, Material::getMaterialIndex(13));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(2.0f, -2.0f, -11.0f), 0.50f, Material::getMaterialIndex(14));
	c->precomputeBounds();
	vec->push_back(c);
	

	
	c = new Sphere(Vec3(-2.0f, 2.0f, -5.0f), 0.50f, Material::getMaterialIndex(11));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(-2.0f, 2.0f, -7.0f), 0.50f, Material::getMaterialIndex(12));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(-2.0f, 2.0f, -9.0f), 0.50f, Material::getMaterialIndex(13));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(-2.0f, 2.0f, -11.0f), 0.50f, Material::getMaterialIndex(14));
	c->precomputeBounds();
	vec->push_back(c);
	
	c = new Sphere(Vec3(2.0f, 2.0f, -5.0f), 0.50f, Material::getMaterialIndex(11));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(2.0f, 2.0f, -7.0f), 0.50f, Material::getMaterialIndex(12));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(2.0f, 2.0f, -9.0f), 0.50f, Material::getMaterialIndex(13));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(2.0f, 2.0f, -11.0f), 0.50f, Material::getMaterialIndex(14));
	c->precomputeBounds();
	vec->push_back(c);

	GLOBAL_TREE.buildTreeStart(*vec);
}
/**
test scene
*/
void buildTestScene4()
{
	
	float w,h,f;
	w = 9.0f;
	h = 6.0f;
	f = 6.0f;
	
	
	//cam = PinholeCamera(Vec3(-3.0f,-3.0f,-4.0f), Vec3(-2.0f,-2.0f,-5.0f), Vec3(0.0f,1.0f,0.0f),f,w,h);
	cam = PinholeCamera(Vec3(0.0f,0.0f,-0.0f), Vec3(0.0f,0.0f,-6.0f), Vec3(0.0f,1.0f,0.0f),f,w,h);
	buffer = RayBuffer();
	PointLight *l;
	l = new PointLight(Vec3(0.0f, 0.0f, 0.0f),Col(1.0f,1.0f,1.0f), 1, 1, 0, 0);
	GLOBAL_BUFFER_LIGHT.addLight(*l);
	//l = new PointLight(Vec3(0.0f, 0.0f, 0.0f),Col(1.0f,1.0f,1.0f), 2, 1, 1, 0);
	//lights.addLight(*l);
	
	std::vector<Collidable *> *vec = new std::vector<Collidable *>();

	Collidable *c;
	
	float posX, posY, posZ;
	float t0x, t0y, t1x, t1y, t2x, t2y;
	t0x = -0.5f;
	t0y = -0.5f;
	t1x = 0.5f;
	t1y = -0.5f;
	t2x = -0.0f;
	t2y = 1.5f;

	posX = 0;
	posY = 0;
	posZ = -2;
	
	//c = new Triangle( Vec3(-1,-1,-3), Vec3(1,-1,-3), Vec3(0,1.5f,-3), Material::getMaterialIndex(1));
	
	//c = new Triangle( Vec3(posX+t0x,posY+t0y,posZ), Vec3(posX+t1x,posY+t1y,posZ), Vec3(posX+t2x,posY+t2y,posZ), Material::getMaterialIndex(11));
	//c->precomputeBounds();
	//vec->push_back(c);

	//c = new Sphere(Vec3(posX, posY, posZ), 0.50f, Material::getMaterialIndex(14));
	//c->precomputeBounds();
	//vec->push_back(c);

	posX = 2;
	//c = new Triangle( Vec3(posX+t0x,posY+t0y,posZ), Vec3(posX+t1x,posY+t1y,posZ), Vec3(posX+t2x,posY+t2y,posZ), Material::getMaterialIndex(11));
	//c->precomputeBounds();
	//vec->push_back(c);
	
	//c = new Sphere(Vec3(posX, posY, posZ), 0.50f, Material::getMaterialIndex(14));
	//c->precomputeBounds();
	//vec->push_back(c);

	c = new Triangle(Vec3(-5, -2, -5),Vec3(5, -2, -5),Vec3(0, 4, -5), Material::getMaterialIndex(14));
	c->precomputeBounds();
	vec->push_back(c);

	GLOBAL_TREE.buildTreeStart(*vec);
}
/**
test scene
*/
void buildTestScene5()
{
	float w,h,f;
	w = 9.0f;
	h = 9.0f;
	f = 4.0f;
	
	float con = 2;

	cam = PinholeCamera(Vec3(-0.0f,0.0f,0.0f), Vec3(-0.0f,0.0f,-5.0f), Vec3(0.0f,1.0f,0.0f),f,w,h);
	buffer = RayBuffer();
	PointLight *l;
	l = new PointLight(Vec3(0.0f, 01.0f * (con-0.10f), -con),Col(1.0f,1.0f,1.0f), 15, 1, 1, 1);
	GLOBAL_BUFFER_LIGHT.addLight(*l);
	l = new PointLight(Vec3(0.0f, 0.0f, 0.0f),Col(1.0f,1.0f,1.0f), 2, 1, 1, 0);
	//lights.addLight(*l);
	

	std::vector<Collidable *> *vec = new std::vector<Collidable *>();

	Collidable *c;

	Vec3 corner = Vec3(-6,-6,-8);
	Vec3 locX = Vec3(12,0,0);
	Vec3 locY = Vec3(0,12,0);
	int sizeX = 32;
	int sizeY = 32;
	/*
	c = new Triangle( Vec3(corner.x,corner.y,corner.z), Vec3(corner.x + 8,corner.y + 8,corner.z + 0), Vec3(corner.x ,corner.y + 8,corner.z), Material::getMaterialIndex(2));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Triangle( Vec3(corner.x,corner.y,corner.z), Vec3(corner.x + 8,corner.y,corner.z), Vec3(corner.x + 8,corner.y + 8,corner.z + 0), Material::getMaterialIndex(1));
	c->precomputeBounds();
	vec->push_back(c);
	*/

	
	locX.mulLoc(1.0f/sizeX);
	locY.mulLoc(1.0f/sizeY);
	
	Vec3 tC;
	for(int i = 0; i < sizeX; i++)
	{
		for(int j = 0; j < sizeY; j++)
		{
			tC = Vec3(corner.x + i*locX.x + j*locY.x, corner.y + i*locX.y + j*locY.y, corner.z + i*locX.z + j*locY.z);

			
			c = new Triangle( Vec3(tC.x,tC.y,tC.z), Vec3(tC.x + locX.x + locY.x,tC.y + locX.y + locY.y,tC.z + locX.z + locY.z), Vec3(tC.x + locY.x ,tC.y + locY.y,tC.z + locY.z), Material::getMaterialIndex(2));
			c->precomputeBounds();
			vec->push_back(c);
			c = new Triangle( Vec3(tC.x,tC.y,tC.z), Vec3(tC.x + locX.x,tC.y + locX.y,tC.z + locX.z), Vec3(tC.x + locX.x + locY.x,tC.y + locX.y + locY.y,tC.z + locX.z + locY.z), Material::getMaterialIndex(1));
			c->precomputeBounds();
			vec->push_back(c);
		}
	}
	
	GLOBAL_TREE.buildTreeStart(*vec);

}

// funkce pro osetreni chyb
static void HandleError( cudaError_t error, const char *file, int line ) {
  if (error != cudaSuccess) {
    printf( "%s in %s at line %d\n", cudaGetErrorString( error ), file, line );
	scanf(" ");
    exit( EXIT_FAILURE );
  }
}
#define BUFFER_EXTENSION 1.5f
#define CHECK_ERROR( error ) ( HandleError( error, __FILE__, __LINE__ ) )

/**
Initialize cuda buffers
*/
void cudaInitialize(int rayBufferSize,int pixCount)
{
	CHECK_ERROR( cudaMalloc( (void**)&(CUDA_Nodes), GLOBAL_COMPACT.countNode*sizeof(KDTreeNodeCompact) ) );
	CHECK_ERROR( cudaMalloc( (void**)&(CUDA_Spheres), GLOBAL_COMPACT.countSph*sizeof(SphereCompact) ) );
	CHECK_ERROR( cudaMalloc( (void**)&(CUDA_Triangles), GLOBAL_COMPACT.countTri*sizeof(TriangleCompact) ) );
	CHECK_ERROR( cudaMalloc( (void**)&(CUDA_Materials), GLOBAL_COMPACT.countMat*sizeof(MaterialCompact) ) );
	CHECK_ERROR( cudaMalloc( (void**)&(CUDA_Lights), GLOBAL_COMPACT.countPLi*sizeof(PointLightCompact) ) );
	
	CHECK_ERROR( cudaMalloc( (void**)&(CUDA_Indices_Tri), GLOBAL_COMPACT.triangleIndicesSize*sizeof(int) ) );
	CHECK_ERROR( cudaMalloc( (void**)&(CUDA_Indices_Sph), GLOBAL_COMPACT.sphereIndicesSize*sizeof(int) ) );

	CHECK_ERROR( cudaMalloc( (void**)&(CUDA_Rays), rayBufferSize*sizeof(RayCompact) ) );
	CHECK_ERROR( cudaMalloc( (void**)&(CUDA_ColorBuffer), pixCount*3*sizeof(float) ) );
}
/**
Free cuda buffers
*/
void cudaFree()
{
	cudaFree(CUDA_Nodes);
	cudaFree(CUDA_Spheres);
	cudaFree(CUDA_Triangles);
	cudaFree(CUDA_Materials);
	cudaFree(CUDA_Lights);
	cudaFree(CUDA_Indices_Tri);
	cudaFree(CUDA_Indices_Sph);
	
	cudaFree(CUDA_Rays);
	cudaFree(CUDA_ColorBuffer);
}

/**
Compute length of vector
*/
__device__ float length(float *vec)
{
	return sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
}
/**
Normalize vector
*/
__device__ void normalize(float *vec)
{
	float f = 1.0f / length(vec);
	vec[0] *= f;
	vec[1] *= f;
	vec[2] *= f;
}
/**
Compute dot product
*/
__device__ float dot(float *v0, float *v1)
{
	return v0[0]*v1[0] + v0[1]*v1[1] + v0[2]*v1[2];
}
/**
Compute cross product
*/
__device__ void cross(float *in1, float *in2, float *out)
{
		out[0] = in1[1] * in2[2] - in1[2] * in2[1];
		out[1] = in1[2] * in2[0] - in1[0] * in2[2];
		out[2] = in1[0] * in2[1] - in1[1] * in2[0];
}
/**
Check if point is inside node nodes[nodeId]

/param[in] nodes compacted KD tree
/param[in] nodeId index of node from KD tree
/param[in] point point 
/return true if point is ni node
*/
__device__ bool isInNode(KDTreeNodeCompact *nodes, int nodeId, float *point)
{
	if(nodes[nodeId].bounds[0] > point[0] || nodes[nodeId].bounds[1] < point[0]){return false;}
	if(nodes[nodeId].bounds[2] > point[1] || nodes[nodeId].bounds[3] < point[1]){return false;}
	if(nodes[nodeId].bounds[4] > point[2] || nodes[nodeId].bounds[5] < point[2]){return false;}
	return true;
}
/**
Find point where ray enters node of KDTree.

/param[in] nodes compacted KD tree
/param[in] nodeId index of node from KD tree
/param[in] start origin of ray
/param[in] dir direction of ray (normalized)
/param[in] outPoint output point
/return value depends on entering side(0->-x, 1->x, 2->-y, 3->y, 4->-z, 5->z)
*/
__device__ int findEnterPoint(KDTreeNodeCompact *nodes, int nodeId, float *start, float *dir, float *outPoint)
{
	float corner[3];
	float bounds[6];
	float n[3];
	float nX, nY, nZ, d, u, div, t;
	
	bounds[0] = nodes[nodeId].bounds[0];
	bounds[1] = nodes[nodeId].bounds[1];
	bounds[2] = nodes[nodeId].bounds[2];
	bounds[3] = nodes[nodeId].bounds[3];
	bounds[4] = nodes[nodeId].bounds[4];
	bounds[5] = nodes[nodeId].bounds[5];

	if(dir[0] < 0.0f)
	{
		corner[0] = bounds[1];
		nX = 1;
	}else{
		corner[0] = bounds[0];
		nX = -1;
	}
	if(dir[1] < 0.0f)
	{
		corner[1] = bounds[3];
		nY = 1;
	}else{
		corner[1] = bounds[2];
		nY = -1;
	}
	if(dir[2] < 0.0f)
	{
		corner[2] = bounds[5];
		nZ = 1;
	}else{
		corner[2] = bounds[4];
		nZ = -1;
	}

	//X plane
	n[0] = nX;
	n[1] = 0.0f;
	n[2] = 0.0f;

	d = dot(n, corner);
	u = dot(n, start);
	div = dot(n, dir);

	if(div != 0.0f)
	{
		t = (d-u)/div;
		
		outPoint[0] = start[0] + t * dir[0];
		outPoint[1] = start[1] + t * dir[1];
		outPoint[2] = start[2] + t * dir[2];
		if(outPoint[1] >= bounds[2] && outPoint[1] <= bounds[3] && outPoint[2] >= bounds[4] && outPoint[2] <= bounds[5])
		{
			return (nX < 0)?0:1;
		}
	}

	//Y plane
	n[0] = 0.0f;
	n[1] = nY;
	n[2] = 0.0f;

	d = dot(n, corner);
	u = dot(n, start);
	div = dot(n, dir);

	if(div != 0.0f)
	{
		t = (d-u)/div;
		
		outPoint[0] = start[0] + t * dir[0];
		outPoint[1] = start[1] + t * dir[1];
		outPoint[2] = start[2] + t * dir[2];
		if(outPoint[0] >= bounds[0] && outPoint[0] <= bounds[1] && outPoint[2] >= bounds[4] && outPoint[2] <= bounds[5])
		{
			return (nY < 0)?2:3;
		}
	}

	//Z plane
	n[0] = 0.0f;
	n[1] = 0.0f;
	n[2] = nZ;

	d = dot(n, corner);
	u = dot(n, start);
	div = dot(n, dir);

	if(div != 0.0f)
	{
		t = (d-u)/div;
		
		outPoint[0] = start[0] + t * dir[0];
		outPoint[1] = start[1] + t * dir[1];
		outPoint[2] = start[2] + t * dir[2];
		if(outPoint[1] >= bounds[2] && outPoint[1] <= bounds[3] && outPoint[0] >= bounds[0] && outPoint[0] <= bounds[1])
		{
			return (nZ < 0)?4:5;
		}
	}
	outPoint[0] = 0.0f;
	outPoint[1] = 0.0f;
	outPoint[2] = 0.0f;
	return -1;
}
/**
Find point where ray escapes node of KDTree.

/param[in] nodes compacted KD tree
/param[in] nodeId index of node from KD tree
/param[in] start origin of ray
/param[in] dir direction of ray (normalized)
/param[in] outPoint output point
/return value depends on exiting side(0->-x, 1->x, 2->-y, 3->y, 4->-z, 5->z)
*/
__device__ int findExitPoint(KDTreeNodeCompact *nodes, int nodeId, float *start, float *dir, float *outPoint)
{
	float corner[3];
	float bounds[6];
	float n[3];
	float nX, nY, nZ, d, u, div, t;
	
	bounds[0] = nodes[nodeId].bounds[0];
	bounds[1] = nodes[nodeId].bounds[1];
	bounds[2] = nodes[nodeId].bounds[2];
	bounds[3] = nodes[nodeId].bounds[3];
	bounds[4] = nodes[nodeId].bounds[4];
	bounds[5] = nodes[nodeId].bounds[5];

	if(dir[0] < 0.0f)
	{
		corner[0] = bounds[0];
		nX = 1;
	}else{
		corner[0] = bounds[1];
		nX = -1;
	}
	if(dir[1] < 0.0f)
	{
		corner[1] = bounds[2];
		nY = 1;
	}else{
		corner[1] = bounds[3];
		nY = -1;
	}
	if(dir[2] < 0.0f)
	{
		corner[2] = bounds[4];
		nZ = 1;
	}else{
		corner[2] = bounds[5];
		nZ = -1;
	}

	//X plane
	n[0] = nX;
	n[1] = 0.0f;
	n[2] = 0.0f;

	d = dot(n, corner);
	u = dot(n, start);
	div = dot(n, dir);

	if(div != 0.0f)
	{
		t = (d-u)/div;
		
		outPoint[0] = start[0] + t * dir[0];
		outPoint[1] = start[1] + t * dir[1];
		outPoint[2] = start[2] + t * dir[2];
		if(outPoint[1] >= bounds[2] && outPoint[1] <= bounds[3] && outPoint[2] >= bounds[4] && outPoint[2] <= bounds[5])
		{
			return (nX > 0)?0:1;
		}
	}

	//Y plane
	n[0] = 0.0f;
	n[1] = nY;
	n[2] = 0.0f;

	d = dot(n, corner);
	u = dot(n, start);
	div = dot(n, dir);

	if(div != 0.0f)
	{
		t = (d-u)/div;
		
		outPoint[0] = start[0] + t * dir[0];
		outPoint[1] = start[1] + t * dir[1];
		outPoint[2] = start[2] + t * dir[2];
		if(outPoint[0] >= bounds[0] && outPoint[0] <= bounds[1] && outPoint[2] >= bounds[4] && outPoint[2] <= bounds[5])
		{
			return (nY > 0)?2:3;
		}
	}

	//Z plane
	n[0] = 0.0f;
	n[1] = 0.0f;
	n[2] = nZ;

	d = dot(n, corner);
	u = dot(n, start);
	div = dot(n, dir);

	if(div != 0.0f)
	{
		t = (d-u)/div;
		
		outPoint[0] = start[0] + t * dir[0];
		outPoint[1] = start[1] + t * dir[1];
		outPoint[2] = start[2] + t * dir[2];
		if(outPoint[1] >= bounds[2] && outPoint[1] <= bounds[3] && outPoint[0] >= bounds[0] && outPoint[0] <= bounds[1])
		{
			return (nZ > 0)?4:5;
		}
	}
	outPoint[0] = 0.0f;
	outPoint[1] = 0.0f;
	outPoint[2] = 0.0f;
	return -1;
}
/**
Copy values of one ray to another

/param[in] in ray to copy from
/param[in] out ray to copy to
*/
__device__ void copyRay(RayCompact& in, RayCompact& out)
{
			out.start[0] = in.start[0];
			out.start[1] = in.start[1];
			out.start[2] = in.start[2];
			out.end[0] = in.end[0];
			out.end[1] = in.end[1];
			out.end[2] = in.end[2];
			out.color[0] = in.color[0];
			out.color[1] = in.color[1];
			out.color[2] = in.color[2];
			out.contribution[0] = in.contribution[0];
			out.contribution[1] = in.contribution[1];
			out.contribution[2] = in.contribution[2];
			
			out.length = in.length;

			out.depth = in.depth;
			out.offset = in.offset;
			out.orientation = in.orientation;
			out.type = in.type;
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
/return true if collided
*/
__device__ bool collideSphere(RayCompact& ray, SphereCompact *spheres, int index, float *impact, float *normal, float& len, float bound)
{
	float b, c, D;
	float dir[3];
	float temp[3];
	float sphP[3];
	
	sphP[0] = spheres[index].pos[0];
	sphP[1] = spheres[index].pos[1];
	sphP[2] = spheres[index].pos[2];
	
	dir[0] = ray.end[0] - ray.start[0];
	dir[1] = ray.end[1] - ray.start[1];
	dir[2] = ray.end[2] - ray.start[2];
	normalize(dir);
	
	temp[0] = ray.start[0] - sphP[0];
	temp[1] = ray.start[1] - sphP[1];
	temp[2] = ray.start[2] - sphP[2];

	float r = spheres[index].r;
	

	b = 2* dot(dir, temp);
	c = dot(temp, temp) - r*r; 
	D = b*b - 4*c;


	// D < 0
	if(D < 0.0f){return false;}
	// D = 0 
	if(D == 0.0f){return false;}
	// D > 0

	float Ds, t;
	float imp[3];
	float norm[3];
	Ds = sqrt(D);

	if(ray.orientation > 0.0f)
	{
		t = 0.5f * (-b - Ds);
	}else{
		t = 0.5f * (-b + Ds);
	}

	if(t < 0.0f || t > ray.length || t > bound){return false;}

	imp[0] = ray.start[0] + t * dir[0];
	imp[1] = ray.start[1] + t * dir[1];
	imp[2] = ray.start[2] + t * dir[2];
	
	norm[0] = imp[0] - sphP[0];
	norm[1] = imp[1] - sphP[1];
	norm[2] = imp[2] - sphP[2];
	normalize(norm);

	if(ray.orientation * dot(dir, norm) > 0.0f){return false;}
	
	impact[0] = imp[0];
	impact[1] = imp[1];
	impact[2] = imp[2];
	
	normal[0] = norm[0];
	normal[1] = norm[1];
	normal[2] = norm[2];
	len = t;
	return true;
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
/return true if collided
*/
__device__ bool collideTriangle(RayCompact& ray, TriangleCompact *triangles, int index, float *impact, float *normal, float& len, float bound)
{
	float v0[3];
	float v1[3];
	float v2[3];
	
	v0[0] = triangles[index].v0[0];
	v0[1] = triangles[index].v0[1];
	v0[2] = triangles[index].v0[2];

	v1[0] = triangles[index].v1[0];
	v1[1] = triangles[index].v1[1];
	v1[2] = triangles[index].v1[2];

	v2[0] = triangles[index].v2[0];
	v2[1] = triangles[index].v2[1];
	v2[2] = triangles[index].v2[2];


	float e1[3];
	float e2[3];
	float pvec[3];
	float tvec[3];
	float qvec[3];
	float dir[3];
	
	e1[0] = v1[0] - v0[0];
	e1[1] = v1[1] - v0[1];
	e1[2] = v1[2] - v0[2];

	e2[0] = v2[0] - v0[0];
	e2[1] = v2[1] - v0[1];
	e2[2] = v2[2] - v0[2];
	
	dir[0] = ray.end[0] - ray.start[0];
	dir[1] = ray.end[1] - ray.start[1];
	dir[2] = ray.end[2] - ray.start[2];
	normalize(dir);

	cross(dir, e2, pvec);
	float det = dot(e1, pvec);

	if(det > -EPSILON && det < EPSILON){return false;}
	
	tvec[0] = ray.start[0] - v0[0];
	tvec[1] = ray.start[1] - v0[1];
	tvec[2] = ray.start[2] - v0[2];

	float u = dot(tvec, pvec);
	if(u < 0.0f || u > det){return false;}

	cross(tvec, e1, qvec);
	float v = dot(dir, qvec);
	if(v < 0.0f || u + v > det){return false;}

	float t = dot(e2, qvec);
	det = 1/det;
	u *= det;
	v *= det;
	t *= det;
	
	if(t < 0.0f || t > len){return false;}
	if(t > bound){return false;}
	
	impact[0] = (1-u-v) * v0[0] + u * v1[0] + v * v2[0];
	impact[1] = (1-u-v) * v0[1] + u * v1[1] + v * v2[1];
	impact[2] = (1-u-v) * v0[2] + u * v1[2] + v * v2[2];
	
	normal[0] = (1-u-v) * triangles[index].n0[0] + u * triangles[index].n1[0] + v * triangles[index].n2[0];
	normal[1] = (1-u-v) * triangles[index].n0[1] + u * triangles[index].n1[1] + v * triangles[index].n2[1];
	normal[2] = (1-u-v) * triangles[index].n0[2] + u * triangles[index].n1[2] + v * triangles[index].n2[2];
	len = t;
	return true;
}
/**
Clip float to values from 0.0f to 1.0f
*/
__device__ float clip(float d)
{
	if(d > 1.0f){return 1.0f;}
	if(d < 0.0f){return 0.0f;}
	return d;
}
/**
Reflect vector with normal

/param[in] dir vector to reflect (normalized)
/param[in] norm normal of reflecting plane
/param[in] out output vector
*/
__device__ void reflect(float *dir, float *norm, float *out)
{
	float mult = -2 * (dir[0] * norm[0] + dir[1] * norm[1] + dir[2] * norm[2]);
	out[0] = dir[0] + norm[0] * mult;
	out[1] = dir[1] + norm[1] * mult;
	out[2] = dir[2] + norm[2] * mult;
}
/**
Compute specular decrease coeficient (clipped dot product of vector to light and ray direction reflected with plane normal )

/param[in] dir ray direction (normalized)
/param[in] toLight vector from reflection point to light
/param[in] normal of reflecting plane
*/
__device__ float getSpecularDecrease(float* dir, float *toLight, float *normal)
{
	float reflected[3];
	reflect(dir, normal, reflected);
	float d = reflected[0] * toLight[0] + reflected[1] * toLight[1] + reflected[2] * toLight[2];

	if( d < 0.0f)	{return 0.0f;}
	if(d > 1.0f)	{return 1.0f;}
	return d;
}
/**
Find leaf there point belongs in tree.

/param[in] nodes compacted KD tree
/param[in] nodeId id of node where to start
/param[in] point point used in search for leaf
/return index of leaf in nodes 
*/
__device__ int findNodeByPoint(KDTreeNodeCompact *nodes, int nodeId, float *point)
{
	if(nodes[nodeId].isLeaf)
	{
		return nodeId;
	}

	int next;
	float split = nodes[nodeId].split;

	if(point[nodes[nodeId].axis] < split)
	{
		next = nodes[nodeId].idLeft; 
	}else{
		next = nodes[nodeId].idRight;
	}
	return findNodeByPoint(nodes, next, point);
}

/**
Trace ray through scene, using basic KDTree traversing algorithm (always search for new leaf from root)

/param[in] ray ray to be traced
/param[in] nodes compacted KD tree
/param[in] spheres sphere buffer
/param[in] triangles triangle buffer
/param[in] materials material buffer
/param[in] lights light buffer
/param[in] lightCount how many lights are there
/param[in] indicesTri buffer of triangle indices
/param[in] indicesSph buffer of sphere indices
/param[in] colorBuffer output color buffer
/param[in] rays global buffer of rays
/param[in] stackInfo index where ray buffer has first free space
/param[in] goDeeper output info, true if reflection happened and ray was rewritten to new reflected ray

*/
__device__ void executeBasic(RayCompact& ray,KDTreeNodeCompact *nodes,SphereCompact *spheres,TriangleCompact *triangles,MaterialCompact *materials,	PointLightCompact *lights, int lightCount,	int *indicesTri,int *indicesSph,float *colorBuffer,	RayCompact *rays,unsigned int& stackInfo, bool& goDeeper)
{
	
	float dir[3];
	float pointIn[3];
	float pointOut[3];
	
	float impact[3];
	float normal[3];

	float dist[3];
	
	int initTri;
	int countTri;
	int initSph;
	int countSph;

	int indexNodeCurrent;
	int indexMat;
	int indexLight;
	int indexObject;

	int found;
	
	bool collided = false;
	bool collidedNow;

	bool refRev = false;
	bool refRevNow;

	float len = ray.length;
	float boxBoundLength;

	float colorMod[3];
	float transp;

	//-------------------------------------------------------------------------------------------------------------

	//compute direction of ray and starting point point
	dir[0] = ray.end[0] - ray.start[0];
	dir[1] = ray.end[1] - ray.start[1];
	dir[2] = ray.end[2] - ray.start[2];
	normalize(dir);

	pointIn[0] = ray.start[0];
	pointIn[1] = ray.start[1];
	pointIn[2] = ray.start[2];

	//skip this if ray is too deep
	if(ray.depth <= MAX_DEPTH)
	{
		//if ray origin isnt in kd tree, find point where it enters
		if(	!isInNode(nodes, 0, pointIn) )
		{
			found = findEnterPoint(nodes,0,ray.start,dir,pointIn);
			pointIn[0] += dir[0] * STEP_SIZE;
			pointIn[1] += dir[1] * STEP_SIZE;
			pointIn[2] += dir[2] * STEP_SIZE;
			if(found == -1)
			{
				pointIn[0] = ray.start[0];
				pointIn[1] = ray.start[1];
				pointIn[2] = ray.start[2];
			}
		}

		//ray traversing routine, first check if point is still in tree
		while( isInNode(nodes, 0, pointIn) )
		{
			indexNodeCurrent = findNodeByPoint(nodes, 0, pointIn);
			//find point, where ray eventuali leaves
			found = findExitPoint(nodes, indexNodeCurrent, ray.start, dir, pointOut);
			//make sure the point is not on border of leaves
			pointOut[0] += dir[0] * STEP_SIZE;
			pointOut[1] += dir[1] * STEP_SIZE;
			pointOut[2] += dir[2] * STEP_SIZE;

			dist[0] = pointOut[0] - ray.start[0];
			dist[1] = pointOut[1] - ray.start[1];
			dist[2] = pointOut[2] - ray.start[2];

			//compute at what length the ray leaves the leaf
			boxBoundLength = length(dist);

			//sphere collisions
			initSph = nodes[indexNodeCurrent].offsetSph;
			countSph = nodes[indexNodeCurrent].countSph;
			for(int i = initSph; i < initSph + countSph; i++)
			{
				indexObject = indicesSph[i];
				//try to collide
				collidedNow = collideSphere(ray, spheres, indexObject, impact, normal, len, boxBoundLength);
				refRevNow = true;
				//check if collision happened in this iteration
				if(collidedNow)
				{
					indexMat = spheres[indexObject].mat;
					transp = materials[indexMat].transparency;
					// if it is shadow ray hiting transparent object, just modify its color, but dont make true collision.
					if( ray.type == RAY_SHADOW && transp < 1.0f)
					{
						ray.color[0] *= (materials[indexMat].diff[0] * materials[indexMat].kD[0] * (1 - transp));
						ray.color[1] *= (materials[indexMat].diff[1] * materials[indexMat].kD[1] * (1 - transp));
						ray.color[2] *= (materials[indexMat].diff[2] * materials[indexMat].kD[2] * (1 - transp)); 
					}else{
						// ray really ends here (if no better collision exists)
						collided = true;
						ray.length = len;
						refRev = refRevNow;
					}
				}
			}

			//triangle collisions
			initTri = nodes[indexNodeCurrent].offsetTri;
			countTri = nodes[indexNodeCurrent].countTri;
			for(int i = initTri; i < initTri + countTri; i++)
			{
				indexObject = indicesTri[i];
				//try to collide
				collidedNow = collideTriangle(ray, triangles, indexObject, impact, normal, len, boxBoundLength);
				refRevNow = false;
				
				//check if collision happened in this iteration
				if(collidedNow)
				{
					indexMat = triangles[indexObject].mat;
					transp = materials[indexMat].transparency;
					// if it is shadow ray hiting transparent object, just modify its color, but dont make true collision.
					if( ray.type == RAY_SHADOW && transp < 1.0f)
					{
						ray.color[0] *= (materials[indexMat].diff[0] * materials[indexMat].kD[0] * transp);
						ray.color[1] *= (materials[indexMat].diff[1] * materials[indexMat].kD[1] * transp);
						ray.color[2] *= (materials[indexMat].diff[2] * materials[indexMat].kD[2] * transp); 
					}else{
						// ray really ends here (if no better collision exists)
						collided = true;
						ray.length = len;
						refRev = refRevNow;
					}
				}
			}

			// if at least one true collision happened, dont traverse tree any further
			if(collided){break;}
			if(found != -1)
			{
				// set new start point
				pointIn[0] = pointOut[0];
				pointIn[1] = pointOut[1];
				pointIn[2] = pointOut[2];
			}else{
				// we are outside tree space
				break;
			}
		}
	}

	if(!collided)
	{
		//no collision happened-> shadow ray reached the light or left scene, in any case just write its color into color buffer 
		float finalCol[3];
		
		finalCol[0] = clip(ray.color[0] * ray.contribution[0]);
		finalCol[1] = clip(ray.color[1] * ray.contribution[1]);
		finalCol[2] = clip(ray.color[2] * ray.contribution[2]);

		int offset = 3 * ray.offset;
		
		atomicAdd(colorBuffer + offset		, finalCol[0]);
		atomicAdd(colorBuffer + offset + 1	, finalCol[1]);
		atomicAdd(colorBuffer + offset + 2	, finalCol[2]);
		return;
	}

	// shadow rays dnt make any reflection, refraction or shadow rays
	if(ray.type == RAY_SHADOW){return;}
	
	RayCompact recursiveRay;
	float vDir[3];
	float atten;
	float specularDecrease;
	float d;
	if(ray.depth < MAX_DEPTH)
	{
		//SHADOW
		if(ray.orientation > 0.0f)
		{
			//go through all lights
			for(int i = 0; i < lightCount; i++)
			{
				//build new ray
				recursiveRay.start[0] = impact[0];
				recursiveRay.start[1] = impact[1];
				recursiveRay.start[2] = impact[2];
				recursiveRay.end[0] = lights[i].pos[0];
				recursiveRay.end[1] = lights[i].pos[1];
				recursiveRay.end[2] = lights[i].pos[2];
				vDir[0] = recursiveRay.end[0] - recursiveRay.start[0];
				vDir[1] = recursiveRay.end[1] - recursiveRay.start[1];
				vDir[2] = recursiveRay.end[2] - recursiveRay.start[2];

				recursiveRay.length = length(vDir);
				normalize(vDir);
				recursiveRay.start[0] += vDir[0] * STEP_SIZE;
				recursiveRay.start[1] += vDir[1] * STEP_SIZE;
				recursiveRay.start[2] += vDir[2] * STEP_SIZE;
				atten = lights[i].intensity/ ((lights[i].a2 * recursiveRay.length + lights[i].a1) * recursiveRay.length + lights[i].a0);
			
				specularDecrease = pow(getSpecularDecrease(dir, vDir, normal), materials[indexMat].shine);
				recursiveRay.color[0] = ( lights[i].col[0] * materials[indexMat].diff[0] + materials[indexMat].kS[0] * specularDecrease) * atten;
				recursiveRay.color[1] = ( lights[i].col[1] * materials[indexMat].diff[1] + materials[indexMat].kS[1] * specularDecrease) * atten;
				recursiveRay.color[2] = ( lights[i].col[2] * materials[indexMat].diff[2] + materials[indexMat].kS[2] * specularDecrease) * atten;
				
				recursiveRay.depth = ray.depth;
				recursiveRay.offset = ray.offset;
				recursiveRay.orientation = 1.0f;
				recursiveRay.type = RAY_SHADOW;
				
				d = clip(dot(vDir, normal)) * materials[indexMat].transparency;
				
				recursiveRay.contribution[0] = ray.contribution[0] * d * materials[indexMat].kD[0];
				recursiveRay.contribution[1] = ray.contribution[1] * d * materials[indexMat].kD[1];
				recursiveRay.contribution[2] = ray.contribution[2] * d * materials[indexMat].kD[2];
				
				bool no = false;
				//recursively execute rhadow ray, it wont go any deeper
				executeBasic(recursiveRay, nodes, spheres, triangles, materials, lights, lightCount, indicesTri, indicesSph, colorBuffer, rays, stackInfo, no);
			}
		}

		//REFRACTED (only in case of transparent objects)
		bool TIR = false;
		if(transp < 1.0f)
		{
			float dotP = dot(dir,normal);
			float tmpNorm[3];
			float ref;
			tmpNorm[0] = normal[0];
			tmpNorm[1] = normal[1];
			tmpNorm[2] = normal[2];

			//am i entering or leaving object<
			if(dotP < 0)
			{
				ref = 1.0f/materials[indexMat].refraction;
			}else{
				ref = materials[indexMat].refraction;
				dotP = -dotP;
				tmpNorm[0] = -tmpNorm[0];
				tmpNorm[1] = -tmpNorm[1];
				tmpNorm[2] = -tmpNorm[2];
			}

			//computation of how much the angle changes
			float s2 = (ref*ref)*(1-(dotP*dotP));
			float sqr = 1 - s2;
			if(sqr < 0.0f)
			{
				//all light will be reflected
				TIR = true;
			}else{
				sqr = -ref * dotP - sqrt(sqr);
				
				vDir[0] = dir[0] * ref + sqr * tmpNorm[0];
				vDir[1] = dir[1] * ref + sqr * tmpNorm[1];
				vDir[2] = dir[2] * ref + sqr * tmpNorm[2];
				
				// allocate one position at end of ray buffer and fill it with refraction ray
				unsigned int bufferAlloc = atomicInc(&stackInfo, UINT_MAX);
				rays[bufferAlloc].start[0] = impact[0] + STEP_SIZE * vDir[0];
				rays[bufferAlloc].start[1] = impact[1] + STEP_SIZE * vDir[1];
				rays[bufferAlloc].start[2] = impact[2] + STEP_SIZE * vDir[2];
				rays[bufferAlloc].end[0] = impact[0] + vDir[0];
				rays[bufferAlloc].end[1] = impact[1] + vDir[1];
				rays[bufferAlloc].end[2] = impact[2] + vDir[2];
				rays[bufferAlloc].color[0] = DEFAULT_COL_R;
				rays[bufferAlloc].color[1] = DEFAULT_COL_G;
				rays[bufferAlloc].color[2] = DEFAULT_COL_B;
				rays[bufferAlloc].contribution[0] = ray.contribution[0];
				rays[bufferAlloc].contribution[1] = ray.contribution[1];
				rays[bufferAlloc].contribution[2] = ray.contribution[2];

				rays[bufferAlloc].length = MAX_DIST;

				rays[bufferAlloc].depth = ray.depth + 1;
				rays[bufferAlloc].offset = ray.offset;
				rays[bufferAlloc].orientation = ((refRev) ? -1.0f : 1.0f) * ray.orientation;
				rays[bufferAlloc].type = RAY_REFRACTION;
			}
		}
	
		//REFLECTED (only in case of reflective objects)
		float refl = materials[indexMat].reflection;
		if(refl > 0.0f)
		{
			//build reflection ray
			//because this is the ond of this function, it is pointless to trace this ray recursively
			//it will be better to finish this, rewrite original ray and say the caller, that he should repeat the process
			float reflected[3];
			reflect(dir, normal, reflected);

			
			ray.start[0]	= impact[0] + STEP_SIZE * reflected[0];
			ray.start[1]	= impact[1] + STEP_SIZE * reflected[1];
			ray.start[2]	= impact[2] + STEP_SIZE * reflected[2];
			ray.end[0]		= impact[0] + reflected[0];
			ray.end[1]		= impact[1] + reflected[1];
			ray.end[2]		= impact[2] + reflected[2];

			ray.color[0]	= DEFAULT_COL_R;
			ray.color[1]	= DEFAULT_COL_G;
			ray.color[2]	= DEFAULT_COL_B;

			ray.contribution[0]	= ray.contribution[0] * materials[indexMat].kS[0] * refl * ((TIR)?1.0f:transp);
			ray.contribution[1]	= ray.contribution[1] * materials[indexMat].kS[1] * refl * ((TIR)?1.0f:transp);
			ray.contribution[2]	= ray.contribution[2] * materials[indexMat].kS[2] * refl * ((TIR)?1.0f:transp);

			ray.length = MAX_DIST;

			ray.depth = ray.depth + 1;
			ray.offset = ray.offset;
			ray.orientation = 1.0f;
			ray.type = RAY_REFLECTION;
			//this sayt it should be repeated
			goDeeper = true;
		}

	}
}
/**
Trace ray through scene, using neighborhood KDTree traversing(Each node knows what node is next to them so search for next node doesnt have to start from root)
For commented code go to _device_ void executeBasic(...) it is nearly identical

/param[in] ray ray to be traced
/param[in] nodes compacted KD tree
/param[in] spheres sphere buffer
/param[in] triangles triangle buffer
/param[in] materials material buffer
/param[in] lights light buffer
/param[in] lightCount how many lights are there
/param[in] indicesTri buffer of triangle indices
/param[in] indicesSph buffer of sphere indices
/param[in] colorBuffer output color buffer
/param[in] rays global buffer of rays
/param[in] stackInfo index where ray buffer has first free space
/param[in] goDeeper output info, true if reflection happened and ray was rewritten to new reflected ray
*/
__device__ void executeNeighbor(RayCompact& ray,KDTreeNodeCompact *nodes,SphereCompact *spheres,TriangleCompact *triangles,MaterialCompact *materials,	PointLightCompact *lights, int lightCount,	int *indicesTri,int *indicesSph,float *colorBuffer,	RayCompact *rays,unsigned int& stackInfo, bool& goDeeper)
{
	float dir[3];
	float pointIn[3];
	float pointOut[3];
	
	float impact[3];
	float normal[3];

	float dist[3];
	
	int initTri;
	int countTri;
	int initSph;
	int countSph;

	int indexNodeCurrent;
	int indexMat;
	int indexLight;
	int indexObject;

	int indexSubTree;

	int found;
	
	bool collided = false;
	bool collidedNow;

	bool refRev = false;
	bool refRevNow;

	float len = ray.length;
	float boxBoundLength;

	float colorMod[3];
	float transp;

	//-------------------------------------------------------------------------------------------------------------

	dir[0] = ray.end[0] - ray.start[0];
	dir[1] = ray.end[1] - ray.start[1];
	dir[2] = ray.end[2] - ray.start[2];
	normalize(dir);

	pointIn[0] = ray.start[0];
	pointIn[1] = ray.start[1];
	pointIn[2] = ray.start[2];

	indexSubTree = 0;

	if(ray.depth <= MAX_DEPTH)
	{
		if(	!isInNode(nodes, 0, pointIn) )
		{
			found = findEnterPoint(nodes,0,ray.start,dir,pointIn);
			pointIn[0] += dir[0] * STEP_SIZE;
			pointIn[1] += dir[1] * STEP_SIZE;
			pointIn[2] += dir[2] * STEP_SIZE;
			if(found == -1)
			{
				pointIn[0] = ray.start[0];
				pointIn[1] = ray.start[1];
				pointIn[2] = ray.start[2];
			}
		}

		while( isInNode(nodes, 0, pointIn) )
		{
			indexNodeCurrent = findNodeByPoint(nodes, indexSubTree, pointIn);

			found = findExitPoint(nodes, indexNodeCurrent, ray.start, dir, pointOut);
			pointOut[0] += dir[0] * STEP_SIZE;
			pointOut[1] += dir[1] * STEP_SIZE;
			pointOut[2] += dir[2] * STEP_SIZE;

			dist[0] = pointOut[0] - ray.start[0];
			dist[1] = pointOut[1] - ray.start[1];
			dist[2] = pointOut[2] - ray.start[2];

			boxBoundLength = length(dist);

			//sphere collisions
			initSph = nodes[indexNodeCurrent].offsetSph;
			countSph = nodes[indexNodeCurrent].countSph;
			for(int i = initSph; i < initSph + countSph; i++)
			{
				indexObject = indicesSph[i];
				collidedNow = collideSphere(ray, spheres, indexObject, impact, normal, len, boxBoundLength);
				refRevNow = true;
				if(collidedNow)
				{
					indexMat = spheres[indexObject].mat;
					transp = materials[indexMat].transparency;
					if( ray.type == RAY_SHADOW && transp < 1.0f)
					{
						ray.color[0] *= (materials[indexMat].diff[0] * materials[indexMat].kD[0] * (1 - transp));
						ray.color[1] *= (materials[indexMat].diff[1] * materials[indexMat].kD[1] * (1 - transp));
						ray.color[2] *= (materials[indexMat].diff[2] * materials[indexMat].kD[2] * (1 - transp)); 
					}else{
						collided = true;
						ray.length = len;
						refRev = refRevNow;
					}
				}
			}

			//triangle collisions
			initTri = nodes[indexNodeCurrent].offsetTri;
			countTri = nodes[indexNodeCurrent].countTri;
			for(int i = initTri; i < initTri + countTri; i++)
			{
				indexObject = indicesTri[i];
				collidedNow = collideTriangle(ray, triangles, indexObject, impact, normal, len, boxBoundLength);
				refRevNow = false;

				if(collidedNow)
				{
					indexMat = triangles[indexObject].mat;
					transp = materials[indexMat].transparency;

					if( ray.type == RAY_SHADOW && transp < 1.0f)
					{
						ray.color[0] *= (materials[indexMat].diff[0] * materials[indexMat].kD[0] * transp);
						ray.color[1] *= (materials[indexMat].diff[1] * materials[indexMat].kD[1] * transp);
						ray.color[2] *= (materials[indexMat].diff[2] * materials[indexMat].kD[2] * transp); 
					}else{
						collided = true;
						ray.length = len;
						refRev = refRevNow;
					}
				}
			}

			if(collided){break;}
			if(found != -1)
			{
				indexSubTree = nodes[indexNodeCurrent].neighbors[found];
				pointIn[0] = pointOut[0];
				pointIn[1] = pointOut[1];
				pointIn[2] = pointOut[2];
			}else{
				break;
			}
		}
	}

	if(!collided)
	{
		float finalCol[3];
		
		finalCol[0] = clip(ray.color[0] * ray.contribution[0]);
		finalCol[1] = clip(ray.color[1] * ray.contribution[1]);
		finalCol[2] = clip(ray.color[2] * ray.contribution[2]);

		int offset = 3 * ray.offset;
		
		atomicAdd(colorBuffer + offset		, finalCol[0]);
		atomicAdd(colorBuffer + offset + 1	, finalCol[1]);
		atomicAdd(colorBuffer + offset + 2	, finalCol[2]);
		return;
	}

	if(ray.type == RAY_SHADOW){return;}
	
	RayCompact recursiveRay;
	float vDir[3];
	float atten;
	float specularDecrease;
	float d;
	if(ray.depth < MAX_DEPTH)
	{
		//SHADOW
		if(ray.orientation > 0.0f)
		{
			for(int i = 0; i < lightCount; i++)
			{
				recursiveRay.start[0] = impact[0];
				recursiveRay.start[1] = impact[1];
				recursiveRay.start[2] = impact[2];
				recursiveRay.end[0] = lights[i].pos[0];
				recursiveRay.end[1] = lights[i].pos[1];
				recursiveRay.end[2] = lights[i].pos[2];
				vDir[0] = recursiveRay.end[0] - recursiveRay.start[0];
				vDir[1] = recursiveRay.end[1] - recursiveRay.start[1];
				vDir[2] = recursiveRay.end[2] - recursiveRay.start[2];
				recursiveRay.length = length(vDir);
				normalize(vDir);
				recursiveRay.start[0] += vDir[0] * STEP_SIZE;
				recursiveRay.start[1] += vDir[1] * STEP_SIZE;
				recursiveRay.start[2] += vDir[2] * STEP_SIZE;
				atten = lights[i].intensity/ ((lights[i].a2 * recursiveRay.length + lights[i].a1) * recursiveRay.length + lights[i].a0);

				specularDecrease = pow(getSpecularDecrease(dir, vDir, normal), materials[indexMat].shine);       
				recursiveRay.color[0] = ( lights[i].col[0] * materials[indexMat].diff[0] + materials[indexMat].kS[0] * specularDecrease) * atten;
				recursiveRay.color[1] = ( lights[i].col[1] * materials[indexMat].diff[1] + materials[indexMat].kS[1] * specularDecrease) * atten;
				recursiveRay.color[2] = ( lights[i].col[2] * materials[indexMat].diff[2] + materials[indexMat].kS[2] * specularDecrease) * atten;
				
				recursiveRay.depth = ray.depth;
				recursiveRay.offset = ray.offset;
				recursiveRay.orientation = 1.0f;
				recursiveRay.type = RAY_SHADOW;
				
				d = clip(dot(vDir, normal)) * materials[indexMat].transparency;
				
				recursiveRay.contribution[0] = ray.contribution[0] * d * materials[indexMat].kD[0];
				recursiveRay.contribution[1] = ray.contribution[1] * d * materials[indexMat].kD[1];
				recursiveRay.contribution[2] = ray.contribution[2] * d * materials[indexMat].kD[2];
				
				bool no = false;
				executeNeighbor(recursiveRay, nodes, spheres, triangles, materials, lights, lightCount, indicesTri, indicesSph, colorBuffer, rays, stackInfo, no);
			}
		}

		//REFRACTED
		bool TIR = false;
		if(transp < 1.0f)
		{
			float dotP = dot(dir,normal);
			float tmpNorm[3];
			float ref;
			tmpNorm[0] = normal[0];
			tmpNorm[1] = normal[1];
			tmpNorm[2] = normal[2];

			if(dotP < 0)
			{
				ref = 1.0f/materials[indexMat].refraction;
			}else{
				ref = materials[indexMat].refraction;
				dotP = -dotP;
				tmpNorm[0] = -tmpNorm[0];
				tmpNorm[1] = -tmpNorm[1];
				tmpNorm[2] = -tmpNorm[2];
			}

			float s2 = (ref*ref)*(1-(dotP*dotP));
			float sqr = 1 - s2;
			if(sqr < 0.0f)
			{
				TIR = true;
			}else{
				sqr = -ref * dotP - sqrt(sqr);
				
				vDir[0] = dir[0] * ref + sqr * tmpNorm[0];
				vDir[1] = dir[1] * ref + sqr * tmpNorm[1];
				vDir[2] = dir[2] * ref + sqr * tmpNorm[2];
				
				unsigned int bufferAlloc = atomicInc(&stackInfo, UINT_MAX);
				rays[bufferAlloc].start[0] = impact[0] + STEP_SIZE * vDir[0];
				rays[bufferAlloc].start[1] = impact[1] + STEP_SIZE * vDir[1];
				rays[bufferAlloc].start[2] = impact[2] + STEP_SIZE * vDir[2];
				rays[bufferAlloc].end[0] = impact[0] + vDir[0];
				rays[bufferAlloc].end[1] = impact[1] + vDir[1];
				rays[bufferAlloc].end[2] = impact[2] + vDir[2];
				rays[bufferAlloc].color[0] = DEFAULT_COL_R;
				rays[bufferAlloc].color[1] = DEFAULT_COL_G;
				rays[bufferAlloc].color[2] = DEFAULT_COL_B;
				rays[bufferAlloc].contribution[0] = ray.contribution[0];
				rays[bufferAlloc].contribution[1] = ray.contribution[1];
				rays[bufferAlloc].contribution[2] = ray.contribution[2];

				rays[bufferAlloc].length = MAX_DIST;

				rays[bufferAlloc].depth = ray.depth + 1;
				rays[bufferAlloc].offset = ray.offset;
				rays[bufferAlloc].orientation = ((refRev) ? -1.0f : 1.0f) * ray.orientation;
				rays[bufferAlloc].type = RAY_REFRACTION;
			}
		}
	
		//REFLECTED
		float refl = materials[indexMat].reflection;
		if(refl > 0.0f)
		{
			float reflected[3];
			reflect(dir, normal, reflected);

			
			ray.start[0]	= impact[0] + STEP_SIZE * reflected[0];
			ray.start[1]	= impact[1] + STEP_SIZE * reflected[1];
			ray.start[2]	= impact[2] + STEP_SIZE * reflected[2];
			ray.end[0]		= impact[0] + reflected[0];
			ray.end[1]		= impact[1] + reflected[1];
			ray.end[2]		= impact[2] + reflected[2];

			ray.color[0]	= DEFAULT_COL_R;
			ray.color[1]	= DEFAULT_COL_G;
			ray.color[2]	= DEFAULT_COL_B;

			ray.contribution[0]	= ray.contribution[0] * materials[indexMat].kS[0] * refl * ((TIR)?1.0f:transp);
			ray.contribution[1]	= ray.contribution[1] * materials[indexMat].kS[1] * refl * ((TIR)?1.0f:transp);
			ray.contribution[2]	= ray.contribution[2] * materials[indexMat].kS[2] * refl * ((TIR)?1.0f:transp);

			ray.length = MAX_DIST;

			ray.depth = ray.depth + 1;
			ray.offset = ray.offset;
			ray.orientation = 1.0f;
			ray.type = RAY_REFLECTION;
			goDeeper = true;
		}
	}
}

/**
Kernel that uses executeBasic (basic KDTree traversing)

/param[in] maxRays max size of ray buffer
/param[in] initRays count of rays at beginning
/param[in] nodes compacted KD tree
/param[in] spheres sphere buffer
/param[in] triangles triangle buffer
/param[in] materials material buffer
/param[in] lights light buffer
/param[in] lightCount how many lights are there
/param[in] indicesTri buffer of triangle indices
/param[in] indicesSph buffer of sphere indices
/param[in] colorBuffer output color buffer
/param[in] rays global buffer of rays
*/
__global__ void kernelGPUBasic(	int maxRays,int initRays,KDTreeNodeCompact *nodes,SphereCompact *spheres,TriangleCompact *triangles,MaterialCompact *materials,PointLightCompact *lights,int lightCount,int *indicesTri,int *indicesSph,float *colorBuffer,	RayCompact *rays)
{

	__shared__ unsigned int STACK_INFO;
	__shared__ unsigned int STACK_NEXT_BATCH_SIZE;

	RayCompact ray_copy;
	SphereCompact sphereC;
	TriangleCompact triangleC;
	int id = threadIdx.x;
	
	// first thread sets shared info (how much rays is in buffer)
	if(id == 0)
	{
		STACK_INFO = initRays;
	}
	__syncthreads();
	int temp;
	int currentRay;
	unsigned int bufferAlloc;

	//main loop
	while(STACK_INFO > 0)
	{
		//first thread bites off part of buffer and tells how many thread is supposed to execute tasks
		if(id == 0)
		{
			temp = STACK_INFO;
			if(temp > blockDim.x)
			{
				//all threads will work
				temp -= blockDim.x;
				STACK_INFO = temp;
				STACK_NEXT_BATCH_SIZE = blockDim.x;
			}else{
				// not enough rays, not all threads will work
				STACK_INFO = 0;
				STACK_NEXT_BATCH_SIZE = temp;
			}
		}
		__syncthreads();

		//reading phase
		if(id < STACK_NEXT_BATCH_SIZE)
		{
			currentRay = STACK_INFO + id;
			copyRay(rays[currentRay],ray_copy);
		}
		
		__syncthreads();

		if(id < STACK_NEXT_BATCH_SIZE)
		{
			//working phase
			bool goAgain = true;
			while(goAgain)
			{
				goAgain = false;
				executeBasic(ray_copy,nodes,spheres,triangles,materials,lights,lightCount,indicesTri,indicesSph,colorBuffer,rays,STACK_INFO,goAgain);
			}
		}
		
		__syncthreads();
	}
}
/**
Kernel that uses executeNeighbor (advanced KDTree traversing)

/param[in] maxRays max size of ray buffer
/param[in] initRays count of rays at beginning
/param[in] nodes compacted KD tree
/param[in] spheres sphere buffer
/param[in] triangles triangle buffer
/param[in] materials material buffer
/param[in] lights light buffer
/param[in] lightCount how many lights are there
/param[in] indicesTri buffer of triangle indices
/param[in] indicesSph buffer of sphere indices
/param[in] colorBuffer output color buffer
/param[in] rays global buffer of rays

for more commented version check kernelGPUBasic, there are just a few changes.
*/
__global__ void kernelGPUNeighbor(	int maxRays,int initRays,KDTreeNodeCompact *nodes,SphereCompact *spheres,TriangleCompact *triangles,MaterialCompact *materials,PointLightCompact *lights,int lightCount,int *indicesTri,int *indicesSph,float *colorBuffer,	RayCompact *rays)
{
	__shared__ unsigned int STACK_INFO;
	__shared__ unsigned int STACK_NEXT_BATCH_SIZE;

	RayCompact ray_copy;
	SphereCompact sphereC;
	TriangleCompact triangleC;
	int id = threadIdx.x;

	// first thread sets shared info
	if(id == 0)
	{
		STACK_INFO = initRays;
	}
	__syncthreads();
	int temp;
	int currentRay;
	unsigned int bufferAlloc;

	//main loop
	while(STACK_INFO > 0)
	{
		//first thread bites off part of buffer and tells how many thread is supposed to execute tasks
		if(id == 0)
		{
			temp = STACK_INFO;
			if(temp > blockDim.x)
			{
				temp -= blockDim.x;
				STACK_INFO = temp;
				STACK_NEXT_BATCH_SIZE = blockDim.x;
			}else{
				STACK_INFO = 0;
				STACK_NEXT_BATCH_SIZE = temp;
			}
		}
		__syncthreads();

		//reading phase
		if(id < STACK_NEXT_BATCH_SIZE)
		{
			currentRay = STACK_INFO + id;
			copyRay(rays[currentRay],ray_copy);
		}
		
		__syncthreads();

		if(id < STACK_NEXT_BATCH_SIZE)
		{
			bool goAgain = true;
			while(goAgain)
			{
				goAgain = false;
				executeNeighbor(ray_copy,nodes,spheres,triangles,materials,lights,lightCount,indicesTri,indicesSph,colorBuffer,rays,STACK_INFO,goAgain);
			}
		}
		
		__syncthreads();
	}
}
/**
Clips float to values from 0.0f to 1.0f
*/
float clip0To1(float f)
{
	if(f > 1.0f){return 1.0f;}
	if(f < 0.0f){return 0.0f;}
	return f;
}
/**
Copy values from GLOBAL_PIXEL_BUFFER to GLOBAL_BITMAP
*/
void pixelBufferToBitmap()
{
	int h = GLOBAL_BITMAP->height;
	int w = GLOBAL_BITMAP->width;
	int offset;
	for(int i = 0; i < h*w; i+=1)
	{
		offset = 3*i;
		GLOBAL_BITMAP->pixels[i].x = 255 * clip0To1(GLOBAL_PIXEL_BUFFER[offset]);
		GLOBAL_BITMAP->pixels[i].y = 255 * clip0To1(GLOBAL_PIXEL_BUFFER[offset + 1]);
		GLOBAL_BITMAP->pixels[i].z = 255 * clip0To1(GLOBAL_PIXEL_BUFFER[offset + 2]);
	}
}
/**
Method handling keyboard events.
*/
static void HandleKeys(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      //exit(0);
		break;
  }
}

void idleFunc() {
  glutPostRedisplay();
}
/**
Build scene chosen by user
*/
void buildSceneTree()
{
	switch(INPUT_SCENE)
	{
	case 0:
		buildTestScene1();
		break;
	case 1:
		buildTestScene2();
		break;
	case 2:
		buildTestScene3();
		break;
	case 3:
		buildTestScene4();
		break;
	case 4:
		buildTestScene5();
		break;
	case 5:
		buildTestBall();
		break;
	case 6:
		buildMassiveScene1();
		break;
	case 7:
		buildMassiveScene2();
		break;
	case 8:
		buildMassiveScene3();
		break;
	}
}
/**
Prepare and execute raytracing on GPU.
*/
void rayTraceSceneGPU()
{
	
	if(GLOBAL_BITMAP == NULL){printf("bMap not initialized\n"); return;}
	if(GLOBAL_BITMAP->pixels == NULL){printf("pixels not initialized\n"); return;}
	
	int countPixels = GLOBAL_BITMAP->width * GLOBAL_BITMAP->height;
	GLOBAL_PIXEL_BUFFER = new float[countPixels*3];
	int bufferSize = (int)(BUFFER_EXTENSION * countPixels);
	GLOBAL_RAYBUFFER = new RayCompact[bufferSize];

	cam.buildRays(*GLOBAL_BITMAP,GLOBAL_RAYBUFFER,bufferSize);
	
	//cam.buildCentralRay(*GLOBAL_BITMAP,GLOBAL_RAYBUFFER,1);
	//countPixels = 1;

	cudaInitialize(bufferSize, countPixels);

	memset(GLOBAL_PIXEL_BUFFER, 0, countPixels * 3 * sizeof(float));
	
	cudaMemcpy(CUDA_Nodes,			GLOBAL_COMPACT.nodes,			GLOBAL_COMPACT.countNode*sizeof(KDTreeNodeCompact), cudaMemcpyHostToDevice);
	cudaMemcpy(CUDA_Spheres,		GLOBAL_COMPACT.sphere,			GLOBAL_COMPACT.countSph*sizeof(SphereCompact),		cudaMemcpyHostToDevice);
	cudaMemcpy(CUDA_Triangles,		GLOBAL_COMPACT.tri,				GLOBAL_COMPACT.countTri*sizeof(TriangleCompact),	cudaMemcpyHostToDevice);
	cudaMemcpy(CUDA_Materials,		GLOBAL_COMPACT.mat,				GLOBAL_COMPACT.countMat*sizeof(MaterialCompact),	cudaMemcpyHostToDevice);
	cudaMemcpy(CUDA_Lights,			GLOBAL_COMPACT.pLi,				GLOBAL_COMPACT.countPLi*sizeof(PointLightCompact),	cudaMemcpyHostToDevice);
	cudaMemcpy(CUDA_Indices_Tri,	GLOBAL_COMPACT.trinagleIndices, GLOBAL_COMPACT.triangleIndicesSize*sizeof(int),		cudaMemcpyHostToDevice);
	cudaMemcpy(CUDA_Indices_Sph,	GLOBAL_COMPACT.sphereIndices,	GLOBAL_COMPACT.sphereIndicesSize*sizeof(int),		cudaMemcpyHostToDevice);
	cudaMemcpy(CUDA_ColorBuffer,	GLOBAL_PIXEL_BUFFER,			countPixels*3*sizeof(float),						cudaMemcpyHostToDevice);
	cudaMemcpy(CUDA_Rays,			GLOBAL_RAYBUFFER,				bufferSize*sizeof(RayCompact),						cudaMemcpyHostToDevice);
	
	

	dim3 blocks(1, 1);
	dim3 threads(512, 1);
	
	//printCompactTree();
	switch(INPUT_TRAVERSING)
	{
	case 2:
		kernelGPUNeighbor<<<blocks,threads>>>(bufferSize,countPixels,CUDA_Nodes,CUDA_Spheres,CUDA_Triangles,CUDA_Materials,CUDA_Lights,GLOBAL_COMPACT.countPLi,CUDA_Indices_Tri,CUDA_Indices_Sph,CUDA_ColorBuffer,CUDA_Rays);
		break;
	default:
		kernelGPUBasic<<<blocks,threads>>>(bufferSize,countPixels,CUDA_Nodes,CUDA_Spheres,CUDA_Triangles,CUDA_Materials,CUDA_Lights,GLOBAL_COMPACT.countPLi,CUDA_Indices_Tri,CUDA_Indices_Sph,CUDA_ColorBuffer,CUDA_Rays);
		break;
	}
	//ker<<<blocks, threads>>>();
	cudaMemcpy(GLOBAL_PIXEL_BUFFER, CUDA_ColorBuffer,				countPixels*3*sizeof(float),						cudaMemcpyDeviceToHost);
	pixelBufferToBitmap();
	cudaFree();


}
/**
Prepare and execute raytracing on CPU
*/
void raytraceSceneCPU()
{
	if(GLOBAL_BITMAP == NULL){printf("bMap not initialized\n"); return;}
	if(GLOBAL_BITMAP->pixels == NULL){printf("pixels not initialized\n"); return;}

	
	GLOBAL_BITMAP->clear();
	cam.buildRays(*GLOBAL_BITMAP,buffer,GLOBAL_BUFFER_LIGHT);

	RayLink *t;
	t = buffer.start->next;
	switch(INPUT_TRAVERSING)
	{
		case 2:
			while(buffer.size > 0)
			{
				buffer.size--;
				t->task->executeNeighbor(GLOBAL_TREE);
				t = t->next;
			}
			break;
		default:
			while(buffer.size > 0)
			{
				buffer.size--;
				t->task->executeBasic(GLOBAL_TREE);
				t = t->next;
			}
			break;
	}
	printf("drawn\n");
	buffer.clear();
}
/**
Read initial user inputs
*/
void readInputs()
{	
	printf("\nWhich method of rendering? \n0) - CPU \n1) - GPU\n-> ");
	std::cin >> INPUT_METHOD;
	if(INPUT_METHOD < 0 || INPUT_METHOD > 1){exit(1);}
	printf("\nWhich method of traversing? \n0) naive (no KDTree) \n1) - always from root \n2) - next cell\n-> ");
	std::cin >> INPUT_TRAVERSING;
	if(INPUT_TRAVERSING < 0 || INPUT_TRAVERSING > 2){exit(1);}
	if(INPUT_TRAVERSING == 0)
	{
		BUILD_KDTREE = false;
	}
	printf("\nSelect scene (0-8)\n 0 - basic\n 1-insane cornel\n 2-2*2*4 balls\n 3-triangle, just triangle\n 4-nearly purple plane\n 5-basic lens\n 6-ball-o-calypse\n 7-...\n 8-...\n-> ");
	std::cin >> INPUT_SCENE;
	printf("\n");
	if(INPUT_SCENE < 0 || INPUT_SCENE > 8){exit(1);}

}
/**
Main
*/
int main(int argc, char **argv) {
	
	readInputs();

	printf("initializing\n");
  initializeBitmap();
  initializeLight();
  Material::initializeMaterials();
    
  buildSceneTree();
  compactizeCurrentTree();

  printf("\n\n");
  //GLOBAL_TREE.printNeighbors();
  //printCompact();
	printf("begin raytracing\n");
	long long timestampBeforeRender = getTime();
	switch(INPUT_METHOD)
	{
	case 0:
		raytraceSceneCPU();
		break;
	case 1:
		rayTraceSceneGPU();
		break;
	default:
		printf(" %d is not valid method for rendering\n", INPUT_METHOD);
		exit(1);
	}
	long long timestampAfterRender = getTime();
	printf(" rendering took %ld ms\n",(timestampAfterRender- timestampBeforeRender));
	printf("finished raytracing\n");

  //printf("rays - created: %d executed: %d \n", countRayCreated, countRayExecuted);

  glutInit(&argc, argv);

  glutInitWindowSize(IMAGE_WIDTH, IMAGE_HEIGHT);
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );

  glutCreateWindow("RayTracer");
  glutDisplayFunc(DisplayFunc);
  glutKeyboardFunc(HandleKeys);

  glutIdleFunc(idleFunc);

  glutMainLoop();

  return 0;
}