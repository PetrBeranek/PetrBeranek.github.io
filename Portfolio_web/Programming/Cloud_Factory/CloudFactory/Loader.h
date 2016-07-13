
//----------------------------------------------------------------------------------------
/**
 * \file       Loader.h
 * \author     Petr Beránek
 * \date       2015/05/09
 * \brief      Loader.h and Loader.cpp contains methods and parameters for loading data from files.
 *
 *  Thede files contain methods and constants for loading data from files. Loader can load data for loghts and fog and for materials. Format of data for lights and fog viz. Config.txt. Format of data for material viz. LanternMat.txt.
 *
*/
//----------------------------------------------------------------------------------------

#ifndef __LOADER_H
#define __LOADER_H

#include "Structures.h"

namespace beranpe6
{

void loadConfiguration();
void loadMaterial(MeshGeometry *geom, std::string str);

}

#endif