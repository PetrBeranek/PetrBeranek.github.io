
//----------------------------------------------------------------------------------------
/**
 * \file       Curves.h
 * \author     Petr Beránek
 * \date       2015/05/09
 * \brief      Curves.h and Curves.cpp contains all about curves.
 *
 *  These files contains methods and constants for creation of curves (Catmull-Rom spline) and for getiing position and 1st derivation of specific curve in specific time.
 *
*/
//----------------------------------------------------------------------------------------

#ifndef __CURVES_H
#define __CURVES_H


#include "pgr.h"
#include "Structures.h"

namespace beranpe6
{

void intializeCurves();
glm::vec3 evaluateClosedCurve(Curve *curve, const float t);
glm::vec3 evaluateClosedCurve_1stDerivative(Curve *curve, const float t);
glm::mat4 alignObject(glm::vec3& position, const glm::vec3& front, const glm::vec3& up);

}

#endif