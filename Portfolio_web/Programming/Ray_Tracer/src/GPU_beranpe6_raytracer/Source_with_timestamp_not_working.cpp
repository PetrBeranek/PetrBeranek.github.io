

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

#ifndef TEST_VAR
#define TEST_VAR

#define IMAGE_WIDTH  600
#define IMAGE_HEIGHT 400

RayBuffer buffer;

Bitmap *bMap = NULL;
//extern Sphere testSphere;
PinholeCamera cam;

LightCol lights;
KDTree sceneTree;

int countRayCreated = 0;
int countRayExecuted = 0;


//extern int countRayCreated = 0;
//extern int countRayExecuted = 0;

#endif


void testBitmap();

/*typedef struct Bitmap {
  int width;			
  int height;			
  uchar4 *pixels;	
};*/


void normalize(float3 &vec)
{
	float m = sqrt((vec.x*vec.x) + (vec.y*vec.y) + (vec.z*vec.z));
}

long long getTime()
{
    SYSTEMTIME t;
    GetSystemTime(&t);

	long long time = ((((((t.wHour * 60) + t.wMinute) * 60) + t.wSecond) * 1000) + t.wMilliseconds);
	return time;
}

void DisplayFunc( void ) {
  glClearColor( 0.0, 1.0, 0.0, 1.0 );
  glClear( GL_COLOR_BUFFER_BIT );
  
  //testBitmap();

  if(bMap != NULL ){
	glDrawPixels( bMap->width, bMap->height, GL_RGBA, GL_UNSIGNED_BYTE, bMap->pixels );
  }

  glutSwapBuffers();
}

void initializeLight()
{
	lights = LightCol();
}

void initializeBitmap()
{
	bMap = new Bitmap(IMAGE_WIDTH, IMAGE_HEIGHT);
	printf("initialized\n");

	//Sphere s = Sphere();
}

void buildTestScene1()
{
	float w,h,f;
	w = 9.0f;
	h = 6.0f;
	f = 6.0f;

	cam = PinholeCamera(Vec3(0.0f,0.0f,0.0f), Vec3(0.0f,0.0f,-5.0f), Vec3(0.0f,1.0f,0.0f),f,w,h);
	buffer = RayBuffer();
	PointLight *l;
	l = new PointLight(Vec3(1.0f,0.0f,1.0f),Col(1.0f,1.0f,1.0f), 15, 1, 1, 1);
	lights.addLight(*l);
	l = new PointLight(Vec3(-3.0f,3.0f,-1.0f),Col(1.0f,1.0f,1.0f), 10, 1, 1, 1);
	lights.addLight(*l);
	l = new PointLight(Vec3(2.0f,0.0f,0.0f),Col(0.30f,0.30f,0.30f), 4, 1, 1, 1);
	lights.addLight(*l);
	


	std::vector<Collidable *> *vec = new std::vector<Collidable *>();

	Collidable *c;

	c = new Sphere(Vec3(0.0f, .5f, -4.20f), 1.0f, Material(1));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(-0.0f, -1.0f, -3.0f), 0.5f, Material(2));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(-0.0f, 0.00f, -0.50f), 0.1f, Material(3));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(0.7f, 0.30f, -2.5f), 0.5f, Material(4));
	c->precomputeBounds();
	//vec->push_back(c);
	c = new Sphere(Vec3(02.0f, 0.00f, -5.0f), 0.70f, Material(4));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Sphere(Vec3(0.9f, 0.00f, -02.50f), 0.50f, Material(7));
	c->precomputeBounds();
	vec->push_back(c);
	

	sceneTree.buildTreeStart(*vec);

	
	sceneTree.isLeaf = true;
	sceneTree.items = vec;
	sceneTree.xMin = -100;
	sceneTree.xMax = 100;
	sceneTree.yMin = -100;
	sceneTree.yMax = 100;
	sceneTree.zMin = -100;
	sceneTree.zMax = 100;

	sceneTree.axis = -1;
	sceneTree.depth = 0;
	sceneTree.id = 55;
	
}
void buildTestScene2()
{
	float w,h,f;
	w = 9.0f;
	h = 6.0f;
	f = 3.0f;
	
	float con = 2;

	cam = PinholeCamera(Vec3(0.0f,0.0f,0.0f), Vec3(0.0f,0.0f,-5.0f), Vec3(0.0f,1.0f,0.0f),f,w,h);
	buffer = RayBuffer();
	PointLight *l;
	l = new PointLight(Vec3(0.0f, con-0.10f, -con),Col(1.0f,1.0f,1.0f), 15, 1, 1, 1);
	lights.addLight(*l);
	

	std::vector<Collidable *> *vec = new std::vector<Collidable *>();

	Collidable *c;
	
	c = new Sphere(Vec3(0.0f, 0.0f, -2.0f), 0.50f, Material(3));
	c->precomputeBounds();
	vec->push_back(c);

	c = new Sphere(Vec3(-1.0f, -1.0f, -3.0f), 1.0f, Material(1));
	c->precomputeBounds();
	vec->push_back(c);


	//left
	c = new Triangle( Vec3(-con,-con,0), Vec3(-con,-con,-2*con), Vec3(-con,con,0), Material(8));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Triangle( Vec3(-con,-con,-2*con), Vec3(-con,con,-2*con), Vec3(-con,con,0), Material(8));
	c->precomputeBounds();
	vec->push_back(c);
	//right
	c = new Triangle( Vec3(con,-con,0), Vec3(con,con,0), Vec3(con,-con,-2*con), Material(9));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Triangle( Vec3(con,-con,-2*con), Vec3(con,con,0), Vec3(con,con,-2*con), Material(9));
	c->precomputeBounds();
	vec->push_back(c);
	//back
	c = new Triangle( Vec3(-con,con,-2*con), Vec3(-con,-con,-2*con), Vec3(con,-con,-2*con), Material(10));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Triangle( Vec3(-con,con,-2*con), Vec3(con,-con,-2*con), Vec3(con,con,-2*con), Material(10));
	c->precomputeBounds();
	vec->push_back(c);
	//bottom
	c = new Triangle( Vec3(-con,-con,0), Vec3(con,-con,0), Vec3(con,-con,-2*con), Material(10));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Triangle( Vec3(-con,-con,-2*con), Vec3(-con,-con,0), Vec3(con,-con,-2*con), Material(10));
	c->precomputeBounds();
	vec->push_back(c);
	//top
	c = new Triangle( Vec3(-con,con,0), Vec3(con,con,-2*con), Vec3(con,con,0), Material(10));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Triangle( Vec3(-con,con,-2*con), Vec3(con,con,-2*con), Vec3(-con,con,0), Material(10));
	c->precomputeBounds();
	vec->push_back(c);

	
	//front
	c = new Triangle( Vec3(-con,con,con), Vec3(con,-con,con), Vec3(-con,-con,con), Material(2));
	c->precomputeBounds();
	vec->push_back(c);
	c = new Triangle( Vec3(-con,con,con), Vec3(con,con,con), Vec3(con,-con,con), Material(2));
	c->precomputeBounds();
	vec->push_back(c);
	

	sceneTree.buildTreeStart(*vec);

	
	sceneTree.isLeaf = true;
	sceneTree.items = vec;
	sceneTree.xMin = -100;
	sceneTree.xMax = 100;
	sceneTree.yMin = -100;
	sceneTree.yMax = 100;
	sceneTree.zMin = -100;
	sceneTree.zMax = 100;

	sceneTree.axis = -1;
	sceneTree.depth = 0;
	sceneTree.id = 55;
	
}


void drawTestScene()
{
	bMap->clear();
	cam.buildRays(*bMap,buffer,lights);
	//cam.buildCentralRay(bMap,&buffer,lights);
	RayLink *t;
	t = buffer.start->next;
	while(buffer.size > 0)
	{
		buffer.size--;
		t->task->execute(sceneTree);
		t = t->next;
		//delete t->task;

		/*
		t = buffer.getFront();
		t->task->execute(sceneTree);
		buffer.deleteFront();
		*/
		countRayExecuted++;
	}
	/*
	unsigned char r, g, b;
	
	for(int i = 0; i < IMAGE_HEIGHT; i++)
		for(int j = 0; j < IMAGE_WIDTH; j++)
		{
			r = ((i/3)%15 <= 2) ? 0 : 255;
			g = ((j/3)%15 <= 2) ? 0 : 255;
			b = ((i/3)%15 < (j/3)%15) ? 0 : 255;
			bMap->setBMapPixel(i, j, r, g, b);
		}
	*/

	printf("drawn\n");
	buffer.clear();
}

void raytraceScene()
{
	if(bMap == NULL){printf("bMap not initialized\n"); return;}
	if(bMap->pixels == NULL){printf("pixels not initialized\n"); return;}

	drawTestScene();
}

void testRay()
{
	
	KDTree *tr = new KDTree(-5, 5, -3, 3, -2, 0,    0,0, true);
	Vec3 ori = Vec3(1,1,1);
	Vec3 dir = Vec3(0,2,-1);
	dir.normalizeLoc();
	Vec3 out;
	bool fin;
	fin = tr->findEnterPoint(ori, dir, out);
	printf("en %f %f %f %d\n", out.x, out.y, out.z, (fin)?1:0 );
	fin = tr->findExitPoint(ori, dir, out);
	printf("ex %f %f %f %d\n", out.x, out.y, out.z, (fin)?1:0);
}

void testTree()
{
	std::vector<Collidable *> *vec = new std::vector<Collidable*>();
	Point *p;
	printf("\ntesting tree \n");

	/*
	p = new Point(-4, -1, -3);
	p->precomputeBounds();
	vec->push_back(p);
	p = new Point(-4, -1, 4);
	p->precomputeBounds();
	vec->push_back(p);
	p = new Point(3, 2, -4);
	p->precomputeBounds();
	vec->push_back(p);
	p = new Point(3, 2, 3);
	p->precomputeBounds();
	vec->push_back(p);
	*/

	
	p = new Point(-5, -5, -5);
	p->precomputeBounds();
	vec->push_back(p);
	p = new Point(5, -5, -5);
	p->precomputeBounds();
	vec->push_back(p);
	p = new Point(-5, 5, 5);
	p->precomputeBounds();
	vec->push_back(p);
	p = new Point(5, 5, 5);
	p->precomputeBounds();
	vec->push_back(p);
	p = new Point(-1, -1, -1);
	p->precomputeBounds();
	vec->push_back(p);
	p = new Point(1, -1, -1);
	p->precomputeBounds();
	vec->push_back(p);
	p = new Point(-1, 1, 1);
	p->precomputeBounds();
	vec->push_back(p);
	p = new Point(1, 1, 1);
	p->precomputeBounds();
	vec->push_back(p);

	KDTree *tr = new KDTree();
	tr->buildTreeStart(*vec);
	//printf("printing \n");
	tr->printTreeMod();
	printf("tree tested \n");


	bMap->clear();
	cam.buildCentralRay(*bMap,buffer,lights);
	RayLink *t;
	t = buffer.start->next;
	t->task->execute(*tr);

}

static void HandleKeys(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      //exit(0);
		break;
  }
}

void idleFunc() {
  // prekresleni
  glutPostRedisplay();
}

int main(int argc, char **argv) {

	printf("initializing\n");
  initializeBitmap();
  initializeLight();
    
  //testRay();
  //testTree();
  //buildTestScene1();
  buildTestScene2();

	printf("begin raytracing\n");
	long long timestampBeforeRender = getTime();
  raytraceScene();
	long long timestampAfterRender = getTime();
	printf(" rendering took %ld ms\n",(timestampAfterRender- timestampBeforeRender));
	printf("finished raytracing\n");

  printf("rays - created: %d executed: %d \n", countRayCreated, countRayExecuted);

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