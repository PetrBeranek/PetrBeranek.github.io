
#include "Curves.h"
#include "Structures.h"
#include "data.h";

namespace beranpe6
{

/// predefined curves
Curve *curve1;
Curve *curve2;
Curve *curve3;
Curve *curve4;



/// data of predefined curves
	float curve1Size = 12;
	glm::vec3 curve1Data[] = {
	glm::vec3( 0.00, 0.0,  0.0),

	glm::vec3(-0.33,  0.35, 0.0),
	glm::vec3(-0.66,  0.35, 0.0),
	glm::vec3(-1.00,  0.0, 0.0),
	glm::vec3(-0.66, -0.35, 0.0),
	glm::vec3(-0.33, -0.35, 0.0),

	glm::vec3( 0.00,  0.0, 0.0),

	glm::vec3( 0.33,  0.35, 0.0),
	glm::vec3( 0.66,  0.35, 0.0),
	glm::vec3( 1.00,  0.0, 0.0),
	glm::vec3( 0.66, -0.35, 0.0),
	glm::vec3( 0.33, -0.35, 0.0)};

	
	float curve2Size = 12;
	glm::vec3 curve2Data[] = {

	glm::vec3( 0.0,  0.0,-2.0),

	glm::vec3(-1.75,  0.33,-1.73),
	glm::vec3(-2.89,  0.97, -0.99),

	glm::vec3(-3.26,  1.28, 0.0),
	
	glm::vec3(-2.89,  0.97, 0.99),
	glm::vec3(-1.75,  0.33, 1.73),
	
	glm::vec3( 0.0,  0.0, 2.0),
	
	glm::vec3( 1.75,  0.33, 1.73),
	glm::vec3( 2.89,  0.97, 0.99),
	
	glm::vec3( 3.26,  1.28, 0.0),
	
	glm::vec3( 2.89,  0.97,-0.99),
	glm::vec3( 1.75,  0.33, -1.73)
	};

	float curve3Size = 4;
	glm::vec3 curve3Data[] = {
		
	glm::vec3( 1.0,  0.0, 0.0),
	glm::vec3( 0.0,  0.0, 1.0),
	glm::vec3(-1.0,  0.0, 0.0),
	glm::vec3( 0.0,  0.0,-1.0)
	};

	float curve4Size = 4;
	glm::vec3 curve4Data[] = {
		
	glm::vec3( 0.1,  0.12, 0.0),
	glm::vec3( 0.0,  0.05, 1.0),
	glm::vec3(-1.0,  0.05, 0.0),
	glm::vec3( 0.0,  0.05,-1.0)
	};

void intializeCurves(){
	curve1 = new Curve();
	curve1->size = curve1Size;
	curve1->points = curve1Data;
	
	curve2 = new Curve();
	curve2->size = curve2Size;
	curve2->points = curve2Data;

	curve3 = new Curve();
	curve3->size = curve3Size;
	curve3->points = curve3Data;

	curve4 = new Curve();
	curve4->size = curve4Size;
	curve4->points = curve4Data;
}

/**
	Compute position based on 4 control points and t parameter from range <0,1>.
*/
glm::vec3 evaluateCurveSegment(
    glm::vec3&  P0,
    glm::vec3&  P1,
    glm::vec3&  P2,
    glm::vec3&  P3,
    const float t
) {
  glm::vec3 result(0.0, 0.0, 0.0);

  glm::vec4 tVec = glm::vec4(t*t*t,t*t,t,1.);
  glm::mat4 mat = glm::mat4(
	  glm::vec4(-1., 2.,-1., 0.),
	  glm::vec4( 3.,-5., 0., 2.),
	  glm::vec4(-3., 4., 1., 0.),
	  glm::vec4( 1.,-1., 0., 0.)
	  );

  glm::mat4 points = glm::transpose(glm::mat4(glm::vec4(P0,0.),glm::vec4(P1,0.),glm::vec4(P2,0.),glm::vec4(P3,0.)));

  glm::vec4 counted = 0.5f * tVec * mat;

  glm::vec4 q = counted * points;
  result = (glm::vec3)q;

  return result;
}

/**
	Compute direction based on 4 control points and t parameter from range <0,1>.
*/
glm::vec3 evaluateCurveSegment_1stDerivative(
    glm::vec3&  P0,
    glm::vec3&  P1,
    glm::vec3&  P2,
    glm::vec3&  P3,
    const float t
) {
  glm::vec3 result(1.0, 0.0, 0.0);

  glm::vec4 tVec = glm::vec4(3*t*t,2*t,1.,0.);
  glm::mat4 mat = glm::mat4(
	  glm::vec4(-1., 2.,-1., 0.),
	  glm::vec4( 3.,-5., 0., 2.),
	  glm::vec4(-3., 4., 1., 0.),
	  glm::vec4( 1.,-1., 0., 0.)
	  );

  glm::mat4 points = glm::transpose(glm::mat4(glm::vec4(P0,0.),glm::vec4(P1,0.),glm::vec4(P2,0.),glm::vec4(P3,0.)));

  glm::vec4 counted = 0.5f * tVec * mat;

  glm::vec4 q = counted * points;
  result = (glm::vec3)q;

  return result;
}

/**
	Compute position on the curve in time t.
*/
glm::vec3 evaluateClosedCurve(Curve *curve,const float t) {
  glm::vec3 result(0.0, 0.0, 0.0);
  int i = (int)t;
  
  result =  evaluateCurveSegment(curve->points[(i-1) % curve->size],curve->points[(i) % curve->size],curve->points[(i+1) % curve->size],curve->points[(i+2) % curve->size],(float)(t-i));

  return result;
}

/**
	Compute direction on the curve in time t.
*/
glm::vec3 evaluateClosedCurve_1stDerivative(Curve *curve,const float t) {
  glm::vec3 result(1.0, 0.0, 0.0);
  int i = (int)t;

  result =  evaluateCurveSegment_1stDerivative(curve->points[(i-1) % curve->size],curve->points[(i) % curve->size],curve->points[(i+1) % curve->size],curve->points[(i+2) % curve->size],(float)(t-i));

  return result;
}

bool isVectorNull(glm::vec3 &vect) {

  return !vect.x && !vect.y && !vect.z;
}

glm::mat4 alignObject(glm::vec3& position, const glm::vec3& front, const glm::vec3& up) {

  glm::vec3 z = -glm::normalize(front);

  if (isVectorNull(z))
    z = glm::vec3(0.0, 0.0, 1.0);

  glm::vec3 x = glm::normalize(glm::cross(up, z));

  if (isVectorNull(x))
    x =  glm::vec3(1.0, 0.0, 0.0);

  glm::vec3 y = glm::cross(z, x);
  glm::mat4 matrix = glm::mat4(
      x.x,        x.y,        x.z,        0.0,
      y.x,        y.y,        y.z,        0.0,
      z.x,        z.y,        z.z,        0.0,
      position.x, position.y, position.z, 1.0
  );

  return matrix;
}
}