
//----------------------------------------------------------------------------------------
/**
 * \file       data.h
 * \author     Petr Beránek
 * \date       2015/05/09
 * \brief      File contains data used in application
 *
 *  This file contains arrays representing polygons of animated stars and clouds.
 *
*/
//----------------------------------------------------------------------------------------


#ifndef __DATA_H
#define __DATA_H

namespace beranpe6
{

const int cloudsNumQuadVertices = 4;
const float cloudsVertexData[cloudsNumQuadVertices * 5] = {

  // x      y      z     u     v
  -1.0f,  0.0f, -1.0f, 0.0f, 2.0f,
  -1.0f,  0.0f, 1.0f, 0.0f, 0.0f,
   1.0f,  0.0f, -1.0f, 2.0f, 2.0f,
   1.0f,  0.0f, 1.0f, 2.0f, 0.0f
}; /// cloud plane

const int starNumQuadVertices = 4;
const float starVertexData[cloudsNumQuadVertices * 5] = {

  // x      y      z     u     v
  -1.0f,  0.0f, -1.0f, 0.0f, 1.0f,
  -1.0f,  0.0f, 1.0f, 0.0f, 0.0f,
   1.0f,  0.0f, -1.0f, 1.0f, 1.0f,
   1.0f,  0.0f, 1.0f, 1.0f, 0.0f
}; /// star plane

}

#endif

