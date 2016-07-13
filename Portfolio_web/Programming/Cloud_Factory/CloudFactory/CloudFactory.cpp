#include <iostream>
#include <time.h>
#include "pgr.h"
#include "Consts.h"
#include "ModelManager.h"
#include "CloudFactory.h"
#include "Scene.h"
#include "Loader.h"


namespace beranpe6
{
/// width of window
int winW;
/// height of window
int winH;

/// varaible contains actual X position of cursor
float mouseX;
/// varaible contains actual Y position of cursor
float mouseY;

/// time elapsed from aplication start
float timeElapsed;
/// time elapsed from last frame
float timeDelta;

bool key_A = false;
bool key_D = false;
bool key_W = false;
bool key_S = false;
bool key_Space = false;
bool key_C = false;
bool key_Q = false;
bool key_E = false;

/// active camera in application
CameraObject* camera;
GameObjects gameObjects;
/// active scene
Scene* scene;
/// list of stars
MyList stars;
/// basic shader
extern SCommonShaderProgram shaderProgram;

/// predefined positions of camera
CameraData *cameraBasic;
CameraData *cameraStatic1;
CameraData *cameraStatic2;

void drawStars( const glm::mat4 view, const glm::mat4 projection, float timeElapsed, glm::mat4 *camTransform){
	for(MyList::iterator it = stars.begin(); it != stars.end(); ++it) {
		Star* s = (Star *)(*it);
		drawStar( view, projection, timeElapsed, s->offset, camTransform, s->transform);
	}
}

void initializeStars() {
	Star *s;
	glm::mat4 *tr;

	s = new Star();
	tr = new glm::mat4(1.0f);
	*tr = glm::translate(*tr,glm::vec3(1.0f, 6.0f, -2.0f));
	*tr = glm::scale(*tr,glm::vec3(0.1f));
	s->transform = tr;
	s->offset = 0.0f;
	stars.push_back(s);
	
	s = new Star();
	tr = new glm::mat4(1.0f);
	*tr = glm::translate(*tr,glm::vec3(4.0f, 6.0f, -1.5f));
	*tr = glm::scale(*tr,glm::vec3(0.2f));
	s->transform = tr;
	s->offset = 1.0f;
	stars.push_back(s);
	
	s = new Star();
	tr = new glm::mat4(1.0f);
	*tr = glm::translate(*tr,glm::vec3(1.0f, 6.0f, 1.5f));
	*tr = glm::scale(*tr,glm::vec3(0.25f));
	s->transform = tr;
	s->offset = 2.0f;
	stars.push_back(s);
	
	s = new Star();
	tr = new glm::mat4(1.0f);
	*tr = glm::translate(*tr,glm::vec3(-2.0f, 6.0f, 0.5f));
	*tr = glm::scale(*tr,glm::vec3(0.08f));
	s->transform = tr;
	s->offset = 3.f;
	stars.push_back(s);
	
	s = new Star();
	tr = new glm::mat4(1.0f);
	*tr = glm::translate(*tr,glm::vec3(0.0f, 6.0f, -9.0f));
	*tr = glm::scale(*tr,glm::vec3(0.3f));
	s->transform = tr;
	s->offset = 4.f;
	stars.push_back(s);
	
	s = new Star();
	tr = new glm::mat4(1.0f);
	*tr = glm::translate(*tr,glm::vec3(-3.0f, 6.0f, 2.5f));
	*tr = glm::scale(*tr,glm::vec3(0.15f));
	s->transform = tr;
	s->offset = 5.f;
	stars.push_back(s);

	s = new Star();
	tr = new glm::mat4(1.0f);
	*tr = glm::translate(*tr,glm::vec3(-1.3f, 6.0f, -1.5f));
	*tr = glm::scale(*tr,glm::vec3(0.1f));
	s->transform = tr;
	s->offset = 6.f;
	stars.push_back(s);
	
	s = new Star();
	tr = new glm::mat4(1.0f);
	*tr = glm::translate(*tr,glm::vec3(-4.3f, 6.0f, -2.5f));
	*tr = glm::scale(*tr,glm::vec3(0.1f));
	s->transform = tr;
	s->offset = 7.f;
	stars.push_back(s);
	
	s = new Star();
	tr = new glm::mat4(1.0f);
	*tr = glm::translate(*tr,glm::vec3(3.3f, 6.0f, -2.5f));
	*tr = glm::scale(*tr,glm::vec3(0.1f));
	s->transform = tr;
	s->offset = 8.f;
	stars.push_back(s);
	
	s = new Star();
	tr = new glm::mat4(1.0f);
	*tr = glm::translate(*tr,glm::vec3(3.3f, 6.0f, -2.5f));
	*tr = glm::scale(*tr,glm::vec3(0.1f));
	s->transform = tr;
	s->offset = 9.f;
	stars.push_back(s);
	
}

void cleanupObjects(){
	cleanupScene(scene);
}

void initializeCameraDatas(){
	cameraBasic = new CameraData();
	cameraBasic->angleX = 0;
	cameraBasic->angleY = 0;
	cameraBasic->angleZ = 0;
	cameraBasic->locX = 0;
	cameraBasic->locY = 0;
	cameraBasic->locZ = 1;
	
	cameraStatic1 = new CameraData();
	cameraStatic1->angleX = 2.f;
	cameraStatic1->angleY = 109.f;
	cameraStatic1->angleZ = 0.f;
	cameraStatic1->locX = 0.467f;
	cameraStatic1->locY = 1.028f;
	cameraStatic1->locZ = 4.695f;
	
	cameraStatic2 = new CameraData();
	cameraStatic2->angleX = -1283.f;
	cameraStatic2->angleY = 164.f;
	cameraStatic2->angleZ = 0.f;
	cameraStatic2->locX = 0.454f;
	cameraStatic2->locY = 2.068f;
	cameraStatic2->locZ =-4.848f;

}

void initializeApplication() {

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	initializeShaderPrograms();
	initializeModels();
	initializeCameraDatas();
	initializeCamera(cameraBasic);
	initializeLights();
	initializeData();
	initializeStars();
	//testStart();

	timeElapsed = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); 

	buildScene();
}

/**
	Append current camera to Dynamic object. Camera will start following that object.
*/
void appendCamera(DynamicObject *obj){
	camera->useDynObject = true;
	camera->obj = obj;
}

void initializeCamera(CameraData *data) {	
	camera = new CameraObject;
	camera->useDynObject = false;
	camera->obj = NULL;

	camera->angleX = data->angleX;
	camera->angleY = data->angleY;
	camera->angleZ = data->angleZ;

	
	glm::mat4 rot = glm::rotate(glm::mat4(1.0),glm::radians(camera->angleZ),glm::vec3(0.0f,0.0f,1.0f));
	rot = glm::rotate(rot,glm::radians(camera->angleY),glm::vec3(1.0f,0.0f,0.0f));
	rot = glm::rotate(rot,glm::radians(camera->angleX),glm::vec3(0.0f,1.0f,0.0f));

	camera->loc = glm::vec3(data->locX,data->locY,data->locZ);
	camera->forward = (glm::vec3)(glm::vec4(0.0f,0.0f,-1.0f,0.0f)*rot);
	camera->up = (glm::vec3)(glm::vec4(0.0f,1.0f,0.0f,0.0f)*rot);
}

void resetCamera(CameraObject *cam) {	
	cam->angleX = 0;
	cam->angleY = 0;
	cam->angleZ = 0;

	
	glm::mat4 rot = glm::rotate(glm::mat4(1.0),glm::radians(camera->angleZ),glm::vec3(0.0f,0.0f,1.0f));
	rot = glm::rotate(rot,glm::radians(cam->angleY),glm::vec3(1.0f,0.0f,0.0f));
	rot = glm::rotate(rot,glm::radians(cam->angleX),glm::vec3(0.0f,1.0f,0.0f));

	cam->loc = glm::vec3(0.0f,0.0f,1.0f);
	cam->forward = (glm::vec3)(glm::vec4(0.0f,0.0f,-1.0f,0.0f)*rot);
	cam->up = (glm::vec3)(glm::vec4(0.0f,1.0f,0.0f,0.0f)*rot);
}

void changeCameraAngles(float dx, float dy){
	camera->angleX += dx;
	camera->angleY += dy;
}

/**
	Check if camera is in bounding box. If not, move camera to border of box.
*/
void clipCamPositionBox(glm::vec3& pos){
	float k = 20.0f;
	glm::vec3 v = glm::vec3(0.0f);

	if(pos.x < -k){
		pos.x = -k;
	}else if(pos.x > k){
		pos.x = k;
	}
	
	if(pos.y < -k){
		pos.y = -k;
	}else if(pos.y > k){
		pos.y = k;
	}
	
	if(pos.z < -k){
		pos.z = -k;
	}else if(pos.z > k){
		pos.z = k;
	}


	//printf("%f %f %f \n",pos.x, pos.y, pos.z);
}

/**
* Check if camera is in bounding sphere. If not, move camera to border of sphere.
*/
void clipCamPositionSphere(glm::vec3& pos){
	float k = 29.0f;
	float f = glm::length(pos);

	if(f > k){
		f = k/f;
		pos = f * pos;
	}

}

/**
	Update camera position using key_* variables (keys S, W, A, D, C and Space)
*/
glm::vec3 updateCameraPosition(CameraObject *cam){
	glm::vec3 vec = glm::vec3(0.0f);
	if(!cam->useDynObject){
		float forward = 0;
		float left = 0;
		float up = 0;
	
		if(key_W != key_S){
			if(key_W){
			//printf("W");
				forward = CAMERA_SPEED;
			}else{
			//printf("S");
				forward = -CAMERA_SPEED;
			}
		}

		if(key_A != key_D){
			if(key_A){
				left = CAMERA_SPEED;
			}else{
				left = -CAMERA_SPEED;
			}
		}
		
		if(key_Space != key_C){
			if(key_Space){
				up = CAMERA_SPEED;
			}else{
				up = -CAMERA_SPEED;
			}
		}
	
		if(key_Q != key_E){
			if(key_Q){
				cam->angleZ -= ROT_Z_PARAM; 
			}else{
				cam->angleZ += ROT_Z_PARAM;
			}
		}
		
		vec += forward*cam->forward;
		vec += up*cam->up;
		vec -= left*glm::cross(cam->forward,cam->up);
		
		cam->loc += vec;
		clipCamPositionSphere(cam->loc);
		clipCamPositionBox(cam->loc);
	}else{
		cam->loc = cam->obj->position;
	}
	return vec;
}

void updateCameraRotation(CameraObject* cam){
	if(cam->useDynObject){

	}else{
		glm::mat4 rot = glm::mat4(1.0);
		rot = glm::rotate(rot,glm::radians(cam->angleY*ROT_Y_PARAM),glm::vec3(1.0f,0.0f,0.0f));
		rot = glm::rotate(rot,glm::radians(cam->angleX*ROT_X_PARAM),glm::vec3(0.0f,1.0f,0.0f));


		cam->forward = (glm::vec3)(glm::vec4(0.0f,0.0f,-1.0f,0.0f)*rot);
		cam->up = (glm::vec3)(glm::vec4(0.0f,1.0f,0.0f,0.0f)*rot);
	}
}

/**
*	Method creates view martix of camera cam.
*/
glm::mat4 getCameraTransformation(CameraObject* cam ){
	glm::mat4 ViewMatrix;
	if(cam->useDynObject){
		glm::mat4 offset = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,0.5f));

		glm::vec3 loc = (glm::vec3)(*cam->obj->transformation * offset * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		glm::vec3 forw = (glm::vec3)(*cam->obj->transformation * offset * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
		glm::vec3 up = (glm::vec3)(*cam->obj->transformation * offset * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
		
		ViewMatrix = glm::lookAt(
		loc,
		loc+forw,
		up
		);
	}else{
		ViewMatrix = glm::lookAt(
		cam->loc,
		cam->loc+cam->forward,
		cam->up
		);
	}
	//printDebug("counting Matrix");

	return ViewMatrix;
}

glm::mat4 getCameraTranslation(CameraObject* cam ){
	
	return glm::translate(glm::mat4(1.0f),cam->loc);
}

void cleanupCameras(){
	
	delete camera;
	delete cameraBasic;
	delete cameraStatic1;
	delete cameraStatic2;
}

void finalizeApplication(void) {

  cleanupObjects();
  cleanupModels();
  cleanupCameras();
  cleanupShaderPrograms();
}

void timerCallback(int){
	GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;;
	glClearColor(0.4f,0.4f,0.7f,1.0f);
	glClear(mask);

	drawWindowContents();

	glutSwapBuffers();
	
	glutTimerFunc(33, timerCallback, 0);
}

void reshapeCallback(int newWidth, int newHeight) {

  winW = newWidth;
  winH = newHeight;

  glViewport(0, 0, (GLsizei) newWidth, (GLsizei) newHeight);
}

void displayCallback() {
  GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

  glClear(mask);

  drawWindowContents();

  glutSwapBuffers();
}

/**
	Process keyboard inputs. (DOWN)
*/
void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY) {
	switch (keyPressed) {
    case 'a':
      key_A = true;
      break;
    case 'd':
      key_D = true;
      break;
    case 'w':
      key_W = true;
      break;
    case 's':
      key_S = true;
      break;
    case ' ':
      key_Space = true;
      break;
    case 'c':
      key_C = true;
      break;
    case 'q':
      key_Q = true;
      break;
    case 'e':
      key_E = true;
      break;
    case 'z':
      break;
    case 'x':
      break;
    default:
		;
  }
}

/**
	Process keyboard inputs.(UP)
*/
void keyboardUpCallback(unsigned char keyPressed, int mouseX, int mouseY) {
	switch (keyPressed) {
    case 'a':
      key_A = false;
      break;
    case 'd':
      key_D = false;
      break;
    case 'w':
      key_W = false;
      break;
    case 's':
      key_S = false;
      break;
    case ' ':
      key_Space = false;
      break;
    case 'c':
      key_C = false;
      break;
    case 'q':
      key_Q = false;
      break;
    case 'e':
      key_E = false;
      break;
	case 'f':
		selection( mouseX, mouseY);
		break;
    case 'o':
		changeFogDensity(0.1f);
      break;
    case 'l':
		changeFogDensity(-0.1f);
      break;
    case 'p':
		setFogDensity(0.0f);
      break;
    case 'r':
		loadConfiguration();
      break;
    default:
		;
  }
}

/**
	Get id of object from stencil buffer at clicked position. Activate scene graph nodes associated with this id.  
*/
void selection(int x, int y){

	unsigned char id;
	
	glReadPixels(mouseX, winH - mouseY-1,1,1,GL_STENCIL_INDEX,GL_UNSIGNED_BYTE,&id);

	printf("selected id: %d \n",id);

	activateNodes(id);

}

/**
	Process special keyboard inputs. (DOWN)
*/
void specialKeyboardCallback(int specKeyPressed, int mouseX, int mouseY) {
		//glm::mat4 m = glm::mat4(1.0f);
	switch (specKeyPressed) {
    case GLUT_KEY_F1:
		initializeCamera(cameraBasic);
      break;
    case GLUT_KEY_F2:
		initializeCamera(cameraStatic1);
      break;
    case GLUT_KEY_F3:
		initializeCamera(cameraStatic2);
      break;
    case GLUT_KEY_F4:
		appendCamera(selectButterfly(0));
		//initializeCamera(cameraStatic2);
      break;
    case GLUT_KEY_F5:
		appendCamera(selectButterfly(1));
      break;
    case GLUT_KEY_F9:
		setShad(0);
      break;
    case GLUT_KEY_F10:
		setShad(1);
      break;
    case GLUT_KEY_F11:
		setShad(2);
      break;
    case GLUT_KEY_F12:

		printf("cam\n");
		printf("angleX %4f angleY %4f\n",camera->angleX,camera->angleY);
		printf("x %4f  y %4f  z %4f  \n",camera->loc.x,camera->loc.y,camera->loc.z);
		printf("\n\n");
      break;
	default:
		;
	}
}

void specialKeyboardUpCallback(int specKeyPressed, int mouseX, int mouseY) {
}

void mouseMovementPas(int x, int y){
	mouseX = (float)x;
	mouseY = (float)y;
}

void mouseMovementAct(int x, int y){
	changeCameraAngles((float)(x-mouseX),(float)(y-mouseY));
	mouseX = (float)x;
	mouseY = (float)y;
}


void mouseClick(int buttonPressed, int buttonState, int mouseX, int mouseY ){
}

void drawWindowContents(){

	timeDelta = 0.001f*(float)glutGet(GLUT_ELAPSED_TIME)-timeElapsed;
	timeElapsed = 0.001f*(float)glutGet(GLUT_ELAPSED_TIME);

	  // setup parallel projection
  glm::mat4 orthoProjectionMatrix = glm::ortho(
    -SCENE_WIDTH, SCENE_WIDTH,
    -SCENE_HEIGHT, SCENE_HEIGHT,
    -20.0f*SCENE_DEPTH, 30.0f*SCENE_DEPTH
  );
  
	updateCameraPosition(camera);
	updateCameraRotation(camera);
  glm::mat4 orthoViewMatrix = getCameraTransformation(camera);

  // setup camera & projection transform
  glm::mat4 viewMatrix  = orthoViewMatrix;
  glm::mat4 projectionMatrix = orthoProjectionMatrix;
  projectionMatrix = glm::perspective(60.0f, winW/(float)winH, 0.1f, 60.0f);

  updateLights();
  updateStaticScene(scene->staticScene,timeDelta);

  
  drawSkybox(viewMatrix, projectionMatrix); 
  drawClouds(viewMatrix, projectionMatrix, timeElapsed, &getCameraTranslation(camera));
  drawStars(viewMatrix, projectionMatrix, timeElapsed, &getCameraTranslation(camera));

  glClear(GL_DEPTH_BUFFER_BIT);

  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

  drawStaticScene(scene->staticScene, &glm::mat4(1.0f),viewMatrix,projectionMatrix);

  updateButterflies(timeElapsed,timeDelta);
  drawButterflies(viewMatrix, projectionMatrix);
  //drawClouds(viewMatrix, projectionMatrix, timeElapsed, &getCameraTranslation(camera));


}

/**
*	Method for debuging
*/
void testStart(){
	if(gameObjects.testObject == NULL){
		gameObjects.testObject = new TestObject;
	}

	gameObjects.testObject->position = glm::vec3(0.0f, 0.0f, 0.0f);
	gameObjects.testObject->viewAngle = 0.0f;
	gameObjects.testObject->direction = glm::vec3(cos(glm::radians(gameObjects.testObject->viewAngle)), sin(glm::radians(gameObjects.testObject->viewAngle)), 0.0f);
	gameObjects.testObject->size = TEST_OBJECT_SIZE;
}

/**
*	Method to buld Scene Graph of bacis scene.
*/
void buildScene(){
	scene = new Scene();
	glm::mat4* matrix = new glm::mat4(1.0f);

	*matrix = glm::scale(*matrix,glm::vec3(1.0f,1.0f,1.0f));

	scene->staticScene = buildTempScene(matrix,0);
}

}

using namespace beranpe6;

int main(int argc, char** argv) {


	// initialize windowing system
	glutInit(&argc, argv);

	glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);

	// initial window size
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow(WINDOW_TITLE);

	glutDisplayFunc(displayCallback);
	// register callback for change of window size
	glutReshapeFunc(reshapeCallback);
	// register callbacks for keyboard
	glutKeyboardFunc(keyboardCallback);
	glutKeyboardUpFunc(keyboardUpCallback);
	glutSpecialFunc(specialKeyboardCallback);     // key pressed
	glutSpecialUpFunc(specialKeyboardUpCallback); // key released
	glutPassiveMotionFunc(mouseMovementPas);
	glutMotionFunc(mouseMovementAct);
	glutMouseFunc(mouseClick);

	glutTimerFunc(33, timerCallback, 0);

	// initialize GL, devil etc.
	if(!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR)){
		pgr::dieWithError("pgr init failed, required OpenGL not supported?");
	}

	initializeApplication();

	glutCloseFunc(finalizeApplication);

	glutMainLoop();

	return 0;
}
