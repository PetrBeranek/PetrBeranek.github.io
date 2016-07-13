
#include "Loader.h"
#include "ModelManager.h"
#include <iostream>
#include <fstream>
#include <string>
#include "Structures.h"

namespace beranpe6
{
	
/**
*	Method for loading one vector with 3 components. Also skipping next line.da
*/
glm::vec3 readVec3WithLine(std::ifstream& file){
	std::string line;
	float f1, f2, f3;
	std::getline(file,line);
	f1 = ::atof(line.c_str());
	std::getline(file,line);
	f2 = ::atof(line.c_str());
	std::getline(file,line);
	f3 = ::atof(line.c_str());
	std::getline(file,line);

	return glm::vec3(f1, f2, f3);
	
}

/**
*	Method for loading one float number from file. Also skipping next line.
*/
float readFloatWithLine(std::ifstream& file){
	std::string line;
	float f;
	std::getline(file,line);
	f = ::atof(line.c_str());
	std::getline(file,line);

	return f;
	
}

/**
*	Method for loading material in specific format.
*/
void loadMaterial(MeshGeometry *geom, std::string str){
	std::string line;
	std::ifstream file;
	glm::vec3 v;
	float f;

	file.open(str);
	if(!file.is_open()){
		return;
	}

	std::getline(file,line);
	std::getline(file,line);
	geom->ambient = readVec3WithLine(file);
	geom->diffuse = readVec3WithLine(file);
	geom->specular = readVec3WithLine(file);
	geom->shininess = readFloatWithLine(file);
	geom->texture = 0;
	
}

/**
*	Method for loading configuration file for lights and fog.
*/
void loadConfiguration(){
	std::string line1;
	std::ifstream file;
	file.open("config.txt");

	if(!file.is_open()){
		return;
	}
	
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 v3;
	float f;
	
	std::getline(file,line1);
	std::getline(file,line1);
//dir1-diffuse3
	v1 = readVec3WithLine(file);
//dir1-ambient3
	v2 = readVec3WithLine(file);
//dir1-specular3
	v3 = readVec3WithLine(file);
	setLightDir1(v1, v2, v3);
//dir2-diffuse3
	v1 = readVec3WithLine(file);
//dir2-ambient3
	v2 = readVec3WithLine(file);
//dir2-speculat3
	v3 = readVec3WithLine(file);
	setLightDir2(v1, v2, v3);
//point-diffuse3
	v1 = readVec3WithLine(file);
//point-ambient3
	v2 = readVec3WithLine(file);
//point-specular3
	v3 = readVec3WithLine(file);
//point-fallOff1
	f = readFloatWithLine(file);
	setLightPoint(v1, v2, v3, f);
//fog-color3
	v1 = readVec3WithLine(file);
//fog-density1
	f= readFloatWithLine(file);
	setFog(glm::vec4(v1,0.0f),f);

	file.close();
}
}