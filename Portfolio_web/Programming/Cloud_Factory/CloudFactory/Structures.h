
//----------------------------------------------------------------------------------------
/**
 * \file       Structures.h
 * \author     Petr Beránek
 * \date       2015/05/09
 * \brief      This file contains most of structures used in CloudFactory application.
 *
 *  This file contains structures for cameras, geometry, shaders, lights, scene objects, scenegraph and for transformations of scenegraph node over time.
 *
*/
//----------------------------------------------------------------------------------------

#ifndef __STRUCTURES_H
#define __STRUCTURES_H


#include "pgr.h"
#include <list>

namespace beranpe6
{

/// Structure for quick setup of camera to predefined place/rotation
struct CameraData {
	float angleX;
	float angleY;
	float angleZ;

	
	float locX;
	float locY;
	float locZ;
};

/// Structure representing triangle mesh with material
typedef struct MeshGeometry {
  GLuint        vertexBufferObject;   // identifier for the vertex buffer object
  GLuint        elementBufferObject;  // identifier for the element buffer object
  GLuint        vertexArrayObject;    // identifier for the vertex array object
  unsigned int  numTriangles;         // number of triangles in the mesh
  // material
  glm::vec3     ambient;
  glm::vec3     diffuse;
  glm::vec3     specular;
  float         shininess;
  GLuint        texture;

};

/// Structure for test object (Wrench)
typedef struct TestObject {
  glm::vec3 position;
  glm::vec3 direction;
  float     size;

  float viewAngle; // in degrees

};

struct GameObjects {
  TestObject *testObject;
};

/// directional light
typedef struct LightDir {
  glm::vec3 diffuse;
  glm::vec3 ambient;
  glm::vec3 specular;
  glm::vec3 position;
  
  glm::mat4 transformation;
  bool transformable;

};

/// point light
typedef struct LightPoint {
  glm::vec3 diffuse;
  glm::vec3 ambient;
  glm::vec3 specular;
  glm::vec3 position;
  float fallOff;

};

typedef struct Fog {
  glm::vec4 fogColor;
  float fogDensity;
};

/// basic shader used for objects
typedef struct SCommonShaderProgram {
  // identifier for the program
  GLuint program;          // = 0;
  // vertex attributes locations
  GLint posLocation;       // = -1;
  GLint normalLocation;    // = -1;
  GLint texCoordLocation;  // = -1;
  // uniforms locations
  GLint PVMmatrixLocation;    // = -1;
  GLint VmatrixLocation;      // = -1;  view/camera matrix
  GLint MmatrixLocation;      // = -1;  modeling matrix
  GLint normalMatrixLocation; // = -1;  inverse transposed VMmatrix

  GLint timeLocation;         // = -1; elapsed time in seconds

  // material 
  GLint diffuseLocation;    // = -1;
  GLint ambientLocation;    // = -1;
  GLint specularLocation;   // = -1;
  GLint shininessLocation;  // = -1;
  
  // lightDir1
  GLint lightDirDiffuseLocation1;
  GLint lightDirAmbientLocation1;
  GLint lightDirSpecularLocation1;
  GLint lightDirPositionLocation1;
  // lightDir2
  GLint lightDirDiffuseLocation2;
  GLint lightDirAmbientLocation2;
  GLint lightDirSpecularLocation2;
  GLint lightDirPositionLocation2;
  // lightPoint
  GLint lightPointDiffuseLocation;
  GLint lightPointAmbientLocation;
  GLint lightPointSpecularLocation;
  GLint lightPointPositionLocation;
  GLint lightPointFallOffLocation;
  
  GLint fogColorLocation;
  GLint fogDensityLocation;

  // texture
  GLint useTextureLocation; // = -1; 
  GLint texSamplerLocation; // = -1;
  // reflector related uniforms
  //GLint reflectorPositionLocation;  // = -1; 
  //GLint reflectorDirectionLocation; // = -1;
};

/// shader for skybox
struct SkyboxShaderProgram {
  GLuint program;             
  GLint screenCoordLocation;   
  GLint inversePVmatrixLocation; 
  GLint skyboxSamplerLocation;  
  
  GLint fogColorLocation;
  GLint fogDensityLocation;
};

/// shader for animated clouds
struct CloudsShaderProgram {
  // identifier for the program
  GLuint program;
  // vertex attributes locations
  GLint posLocation;
  GLint texCoordLocation;
  // uniforms locations
  GLint PVMmatrixLocation;
  GLint timeLocation;
  GLint texSamplerLocation;
};

/// shader for animated stars
struct StarShaderProgram {
  // identifier for the program
  GLuint program;
  // vertex attributes locations
  GLint posLocation;
  GLint texCoordLocation;
  // uniforms locations
  GLint PVMmatrixLocation;
  GLint timeLocation;
  GLint texSamplerLocation;
};

typedef std::list<void *> MyList; 

/// Abstract class for scenegraph node transformation.
class ObjectTransform
{
protected:
	glm::mat4 *transformation;
public:
	bool transformable;

	virtual void updateTransformation(float deltaTime)
	{
		transformation = new glm::mat4(1.0f);
	}

	glm::mat4 *getTransformation()
	{
		return transformation;
	}
};


/// Class representing common transformation of scenegraph node.
class ObjectTransformBasic: public ObjectTransform
{
public:
	glm::mat4 *update;

	ObjectTransformBasic(glm::mat4 *mat){
		transformation = new glm::mat4(1.0f);

		update = mat;
		transformable = false;
	}

	void updateTransformation(float deltaTime)
	{
		*transformation = (*update)*(*transformation);
	}
};

/// Class representing specific transformation of scenegraph node. This transformation represents rotation around axis.
class ObjectTransformRotation: public ObjectTransform
{
public:
	float angle;
	float angleDelta;
	glm::vec3 *axis;

	ObjectTransformRotation(glm::vec3 *vec, float delta){
		transformation = new glm::mat4(1.0f);

		axis = vec;
		angle = 0;
		angleDelta = delta;
		transformable = false;
	}

	void updateTransformation(float timeDelta)
	{
		angle += angleDelta*timeDelta;
		*transformation = glm::rotate(glm::mat4(1.0f), angle, *axis);
	}
};

/// Class representing specific transformation of scenegraph node. This transformation represents translation in one direction.
class ObjectTransformTranslation: public ObjectTransform
{
public:
	float time;
	float timeDelta;
	glm::vec3 *velocity;

	ObjectTransformTranslation(glm::vec3 *vec, float delta){
		transformation = new glm::mat4(1.0f);

		velocity = vec;
		time = 0;
		timeDelta = delta;
		transformable = false;
	}

	void updateTransformation(float timeD)
	{
		time += timeDelta*timeD;
		*transformation = glm::translate(glm::mat4(1.0f), *velocity * time);
	}
};

/// Class representing specific transformation of scenegraph node. This transformation represents bouncing of clouds. Y coord changes with time to values of modified sinusoid.
class ObjectTransformBouncing: public ObjectTransform
{
public:
	float bounceModifier;
	float timeModifier;
	float timeOffset;

	float timeElapsed;

	ObjectTransformBouncing(float bounce, float time, float timeOff){
		transformation = new glm::mat4(1.0f);

		bounceModifier = bounce;
		timeModifier = time;
		timeOffset = timeOff;
		timeElapsed = 0;
		transformable = true;
	}

	void updateTransformation(float timeDelta)
	{
		timeElapsed += timeDelta;
		float bounce = glm::sin(bounceModifier*glm::radians(timeModifier*timeElapsed+timeOffset));
		glm::vec3 *v = new glm::vec3(0.0f,bounce,0.0f);
		*transformation = glm::translate(glm::mat4(1.0f),*v);
	}
};


/// Structure of node of scenegraph.
typedef struct NodeStatic {
	glm::mat4 *transformation;
	ObjectTransform *dynamicTransformation;
	unsigned short id;

	MyList nodes;

	MeshGeometry *geometry;
};

typedef struct Curve {
	size_t  size;
	glm::vec3 *points;

};

/// Structure for object which moves along curve.
typedef struct DynamicObject {
  Curve *curve;
  glm::vec3 position;
  glm::vec3 direction;
  float     size;
  float     speed;

  glm::mat4 *transformation;
  NodeStatic *geometry;
};

typedef struct Reaction {
	unsigned char id;
	NodeStatic *node;
};

/// Structure representing camera object.
struct CameraObject {
	float angleX;
	float angleY;
	float angleZ;

	glm::vec3 loc;
	
	glm::vec3 forward;
	glm::vec3 up;

	bool useDynObject;
	DynamicObject *obj;
};



typedef struct Scene {
  NodeStatic *staticScene;
};

/*
ObjectTransformBasic::ObjectTransformBasic(glm::mat4 mat){
	update = mat;
}
*/

typedef struct Star{
	glm::mat4 *transform;
	float offset;
};

}

#endif