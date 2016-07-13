
//----------------------------------------------------------------------------------------
/**
 * \file       ModelManager.h
 * \author     Petr Beránek
 * \date       2015/05/09
 * \brief      ModelManager.h and ModelManager.cpp contains all methods for inicialization of models and scene and for updating and drawing.
 *
 *  These files contain methods for inicialization, drawing and updating models, scene graph and scene objects. Methods named "build*" are specificaly for creating parts of scenegraph. 
 *
*/
//----------------------------------------------------------------------------------------

#ifndef __MODEL_MANAGER_H
#define __MODEL_MANAGER_H


#include "Consts.h"
#include "Structures.h"


namespace beranpe6
{

MeshGeometry* getGeometryTreeTrunk();
MeshGeometry* getGeometryTreeCrown();

void drawTestObject(TestObject *test, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawStaticObject(const glm::mat4 &modelMatrix, MeshGeometry *mesh, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix, unsigned char id);
void updateStaticScene(NodeStatic* tree, float deltaTime);
void setMaterialUniforms(const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, float shininess, GLuint texture);
void setLightDir1Uniforms(LightDir *light);
void setLightDir2Uniforms(LightDir *light);
void setLightPointUniforms(LightPoint *light);
void setFogUniforms(Fog *fog);
void setFogUniformsSkybox(Fog *fog);
void setTransformUniforms(const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
bool loadSingleMesh(const std::string &fileName, SCommonShaderProgram& shader, MeshGeometry** geometry);
void initializeShaderPrograms(void);
void initializeLights();
void initializeModels();
void printDebug(const std::string &msg);
void drawSkybox(const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void updateLights();
void updateLightDir(LightDir *light);

void drawStaticScene(NodeStatic* tree, glm::mat4* transform, glm::mat4 view, glm::mat4 projection );

void cleanupShaderPrograms();
void cleanupModels();
void cleanupScene(Scene *sc);

void activateNodes(unsigned char id);

DynamicObject *selectButterfly(int index);
void initializeData();
void initializeButterflies();
void updateButterfly(DynamicObject *but, float time, float timeDelta);
void updateButterflies(float time, float timeDelta);
void drawButterflies(glm::mat4 view, glm::mat4 projection );
void drawDynamicObject(DynamicObject *dyn, glm::mat4 view, glm::mat4 projection );
void drawClouds(const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix, float timeElapsed, glm::mat4 *camTransform) ;
void drawStar(const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix, float timeElapsed, float offset, glm::mat4 *camTransform, glm::mat4 *ownTransform) ;

bool isFoggy();
void changeFogDensity(float f);
void setFogDensity(float f);

void setShad(int i);

void setLightDir1(glm::vec3 diff, glm::vec3 amb, glm::vec3 spec);
void setLightDir2(glm::vec3 diff, glm::vec3 amb, glm::vec3 spec);
void setLightPoint(glm::vec3 diff, glm::vec3 amb, glm::vec3 spec, float fallOff);
void setFog(glm::vec4 color, float density);

NodeStatic* buildTree(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildHouseWindowRound(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildHouseWindowSquare(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildHouseEnergyBox(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildHouseDoors(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildHouseRoof(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildFactoryShedPlate(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildFactoryShed(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildFactoryTank(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildFactoryConnector(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildPiecesPlank(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildPiecesLadder(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildFactory(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildHouse(glm::mat4 *transform, unsigned char incommingId);

NodeStatic* buildIslandMainBase(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildIslandCloudMaker(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildGearBig(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildGearSmall(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildGearsD(glm::mat4 *transform, unsigned char incommingId);

NodeStatic* buildIslandMain(glm::mat4 *transform, unsigned char incommingId);

NodeStatic* buildAirshipBalloonBelt(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildAirshipBalloon(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildAirshipHolder(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildAirshipFan(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildAirshipBath(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildAirshipBathD(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildAirship(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildAirshipD(glm::mat4 *transform, unsigned char incommingId);

NodeStatic* buildTempScene(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildClouds(glm::mat4 *transform, unsigned char incommingId);

NodeStatic* buildButterflyNutL(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildButterflyNutS(glm::mat4 *transform, unsigned char incommingId);
NodeStatic* buildButterfly(glm::mat4 *transform, unsigned char incommingId);

NodeStatic* buildLantern(glm::mat4 *transform, unsigned char incommingId);

}

#endif