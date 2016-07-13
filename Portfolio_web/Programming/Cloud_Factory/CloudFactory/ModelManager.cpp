



#include <iostream>
#include <unordered_map>
#include "pgr.h"
#include "ModelManager.h"
#include "Consts.h"
#include "Curves.h"
#include "data.h"
#include "Lantern.h"
#include "Loader.h"

namespace beranpe6
{

/// geometries of models used in application
MeshGeometry* testObjectGeometry = NULL;

MeshGeometry* skyboxGeometry = NULL;

MeshGeometry* GeometryTreeTrunk = NULL;
MeshGeometry* GeometryTreeCrown = NULL;

MeshGeometry* GeometryHouseBase = NULL;
MeshGeometry* GeometryRoofHolder = NULL;
MeshGeometry* GeometryRoofTop = NULL;

MeshGeometry* GeometryWindowRound = NULL;
MeshGeometry* GeometryGlassRound = NULL;
MeshGeometry* GeometryFrameTop = NULL;
MeshGeometry* GeometryFrameBottom = NULL;
MeshGeometry* GeometryFrameSides = NULL;
MeshGeometry* GeometryGlassSquare = NULL;

MeshGeometry* GeometryDoorBody = NULL;
MeshGeometry* GeometryDoorFrame = NULL;
MeshGeometry* GeometryDoorKnob = NULL;

MeshGeometry* GeometryTransformerSpire = NULL;
MeshGeometry* GeometryTransformerCoils = NULL;
MeshGeometry* GeometryTransformerRoofHalf = NULL;
MeshGeometry* GeometryTransformerBody = NULL;
MeshGeometry* GeometryTransformerFrame = NULL;
MeshGeometry* GeometryTransformerHolder = NULL;

MeshGeometry* GeometryFactoryConnectorBody = NULL;
MeshGeometry* GeometryFactoryConnectorFrame = NULL;
MeshGeometry* GeometryFactoryCovers = NULL;
MeshGeometry* GeometryFactoryShedPlate = NULL;
MeshGeometry* GeometryFactoryShedRivets = NULL;
MeshGeometry* GeometryFactoryShedRoof = NULL;
MeshGeometry* GeometryFactoryShedWalls = NULL;
MeshGeometry* GeometryFactoryTankBody = NULL;
MeshGeometry* GeometryFactoryTankHolder = NULL;
MeshGeometry* GeometryFactoryTankHoop = NULL;
MeshGeometry* GeometryFactoryTubings = NULL;

MeshGeometry* GeometryPiecesExhust = NULL;
MeshGeometry* GeometryPiecesExhustInside = NULL;
MeshGeometry* GeometryPiecesLadderHolder = NULL;
MeshGeometry* GeometryPiecesLadderLongs = NULL;
MeshGeometry* GeometryPiecesLadderShorts = NULL;
MeshGeometry* GeometryPiecesPlank = NULL;
MeshGeometry* GeometryPiecesPlankNails = NULL;

MeshGeometry* GeometryIslandMainBase = NULL;
MeshGeometry* GeometryIslandMainGrass = NULL;

MeshGeometry* GeometryIslandGearsAxis = NULL;
MeshGeometry* GeometryIslandGearsBigHoop = NULL;
MeshGeometry* GeometryIslandGearsBigInside = NULL;
MeshGeometry* GeometryIslandGearsSmall = NULL;

MeshGeometry* GeometryIslandCloudTank = NULL;
MeshGeometry* GeometryIslandCloudTankExhaust = NULL;
MeshGeometry* GeometryIslandCloudTankMetals = NULL;

MeshGeometry* GeometryAirshipAnchor = NULL;
MeshGeometry* GeometryAirshipBalloon = NULL;
MeshGeometry* GeometryAirshipBalloonBelt = NULL;
MeshGeometry* GeometryAirshipBalloonBeltRivets = NULL;
MeshGeometry* GeometryAirshipHolderAxis = NULL;
MeshGeometry* GeometryAirshipHolderBoxes = NULL;
MeshGeometry* GeometryAirshipHolderRopes = NULL;
MeshGeometry* GeometryAirshipBath = NULL;
MeshGeometry* GeometryAirshipBoxes = NULL;
MeshGeometry* GeometryAirshipFanBase = NULL;
MeshGeometry* GeometryAirshipFanSailRails = NULL;
MeshGeometry* GeometryAirshipFanSails = NULL;

MeshGeometry* GeometryCloudA = NULL;
MeshGeometry* GeometryCloudB = NULL;
MeshGeometry* GeometryCloudC = NULL;
MeshGeometry* GeometryCloudD = NULL;
MeshGeometry* GeometryCloudE = NULL;

MeshGeometry* GeometryButterflyBolt = NULL;
MeshGeometry* GeometryButterflyNutL = NULL;
MeshGeometry* GeometryButterflyNutS = NULL;
MeshGeometry* GeometryButterflyWingL = NULL;
MeshGeometry* GeometryButterflyWingS = NULL;

MeshGeometry* GeometryClouds = NULL;
MeshGeometry* GeometryStar = NULL;
MeshGeometry* GeometryLantern = NULL;


/// basic shaders for objects
SCommonShaderProgram shaderProgram;
SCommonShaderProgram shader2Program;
SCommonShaderProgram shader3Program;

/// shader for skybox
SkyboxShaderProgram skyboxShaderProgram;
/// shader for animated clouds
CloudsShaderProgram cloudsShaderProgram;
/// shader for animated stars
StarShaderProgram starShaderProgram;

/// list of objects registred to react to id of clicked object
MyList registrations;
unsigned char currentId = 0;

/// list of butterflies
MyList butterflies;

///lights in scene
LightDir *lightDir1;
LightDir *lightDir2;
LightPoint *lightPoint;

Fog *fog;

/// number of basic shader used
int shad = 0;

extern Curve *curve1;
extern Curve *curve2;
extern Curve *curve3;
extern Curve *curve4;


/**
*	Set which shader will be used for drawing.
*/
void setShad(int i){
	shad = i;
}

/**
*	Register node to id. When object with that id is clicked on screen, transformation of nodes registred under that id will be turned on/off. 
*/
void registerNode(unsigned char id, NodeStatic *node){
	Reaction *reaction = new Reaction();
	reaction->id=id;
	reaction->node = node;
	registrations.push_back(reaction);
}

/**
	Method creates all butterfly objects and add them curves to move along.
*/
void initializeButterflies(){
	DynamicObject *but;
	glm::mat4 *m;
	float scale;
	
	but= new DynamicObject();
	scale = 0.025f;
	but->size = scale;
	but->speed = 1.0f;
	m = new glm::mat4(1.0f);
	*m = glm::scale(*m,glm::vec3(scale));
	but->geometry = buildButterfly(m,0);
	but->curve = curve2;
	updateButterfly(but,0.0f,0.0f);
	butterflies.push_back(but);
	
	but= new DynamicObject();
	scale = 0.025f;
	but->size = scale;
	but->speed = 0.3f;
	m = new glm::mat4(1.0f);
	*m = glm::scale(*m,glm::vec3(scale));
	but->geometry = NULL;
	but->curve = curve4;
	updateButterfly(but,0.0f,0.0f);
	butterflies.push_back(but);
	
	but= new DynamicObject();
	scale = 0.025f;
	but->size = scale;
	but->speed = 0.3f;
	m = new glm::mat4(1.0f);
	*m = glm::scale(*m,glm::vec3(scale));
	but->geometry = buildButterfly(m,0);
	but->curve = curve3;
	updateButterfly(but,0.0f,0.0f);
	butterflies.push_back(but);
	
}

float angleBetweenVectors(glm::vec3 *a, glm::vec3 *b){
	return glm::acos((glm::dot(*a,*b))/(glm::length(*a)*glm::length(*b)));
}

/**
*	Update position and rotation of the butterfly.
*/
void updateButterfly(DynamicObject *but, float time, float timeDelta){
	
	but->position = evaluateClosedCurve(but->curve, time*but->speed);
	but->direction = evaluateClosedCurve_1stDerivative(but->curve, time*but->speed);

	glm::mat4 *m = new glm::mat4(1.0f);
	
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	
	*m = glm::translate(*m,but->position);
	*m = *m * alignObject(but->position,but->direction,up);

	but->transformation = m;
	
	if(but->geometry != NULL){
		updateStaticScene(but->geometry,timeDelta);
	}
}


void cleanupGeometry(MeshGeometry *geometry) {

  glDeleteVertexArrays(1, &(geometry->vertexArrayObject));
  glDeleteBuffers(1, &(geometry->elementBufferObject));
  glDeleteBuffers(1, &(geometry->vertexBufferObject));

  if(geometry->texture != 0)
    glDeleteTextures(1, &(geometry->texture));
}

void cleanupShaderPrograms(void) {
  pgr::deleteProgramAndShaders(shaderProgram.program);
}

void cleanupModels(){
	cleanupGeometry(GeometryAirshipAnchor);
	cleanupGeometry(GeometryAirshipBalloon);
	cleanupGeometry(GeometryAirshipBalloonBelt);
	cleanupGeometry(GeometryAirshipBalloonBeltRivets);
	cleanupGeometry(GeometryAirshipBath);
	cleanupGeometry(GeometryAirshipBoxes);
	cleanupGeometry(GeometryAirshipFanBase);
	cleanupGeometry(GeometryAirshipFanSailRails);
	cleanupGeometry(GeometryAirshipFanSails);
	cleanupGeometry(GeometryAirshipHolderAxis);
	cleanupGeometry(GeometryAirshipHolderBoxes);
	cleanupGeometry(GeometryAirshipHolderRopes);
	cleanupGeometry(GeometryCloudA);
	cleanupGeometry(GeometryCloudB);
	cleanupGeometry(GeometryCloudC);
	cleanupGeometry(GeometryCloudD);
	cleanupGeometry(GeometryCloudE);
	cleanupGeometry(GeometryDoorBody);
	cleanupGeometry(GeometryDoorFrame);
	cleanupGeometry(GeometryDoorKnob);
	cleanupGeometry(GeometryFactoryConnectorBody);
	cleanupGeometry(GeometryFactoryConnectorFrame);
	cleanupGeometry(GeometryFactoryCovers);
	cleanupGeometry(GeometryFactoryShedPlate);
	cleanupGeometry(GeometryFactoryShedRivets);
	cleanupGeometry(GeometryFactoryShedRoof);
	cleanupGeometry(GeometryFactoryShedWalls);
	cleanupGeometry(GeometryFactoryTankBody);
	cleanupGeometry(GeometryFactoryTankHolder);
	cleanupGeometry(GeometryFactoryTankHoop);
	cleanupGeometry(GeometryFactoryTubings);
	cleanupGeometry(GeometryFrameBottom);
	cleanupGeometry(GeometryFrameSides);
	cleanupGeometry(GeometryFrameTop);
	cleanupGeometry(GeometryGlassRound);
	cleanupGeometry(GeometryGlassSquare);
	cleanupGeometry(GeometryHouseBase);
	cleanupGeometry(GeometryIslandCloudTank);
	cleanupGeometry(GeometryIslandCloudTankExhaust);
	cleanupGeometry(GeometryIslandCloudTankMetals);
	cleanupGeometry(GeometryIslandGearsAxis);
	cleanupGeometry(GeometryIslandGearsBigHoop);
	cleanupGeometry(GeometryIslandGearsBigInside);
	cleanupGeometry(GeometryIslandGearsSmall);
	cleanupGeometry(GeometryIslandMainBase);
	cleanupGeometry(GeometryIslandMainGrass);
	cleanupGeometry(GeometryIslandMainGrass);
	cleanupGeometry(GeometryPiecesExhust);
	cleanupGeometry(GeometryPiecesExhustInside);
	cleanupGeometry(GeometryPiecesLadderHolder);
	cleanupGeometry(GeometryPiecesLadderLongs);
	cleanupGeometry(GeometryPiecesLadderShorts);
	cleanupGeometry(GeometryPiecesPlank);
	cleanupGeometry(GeometryPiecesPlankNails);
	cleanupGeometry(GeometryRoofHolder);
	cleanupGeometry(GeometryRoofTop);
	cleanupGeometry(GeometryTransformerBody);
	cleanupGeometry(GeometryTransformerCoils);
	cleanupGeometry(GeometryTransformerFrame);
	cleanupGeometry(GeometryTransformerHolder);
	cleanupGeometry(GeometryTransformerRoofHalf);
	cleanupGeometry(GeometryTransformerSpire);
	cleanupGeometry(GeometryTreeCrown);
	cleanupGeometry(GeometryTreeTrunk);
	cleanupGeometry(GeometryWindowRound);
	
	cleanupGeometry(GeometryButterflyBolt);
	cleanupGeometry(GeometryButterflyNutL);
	cleanupGeometry(GeometryButterflyNutS);
	cleanupGeometry(GeometryButterflyWingL);
	cleanupGeometry(GeometryButterflyWingS);
}

void cleanupStaticTree(NodeStatic *ns){
	for(MyList::iterator it = ns->nodes.begin(); it != ns->nodes.end(); ++it) {
    NodeStatic* node = (NodeStatic *)(*it);
		cleanupStaticTree(node);
		delete node;
	}
	ns->geometry = NULL;
	delete ns->geometry;
	delete ns->transformation; 

}

void cleanupScene(Scene *sc){
	cleanupStaticTree(sc->staticScene);
	delete sc;
}

void drawTestObject(TestObject *test, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {

  glUseProgram(shaderProgram.program);

  glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), test->position);
  modelMatrix = glm::rotate(modelMatrix, test->viewAngle, glm::vec3(0, 0, 1));
  modelMatrix = glm::scale(modelMatrix, glm::vec3(test->size, test->size, test->size));

  // setting matrices to the vertex & fragment shader
  setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

  setMaterialUniforms(
    testObjectGeometry->ambient,
    testObjectGeometry->diffuse,
    testObjectGeometry->specular,
    testObjectGeometry->shininess,
    testObjectGeometry->texture
  );

  glBindVertexArray(testObjectGeometry->vertexArrayObject);
  glDrawElements(GL_TRIANGLES, testObjectGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
  glUseProgram(0);

  return;
}

void drawSkybox(const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {

	glUseProgram(skyboxShaderProgram.program);

  glm::mat4 matrix = projectionMatrix * viewMatrix;

  glm::mat4 viewRotation = viewMatrix;
  viewRotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

  glm::mat4 inversePVmatrix = glm::inverse(projectionMatrix * viewRotation);

  glUniformMatrix4fv(skyboxShaderProgram.inversePVmatrixLocation, 1, GL_FALSE, glm::value_ptr(inversePVmatrix));
  glUniform1i(skyboxShaderProgram.skyboxSamplerLocation, 0);
  setFogUniformsSkybox(fog);

  glBindVertexArray(skyboxGeometry->vertexArrayObject);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxGeometry->texture);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, skyboxGeometry->numTriangles+2);

  glBindVertexArray(0);
  glUseProgram(0);
}

void drawStaticObject(const glm::mat4 &modelMatrix, MeshGeometry *mesh, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix, unsigned char id) {
	
	glStencilFunc(GL_ALWAYS,id,0);

	if(shad == 0){
		glUseProgram(shaderProgram.program);
	}else if(shad == 1){
		glUseProgram(shader2Program.program);
	}else if(shad == 2){
		glUseProgram(shader3Program.program);
	}
  // setting matrices to the vertex & fragment shader
  setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
  
  setLightDir1Uniforms(
	lightDir1
	);
  setLightDir2Uniforms(
	lightDir2
	);
  setLightPointUniforms(
	lightPoint
	);
  setFogUniforms(
	fog
	);

  setMaterialUniforms(
    mesh->ambient,
    mesh->diffuse,
    mesh->specular,
    mesh->shininess,
    mesh->texture
  );

  glBindVertexArray(mesh->vertexArrayObject);
  
  //printf("drawing %d triangles\n",mesh->numTriangles);
  glDrawElements(GL_TRIANGLES, mesh->numTriangles * 3, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
  glUseProgram(0);

  return;
}

DynamicObject *selectButterfly(int index){	
	DynamicObject *but;
	int i = 0;
	for(MyList::iterator it = butterflies.begin(); it != butterflies.end(); ++it) {
		but = (DynamicObject *)(*it);

		if(i==index){
			return but;
		}
		i++;
	}
}

/**
	Update all butterflies
*/
void updateButterflies(float time, float timeDelta){
	for(MyList::iterator it = butterflies.begin(); it != butterflies.end(); ++it) {
		DynamicObject* but = (DynamicObject *)(*it);
		updateButterfly(but,time, timeDelta);
	}
}

void drawButterflies( const glm::mat4 view, const glm::mat4 projection ){
	for(MyList::iterator it = butterflies.begin(); it != butterflies.end(); ++it) {
		DynamicObject* but = (DynamicObject *)(*it);
		drawDynamicObject(but, view, projection);
	}
}

void drawDynamicObject(DynamicObject *dyn, const glm::mat4 view, const glm::mat4 projection ) {
	if(dyn->geometry != NULL){
		drawStaticScene(dyn->geometry,dyn->transformation,view,projection);
	}
}

void setMaterialUniforms(const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, float shininess, GLuint texture) {
  glUniform3fv(shaderProgram.diffuseLocation,  1, glm::value_ptr(diffuse));  // 2nd parameter must be 1 - it declares number of vectors in the vector array
  glUniform3fv(shaderProgram.ambientLocation,  1, glm::value_ptr(ambient));
  glUniform3fv(shaderProgram.specularLocation, 1, glm::value_ptr(specular));
  glUniform1f(shaderProgram.shininessLocation,    shininess);

  if(texture != 0) {
    glUniform1i(shaderProgram.useTextureLocation, 1);  // do texture sampling
    glUniform1i(shaderProgram.texSamplerLocation, 0);  // texturing unit 0 -> samplerID   [for the GPU linker]
    glActiveTexture(GL_TEXTURE0 + 0);                  // texturing unit 0 -> to be bound [for OpenGL BindTexture]
    glBindTexture(GL_TEXTURE_2D, texture);
  }
  else {
    glUniform1i(shaderProgram.useTextureLocation, 0);  // do not sample the texture
  }
}

/**
*	Set uniforms for 1st directioal light.
*/
void setLightDir1Uniforms(LightDir *light) {
	glUniform3fv(shaderProgram.lightDirAmbientLocation1,  1, glm::value_ptr(light->ambient));
	glUniform3fv(shaderProgram.lightDirDiffuseLocation1,  1, glm::value_ptr(light->diffuse));
	glUniform3fv(shaderProgram.lightDirSpecularLocation1, 1, glm::value_ptr(light->specular));
	glUniform3fv(shaderProgram.lightDirPositionLocation1, 1, glm::value_ptr(light->position));
}

/**
*	Set uniforms for 2nd directional light.
*/
void setLightDir2Uniforms(LightDir *light) {
	glUniform3fv(shaderProgram.lightDirAmbientLocation2,  1, glm::value_ptr(light->ambient));
	glUniform3fv(shaderProgram.lightDirDiffuseLocation2,  1, glm::value_ptr(light->diffuse));
	glUniform3fv(shaderProgram.lightDirSpecularLocation2, 1, glm::value_ptr(light->specular));
	glUniform3fv(shaderProgram.lightDirPositionLocation2, 1, glm::value_ptr(light->position));
}

/**
*	Set uniforms for point light.
*/
void setLightPointUniforms(LightPoint *light) {
	glUniform3fv(shaderProgram.lightPointAmbientLocation,  1, glm::value_ptr(light->ambient));
	glUniform3fv(shaderProgram.lightPointDiffuseLocation,  1, glm::value_ptr(light->diffuse));
	glUniform3fv(shaderProgram.lightPointSpecularLocation, 1, glm::value_ptr(light->specular));
	glUniform3fv(shaderProgram.lightPointPositionLocation, 1, glm::value_ptr(light->position));
	glUniform1f(shaderProgram.lightPointFallOffLocation, light->fallOff);
}

void setFogUniforms(Fog *fog) {
	glUniform4fv(shaderProgram.fogColorLocation,  1, glm::value_ptr(fog->fogColor));
	glUniform1f(shaderProgram.fogDensityLocation, fog->fogDensity);
}

void setFogUniformsSkybox(Fog *fog) {
	glUniform4fv(skyboxShaderProgram.fogColorLocation,  1, glm::value_ptr(fog->fogColor));
	glUniform1f(skyboxShaderProgram.fogDensityLocation, fog->fogDensity);
}

void setTransformUniforms(const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {

  glm::mat4 PVM = projectionMatrix * viewMatrix * modelMatrix;
  glUniformMatrix4fv(shaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVM));

  glUniformMatrix4fv(shaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
  glUniformMatrix4fv(shaderProgram.MmatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
  glm::mat4 normalMatrix = glm::transpose(glm::inverse(viewMatrix * modelMatrix));
  glUniformMatrix4fv(shaderProgram.normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));  // correct matrix for non-rigid transform
}
/**
	This method was provided to students to enable us load meshes without having to implement our own loader.
*/
bool loadSingleMesh(const std::string &fileName, SCommonShaderProgram& shader, MeshGeometry** geometry) {
  Assimp::Importer importer;

  importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1); // Unitize object in size (scale the model to fit into (-1..1)^3)
  // Load asset from the file - you can play with various processing steps
  const aiScene * scn = importer.ReadFile(fileName.c_str(), 0
      | aiProcess_Triangulate             // Triangulate polygons (if any).
      | aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
      | aiProcess_GenSmoothNormals        // Calculate normals per vertex.
      | aiProcess_JoinIdenticalVertices);
  // abort if the loader fails
  if(scn == NULL) {
    std::cerr << "assimp error: " << importer.GetErrorString() << std::endl;
    *geometry = NULL;
    return false;
  }
  // some formats store whole scene (multiple meshes and materials, lights, cameras, ...) in one file, we cannot handle that in our simplified example
  if(scn->mNumMeshes != 1) {
	  printf("number of meshes %d \n", scn->mNumMeshes);
    std::cerr << "this simplified loader can only process files with only one mesh" << std::endl;
    *geometry = NULL;
    return false;
  }
  // in this phase we know we have one mesh in our loaded scene, we can directly copy its data to opengl ...
  const aiMesh * mesh = scn->mMeshes[0];

  *geometry = new MeshGeometry;

  // vertex buffer object, store all vertex positions and normals
  glGenBuffers(1, &((*geometry)->vertexBufferObject));
  glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float)*mesh->mNumVertices, 0, GL_STATIC_DRAW); // allocate memory for vertices, normals, and texture coordinates
  // first store all vertices
  glBufferSubData(GL_ARRAY_BUFFER, 0, 3*sizeof(float)*mesh->mNumVertices, mesh->mVertices);
  // then store all normals
  glBufferSubData(GL_ARRAY_BUFFER, 3*sizeof(float)*mesh->mNumVertices, 3*sizeof(float)*mesh->mNumVertices, mesh->mNormals);
  
  // just texture 0 for now
  float *textureCoords = new float[2 * mesh->mNumVertices];  // 2 floats per vertex
  float *currentTextureCoord = textureCoords;

  // copy texture coordinates
  aiVector3D vect;
    
  if(mesh->HasTextureCoords(0) ) {
    // we use 2D textures with 2 coordinates and ignore the third coordinate
    for(unsigned int idx=0; idx<mesh->mNumVertices; idx++) {
      vect = (mesh->mTextureCoords[0])[idx];
      *currentTextureCoord++ = vect.x;
      *currentTextureCoord++ = vect.y;
    }
  }
    
  // finally store all texture coordinates
  glBufferSubData(GL_ARRAY_BUFFER, 6*sizeof(float)*mesh->mNumVertices, 2*sizeof(float)*mesh->mNumVertices, textureCoords);

  // copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
  unsigned int *indices = new unsigned int[mesh->mNumFaces * 3];
  for(unsigned int f = 0; f < mesh->mNumFaces; ++f) {
    indices[f*3 + 0] = mesh->mFaces[f].mIndices[0];
    indices[f*3 + 1] = mesh->mFaces[f].mIndices[1];
    indices[f*3 + 2] = mesh->mFaces[f].mIndices[2];
  }
  // copy our temporary index array to OpenGL and free the array
  glGenBuffers(1, &((*geometry)->elementBufferObject));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

  delete [] indices;

  // copy the material info to MeshGeometry structure
  const aiMaterial *mat  = scn->mMaterials[mesh->mMaterialIndex];
  aiColor3D color;
  aiString name;

  // Get returns: aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
  mat->Get(AI_MATKEY_NAME, name); // may be "" after the input mesh processing. Must be aiString type!

  mat->Get<aiColor3D>(AI_MATKEY_COLOR_DIFFUSE, color);
  (*geometry)->diffuse = glm::vec3(color.r, color.g, color.b);

  // mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
  mat->Get<aiColor3D>(AI_MATKEY_COLOR_AMBIENT, color);
  (*geometry)->ambient = glm::vec3(color.r, color.g, color.b);

  mat->Get<aiColor3D>(AI_MATKEY_COLOR_SPECULAR, color);
  (*geometry)->specular = glm::vec3(color.r, color.g, color.b);

  float shininess;

  mat->Get<float>(AI_MATKEY_SHININESS, shininess);
  (*geometry)->shininess = shininess / 4.0f;  // shininess divisor-not descibed anywhere

  (*geometry)->texture = 0;

  // load texture image
  if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
    // get texture name 
    mat->Get<aiString>(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0) , name);
    std::string textureName = name.data;

    size_t found = fileName.find_last_of("/\\");
    // insert correct texture file path 
    if(found != std::string::npos) { // not found
      //subMesh_p->textureName.insert(0, "/");
      textureName.insert(0, fileName.substr(0, found+1));
    }

    std::cout << "Loading texture file: " << textureName << std::endl;
    (*geometry)->texture = pgr::createTexture(textureName);
  }
  CHECK_GL_ERROR();

  glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
  glBindVertexArray((*geometry)->vertexArrayObject);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject); // bind our element array buffer (indices) to vao
  glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);

  glEnableVertexAttribArray(shader.posLocation);
  glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(shader.normalLocation);
    glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));

  glEnableVertexAttribArray(shader.texCoordLocation);
  glVertexAttribPointer(shader.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));
  CHECK_GL_ERROR();

  glBindVertexArray(0);

  (*geometry)->numTriangles = mesh->mNumFaces;

  return true;
}

void initializeShaderPrograms(void) {

  std::vector<GLuint> shaderList;

  if(true) {

    shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "shader/Basic.vert"));
    shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "shader/Basic.frag"));

    shaderProgram.program = pgr::createProgram(shaderList);

    shaderProgram.posLocation      = glGetAttribLocation(shaderProgram.program, "position");
    shaderProgram.normalLocation   = glGetAttribLocation(shaderProgram.program, "normal");
    shaderProgram.texCoordLocation = glGetAttribLocation(shaderProgram.program, "texCoord");

    shaderProgram.PVMmatrixLocation    = glGetUniformLocation(shaderProgram.program, "PVMmatrix");
    shaderProgram.VmatrixLocation      = glGetUniformLocation(shaderProgram.program, "Vmatrix");
    shaderProgram.MmatrixLocation      = glGetUniformLocation(shaderProgram.program, "Mmatrix");
    shaderProgram.normalMatrixLocation = glGetUniformLocation(shaderProgram.program, "normalMatrix");


    shaderProgram.ambientLocation      = glGetUniformLocation(shaderProgram.program, "material.ambient");
    shaderProgram.diffuseLocation      = glGetUniformLocation(shaderProgram.program, "material.diffuse");
    shaderProgram.specularLocation     = glGetUniformLocation(shaderProgram.program, "material.specular");
    shaderProgram.shininessLocation    = glGetUniformLocation(shaderProgram.program, "material.shininess");

	shaderProgram.lightDirAmbientLocation1      = glGetUniformLocation(shaderProgram.program, "lightDir1.ambient");
	shaderProgram.lightDirDiffuseLocation1      = glGetUniformLocation(shaderProgram.program, "lightDir1.diffuse");
	shaderProgram.lightDirSpecularLocation1     = glGetUniformLocation(shaderProgram.program, "lightDir1.specular");
	shaderProgram.lightDirPositionLocation1    = glGetUniformLocation(shaderProgram.program, "lightDir1.position");

	shaderProgram.lightDirAmbientLocation2      = glGetUniformLocation(shaderProgram.program, "lightDir2.ambient");
	shaderProgram.lightDirDiffuseLocation2      = glGetUniformLocation(shaderProgram.program, "lightDir2.diffuse");
	shaderProgram.lightDirSpecularLocation2     = glGetUniformLocation(shaderProgram.program, "lightDir2.specular");
	shaderProgram.lightDirPositionLocation2    = glGetUniformLocation(shaderProgram.program, "lightDir2.position");

	shaderProgram.lightPointAmbientLocation      = glGetUniformLocation(shaderProgram.program, "lightPoint.ambient");
	shaderProgram.lightPointDiffuseLocation      = glGetUniformLocation(shaderProgram.program, "lightPoint.diffuse");
	shaderProgram.lightPointSpecularLocation     = glGetUniformLocation(shaderProgram.program, "lightPoint.specular");
	shaderProgram.lightPointPositionLocation    = glGetUniformLocation(shaderProgram.program, "lightPoint.position");
	shaderProgram.lightPointFallOffLocation    = glGetUniformLocation(shaderProgram.program, "lightPoint.fallOff");

	
	shaderProgram.fogColorLocation    = glGetUniformLocation(shaderProgram.program, "fog.color");
	shaderProgram.fogDensityLocation    = glGetUniformLocation(shaderProgram.program, "fog.density");

    shaderProgram.texSamplerLocation   = glGetUniformLocation(shaderProgram.program, "texSampler");
    shaderProgram.useTextureLocation   = glGetUniformLocation(shaderProgram.program, "material.useTexture");

  shaderList.clear();
	
  shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "shader/Skybox.vert"));
  shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "shader/Skybox.frag"));
	
  skyboxShaderProgram.program = pgr::createProgram(shaderList);


  skyboxShaderProgram.screenCoordLocation = glGetAttribLocation(skyboxShaderProgram.program, "screenCoord");

  skyboxShaderProgram.skyboxSamplerLocation   = glGetUniformLocation(skyboxShaderProgram.program, "skyboxSampler");
  skyboxShaderProgram.inversePVmatrixLocation = glGetUniformLocation(skyboxShaderProgram.program, "inversePVmatrix");
  
	skyboxShaderProgram.fogColorLocation    = glGetUniformLocation(skyboxShaderProgram.program, "fog.color");
	skyboxShaderProgram.fogDensityLocation    = glGetUniformLocation(skyboxShaderProgram.program, "fog.density");

	 shaderList.clear();


  shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "shader/clouds.vert"));
  shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "shader/clouds.frag"));

  cloudsShaderProgram.program = pgr::createProgram(shaderList);

  cloudsShaderProgram.posLocation      = glGetAttribLocation(cloudsShaderProgram.program, "position");
  cloudsShaderProgram.texCoordLocation = glGetAttribLocation(cloudsShaderProgram.program, "texCoord");

  cloudsShaderProgram.PVMmatrixLocation  = glGetUniformLocation(cloudsShaderProgram.program, "PVMmatrix");
  cloudsShaderProgram.timeLocation       = glGetUniformLocation(cloudsShaderProgram.program, "time");
  cloudsShaderProgram.texSamplerLocation = glGetUniformLocation(cloudsShaderProgram.program, "texSampler");

	 shaderList.clear();

  shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "shader/Stars.vert"));
  shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "shader/Stars.frag"));

  starShaderProgram.program = pgr::createProgram(shaderList);

  starShaderProgram.posLocation      = glGetAttribLocation(starShaderProgram.program, "position");
  starShaderProgram.texCoordLocation = glGetAttribLocation(starShaderProgram.program, "texCoord");

  starShaderProgram.PVMmatrixLocation  = glGetUniformLocation(starShaderProgram.program, "PVMmatrix");
  starShaderProgram.timeLocation       = glGetUniformLocation(starShaderProgram.program, "time");
  starShaderProgram.texSamplerLocation = glGetUniformLocation(starShaderProgram.program, "texSampler");
  
	shaderList.clear();
  }
  if(true){

    shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "shader/Basic.vert"));
    shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "shader/Shading.frag"));

    shader2Program.program = pgr::createProgram(shaderList);

    shader2Program.posLocation      = glGetAttribLocation(shader2Program.program, "position");
    shader2Program.normalLocation   = glGetAttribLocation(shader2Program.program, "normal");
    shader2Program.texCoordLocation = glGetAttribLocation(shader2Program.program, "texCoord");

    shader2Program.PVMmatrixLocation    = glGetUniformLocation(shader2Program.program, "PVMmatrix");
    shader2Program.VmatrixLocation      = glGetUniformLocation(shader2Program.program, "Vmatrix");
    shader2Program.MmatrixLocation      = glGetUniformLocation(shader2Program.program, "Mmatrix");
    shader2Program.normalMatrixLocation = glGetUniformLocation(shader2Program.program, "normalMatrix");


    shader2Program.ambientLocation      = glGetUniformLocation(shader2Program.program, "material.ambient");
    shader2Program.diffuseLocation      = glGetUniformLocation(shader2Program.program, "material.diffuse");
    shader2Program.specularLocation     = glGetUniformLocation(shader2Program.program, "material.specular");
    shader2Program.shininessLocation    = glGetUniformLocation(shader2Program.program, "material.shininess");

	shader2Program.lightDirAmbientLocation1      = glGetUniformLocation(shader2Program.program, "lightDir1.ambient");
	shader2Program.lightDirDiffuseLocation1      = glGetUniformLocation(shader2Program.program, "lightDir1.diffuse");
	shader2Program.lightDirSpecularLocation1     = glGetUniformLocation(shader2Program.program, "lightDir1.specular");
	shader2Program.lightDirPositionLocation1    = glGetUniformLocation(shader2Program.program, "lightDir1.position");

	shader2Program.lightDirAmbientLocation2      = glGetUniformLocation(shader2Program.program, "lightDir2.ambient");
	shader2Program.lightDirDiffuseLocation2      = glGetUniformLocation(shader2Program.program, "lightDir2.diffuse");
	shader2Program.lightDirSpecularLocation2     = glGetUniformLocation(shader2Program.program, "lightDir2.specular");
	shader2Program.lightDirPositionLocation2    = glGetUniformLocation(shader2Program.program, "lightDir2.position");

	shader2Program.lightPointAmbientLocation      = glGetUniformLocation(shader2Program.program, "lightPoint.ambient");
	shader2Program.lightPointDiffuseLocation      = glGetUniformLocation(shader2Program.program, "lightPoint.diffuse");
	shader2Program.lightPointSpecularLocation     = glGetUniformLocation(shader2Program.program, "lightPoint.specular");
	shader2Program.lightPointPositionLocation    = glGetUniformLocation(shader2Program.program, "lightPoint.position");
	shader2Program.lightPointFallOffLocation    = glGetUniformLocation(shader2Program.program, "lightPoint.fallOff");

	
	shader2Program.fogColorLocation    = glGetUniformLocation(shader2Program.program, "fog.color");
	shader2Program.fogDensityLocation    = glGetUniformLocation(shader2Program.program, "fog.density");

    shader2Program.texSamplerLocation   = glGetUniformLocation(shader2Program.program, "texSampler");
    shader2Program.useTextureLocation   = glGetUniformLocation(shader2Program.program, "material.useTexture");

  shaderList.clear();
  }

  if(true){

    shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "shader/Basic.vert"));
    shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "shader/Shading2.frag"));

    shader3Program.program = pgr::createProgram(shaderList);

    shader3Program.posLocation      = glGetAttribLocation(shader3Program.program, "position");
    shader3Program.normalLocation   = glGetAttribLocation(shader3Program.program, "normal");
    shader3Program.texCoordLocation = glGetAttribLocation(shader3Program.program, "texCoord");

    shader3Program.PVMmatrixLocation    = glGetUniformLocation(shader3Program.program, "PVMmatrix");
    shader3Program.VmatrixLocation      = glGetUniformLocation(shader3Program.program, "Vmatrix");
    shader3Program.MmatrixLocation      = glGetUniformLocation(shader3Program.program, "Mmatrix");
    shader3Program.normalMatrixLocation = glGetUniformLocation(shader3Program.program, "normalMatrix");


    shader3Program.ambientLocation      = glGetUniformLocation(shader3Program.program, "material.ambient");
    shader3Program.diffuseLocation      = glGetUniformLocation(shader3Program.program, "material.diffuse");
    shader3Program.specularLocation     = glGetUniformLocation(shader3Program.program, "material.specular");
    shader3Program.shininessLocation    = glGetUniformLocation(shader3Program.program, "material.shininess");

	shader3Program.lightDirAmbientLocation1      = glGetUniformLocation(shader3Program.program, "lightDir1.ambient");
	shader3Program.lightDirDiffuseLocation1      = glGetUniformLocation(shader3Program.program, "lightDir1.diffuse");
	shader3Program.lightDirSpecularLocation1     = glGetUniformLocation(shader3Program.program, "lightDir1.specular");
	shader3Program.lightDirPositionLocation1    = glGetUniformLocation(shader3Program.program, "lightDir1.position");

	shader3Program.lightDirAmbientLocation2      = glGetUniformLocation(shader3Program.program, "lightDir2.ambient");
	shader3Program.lightDirDiffuseLocation2      = glGetUniformLocation(shader3Program.program, "lightDir2.diffuse");
	shader3Program.lightDirSpecularLocation2     = glGetUniformLocation(shader3Program.program, "lightDir2.specular");
	shader3Program.lightDirPositionLocation2    = glGetUniformLocation(shader3Program.program, "lightDir2.position");

	shader3Program.lightPointAmbientLocation      = glGetUniformLocation(shader3Program.program, "lightPoint.ambient");
	shader3Program.lightPointDiffuseLocation      = glGetUniformLocation(shader3Program.program, "lightPoint.diffuse");
	shader3Program.lightPointSpecularLocation     = glGetUniformLocation(shader3Program.program, "lightPoint.specular");
	shader3Program.lightPointPositionLocation    = glGetUniformLocation(shader3Program.program, "lightPoint.position");
	shader3Program.lightPointFallOffLocation    = glGetUniformLocation(shader3Program.program, "lightPoint.fallOff");

	
	shader3Program.fogColorLocation    = glGetUniformLocation(shader3Program.program, "fog.color");
	shader3Program.fogDensityLocation    = glGetUniformLocation(shader3Program.program, "fog.density");

    shader3Program.texSamplerLocation   = glGetUniformLocation(shader3Program.program, "texSampler");
    shader3Program.useTextureLocation   = glGetUniformLocation(shader3Program.program, "material.useTexture");

  shaderList.clear();
  }
  
}
void setLightDir1(glm::vec3 diff, glm::vec3 amb, glm::vec3 spec){
	lightDir1->diffuse = diff;
	lightDir1->ambient = amb;
	lightDir1->specular = spec;
}

void setLightDir2(glm::vec3 diff, glm::vec3 amb, glm::vec3 spec){
	lightDir2->diffuse = diff;
	lightDir2->ambient = amb;
	lightDir2->specular = spec;
}

void setLightPoint(glm::vec3 diff, glm::vec3 amb, glm::vec3 spec, float fallOff){
	lightPoint->diffuse = diff;
	lightPoint->ambient = amb;
	lightPoint->specular = spec;
	lightPoint->fallOff = fallOff;
}

void setFog(glm::vec4 color, float density){	
	fog->fogColor = color;
	fog->fogDensity = density;
}


void initializeLights(){
	lightDir1 = new LightDir();
	setLightDir1(glm::vec3(1.f,1.f,.8f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.0f));
	/*
	lightDir1->diffuse = glm::vec3(1.f,1.f,.8f);
	lightDir1->ambient = glm::vec3(0.0f,0.0f,0.0f);
	lightDir1->specular = glm::vec3(1.0f);
	*/
	lightDir1->position = glm::vec3(2.0f,1.0f,-1.5f);
	
	lightDir1->transformation = glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(1.f,0.f,0.f));
	lightDir1->transformable = true;
	
	lightDir2 = new LightDir();
	setLightDir2(glm::vec3(0.3f,0.3f,0.2f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.2f));
	/*
	lightDir2->diffuse = glm::vec3(0.3f,0.3f,0.2f);
	lightDir2->ambient = glm::vec3(0.0f,0.0f,0.0f);
	lightDir2->specular = glm::vec3(0.2f);
	*/
	lightDir2->position = glm::vec3(2.0f,-1.0f,1.5f);
	
	lightDir2->transformation = glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(1.f,0.f,0.f));
	lightDir2->transformable = true;
	
	lightPoint = new LightPoint();
	setLightPoint(glm::vec3(1.0f, 1.0f, 0.6f),glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(0.0f),0.1);
	/*
	lightPoint->diffuse = glm::vec3(1.0f, 1.0f, 0.6f);
	lightPoint->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	lightPoint->specular = glm::vec3(0.0f);
	lightPoint->fallOff = 0.1;
	*/
	lightPoint->position = glm::vec3(.75f, 0.2f, 0.0f);

	fog = new Fog();
	setFog(glm::vec4(0.5f, 0.5f, 0.5f, 0.0f),0.0f);
	/*
	fog->fogColor = glm::vec4(0.5f, 0.5f, 0.5f, 0.0f);
	fog->fogDensity = 0.0f;
	*/
}
void updateLights(){
	updateLightDir(lightDir1);
	updateLightDir(lightDir2);
}

void updateLightDir(LightDir *light){
	if(light->transformable){
		light->position = (glm::vec3)(light->transformation*glm::vec4(light->position,0.0f));
	}
}


void initSkyboxGeometry(GLuint shader, MeshGeometry **geometry) {

  *geometry = new MeshGeometry;

  static const float screenCoords[] = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f
  };

  glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
  glBindVertexArray((*geometry)->vertexArrayObject);

  glGenBuffers(1, &((*geometry)->vertexBufferObject));\
  glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screenCoords), screenCoords, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(skyboxShaderProgram.screenCoordLocation);
  glVertexAttribPointer(skyboxShaderProgram.screenCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glBindVertexArray(0);
  glUseProgram(0);
  CHECK_GL_ERROR();

  (*geometry)->numTriangles = 2;

  glActiveTexture(GL_TEXTURE0);

  glGenTextures(1, &((*geometry)->texture));
  glBindTexture(GL_TEXTURE_CUBE_MAP, (*geometry)->texture);

  const char * suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
  GLuint targets[] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
  };

  for( int i = 0; i < 6; i++ ) {
    std::string texName = std::string(SKYBOX_PREFIX) + "_" + suffixes[i] + ".jpg";
    std::cout << "Loading cube map texture: " << texName << std::endl;
    if(!pgr::loadTexImage2D(texName, targets[i])) {
      pgr::dieWithError("Skybox cube map loading failed!");
    }
  }

  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  CHECK_GL_ERROR();
}

void initCloudsGeometry(MeshGeometry **geometry) {

  *geometry = new MeshGeometry;
  
  (*geometry)->texture = pgr::createTexture(CLOUDS_TEXTURE_NAME);
  glBindTexture(GL_TEXTURE_2D, (*geometry)->texture);

  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

  glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
  glBindVertexArray((*geometry)->vertexArrayObject);

  glGenBuffers(1, &((*geometry)->vertexBufferObject));
  glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cloudsVertexData), cloudsVertexData, GL_STATIC_DRAW);

  glEnableVertexAttribArray(cloudsShaderProgram.posLocation);
  glEnableVertexAttribArray(cloudsShaderProgram.texCoordLocation);
  // vertices of triangles - start at the beginning of the interlaced array
  glVertexAttribPointer(cloudsShaderProgram.posLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
  // texture coordinates of each vertices are stored just after its position
  glVertexAttribPointer(cloudsShaderProgram.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  glBindVertexArray(0);

  (*geometry)->numTriangles = cloudsNumQuadVertices;
}

void initLanternGeometry(SCommonShaderProgram &shader, MeshGeometry **geometry) {

  *geometry = new MeshGeometry;

  glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
  glBindVertexArray((*geometry)->vertexArrayObject);

  glGenBuffers(1, &((*geometry)->vertexBufferObject));
  glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
  //glBufferData(GL_ARRAY_BUFFER, sizeof( lanternVertices ), lanternVertices, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, lanternNVertices * lanternNAttribsPerVertex * sizeof(float), lanternVertices, GL_STATIC_DRAW);

  // copy our temporary index array to opengl and free the array
  glGenBuffers(1, &((*geometry)->elementBufferObject));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int) * lanternNTriangles, lanternTriangles, GL_STATIC_DRAW);

  glEnableVertexAttribArray(shader.posLocation);
  // vertices of triangles - start at the beginning of the array
  glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
  
    glEnableVertexAttribArray(shader.normalLocation);
    // normal of vertex starts after the color (interlaced array)
    glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(shader.texCoordLocation);
    // normal of vertex starts after the color (interlaced array)
    glVertexAttribPointer(shader.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	loadMaterial(*geometry, LANTERN_MAT);
  /*
	(*geometry)->ambient = glm::vec3(1.0f, 0.0f, 1.0f);
	(*geometry)->diffuse = glm::vec3(1.0f, 0.0f, 1.0f);
	(*geometry)->specular = glm::vec3(1.0f, 0.0f, 1.0f);
	(*geometry)->shininess = 10.0f;
	(*geometry)->texture = 0;
  */

  glBindVertexArray(0);

  (*geometry)->numTriangles = lanternNTriangles;
}

void initStarGeometry(MeshGeometry **geometry) {

  *geometry = new MeshGeometry;
  
  (*geometry)->texture = pgr::createTexture(STAR_TEXTURE_NAME);
  glBindTexture(GL_TEXTURE_2D, (*geometry)->texture);

  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

  glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
  glBindVertexArray((*geometry)->vertexArrayObject);

  glGenBuffers(1, &((*geometry)->vertexBufferObject));
  glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(starVertexData), starVertexData, GL_STATIC_DRAW);

  glEnableVertexAttribArray(starShaderProgram.posLocation);
  glEnableVertexAttribArray(starShaderProgram.texCoordLocation);
  // vertices of triangles - start at the beginning of the interlaced array
  glVertexAttribPointer(starShaderProgram.posLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
  // texture coordinates of each vertices are stored just after its position
  glVertexAttribPointer(starShaderProgram.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  glBindVertexArray(0);

  (*geometry)->numTriangles = cloudsNumQuadVertices;
}

bool isFoggy(){
	return fog->fogDensity!=0;
}

void changeFogDensity(float f){
	fog->fogDensity += f;
	/*if(fog->fogDensity < 0){
		fog->fogDensity = 0.0f;
	}*/
}

void setFogDensity(float f){
	fog->fogDensity = f;
}

void drawClouds(const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix, float timeElapsed, glm::mat4 *camTransform) {

	if(fog->fogDensity > 0){ return; }

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);

  glDisable(GL_DEPTH_TEST);

  glUseProgram(cloudsShaderProgram.program);

  glm::mat4 matrix = glm::translate(glm::mat4(1.0f),  (glm::vec3)( (*camTransform)[3] + glm::vec4(0.0f, -10.0f, 0.0f, 0.0f)));
  matrix = glm::scale(matrix, glm::vec3(60.0f));

  glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * matrix;
  glUniformMatrix4fv(cloudsShaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));        // model-view-projection
  glUniform1f(cloudsShaderProgram.timeLocation, timeElapsed);
  glUniform1i(cloudsShaderProgram.texSamplerLocation, 0);

  glBindTexture(GL_TEXTURE_2D, GeometryClouds->texture);
  glBindVertexArray(GeometryClouds->vertexArrayObject);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, GeometryClouds->numTriangles);

  CHECK_GL_ERROR();

  glBindVertexArray(0);
  glUseProgram(0);

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  return;
}

void drawStar(const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix, float timeElapsed, float offset, glm::mat4 *camTransform, glm::mat4 *ownTransform) {
	
	if(fog->fogDensity > 0){ return; }

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);

  //glDisable(GL_DEPTH_TEST);

  glUseProgram(starShaderProgram.program);
  
  //glm::mat4 matrix = glm::translate(*ownTransform,  (glm::vec3)( (*camTransform)[3]));
  //glm::mat4 matrix = glm::translate(glm::mat4(1.0f),  (glm::vec3)( (*camTransform)[3]));
  glm::mat4 matrix = glm::translate( glm::mat4(1.0f),  (glm::vec3)( (*camTransform)[3] + glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)));

  glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * matrix * *ownTransform;
  glUniformMatrix4fv(starShaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));        // model-view-projection
  glUniform1f(starShaderProgram.timeLocation, timeElapsed+offset);
  glUniform1i(starShaderProgram.texSamplerLocation, 0);

  glBindTexture(GL_TEXTURE_2D, GeometryStar->texture);
  glBindVertexArray(GeometryStar->vertexArrayObject);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, GeometryStar->numTriangles);

  CHECK_GL_ERROR();

  glBindVertexArray(0);
  glUseProgram(0);

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  return;
}


void initializeData(){
	intializeCurves();
	initializeButterflies();
}

void initializeModels() {

	
  initSkyboxGeometry(skyboxShaderProgram.program, &skyboxGeometry);

  // load asteroid model from external file
  if(loadSingleMesh(TEST_OBJECT, shaderProgram, &testObjectGeometry) != true) {
    std::cerr << "initializeModels(): Test Object model loading failed." << std::endl;
  }
  CHECK_GL_ERROR();

  //load tree
	if(true){
		if(loadSingleMesh(MODEL_TREE_TRUNK, shaderProgram, &GeometryTreeTrunk) != true) {
			std::cerr << "initializeModels(): TREE TRUNK model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
  
		if(loadSingleMesh(MODEL_TREE_CROWN, shaderProgram, &GeometryTreeCrown) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
	}
  //load house
	if (true){
		if(loadSingleMesh(MODEL_HOUSE_BASE, shaderProgram, &GeometryHouseBase) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();


		if(loadSingleMesh(MODEL_ROOF_HOLDER, shaderProgram, &GeometryRoofHolder) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_ROOF_TOP, shaderProgram, &GeometryRoofTop) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_WINDOW_ROUND, shaderProgram, &GeometryWindowRound) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_GLASS_ROUND, shaderProgram, &GeometryGlassRound) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_FRAME_TOP, shaderProgram, &GeometryFrameTop) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_FRAME_BOTTOM, shaderProgram, &GeometryFrameBottom) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_FRAME_SIDES, shaderProgram, &GeometryFrameSides) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_GLASS_SQUARE, shaderProgram, &GeometryGlassSquare) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR(); 
	}

	

	if(true){
		if(loadSingleMesh(MODEL_DOOR_BODY, shaderProgram, &GeometryDoorBody) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_DOOR_FRAME, shaderProgram, &GeometryDoorFrame) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_DOOR_KNOB, shaderProgram, &GeometryDoorKnob) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR(); 
	}

	if(true){
		if(loadSingleMesh(MODEL_TRANSFORMER_SPIRE, shaderProgram, &GeometryTransformerSpire) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_TRANSFORMER_COILS, shaderProgram, &GeometryTransformerCoils) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_TRANSFORMER_ROOF_HALF, shaderProgram, &GeometryTransformerRoofHalf) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR(); 

		if(loadSingleMesh(MODEL_TRANSFORMER_BODY, shaderProgram, &GeometryTransformerBody) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_TRANSFORMER_FRAME, shaderProgram, &GeometryTransformerFrame) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_TRANSFORMER_HOLDER, shaderProgram, &GeometryTransformerHolder) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR(); 
	}

	if(true){
		if(loadSingleMesh(MODEL_FACTORY_CONNECTOR_BODY, shaderProgram, &GeometryFactoryConnectorBody) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_FACTORY_CONNECTOR_FRAME, shaderProgram, &GeometryFactoryConnectorFrame) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_FACTORY_COVERS, shaderProgram, &GeometryFactoryCovers) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR(); 

		if(loadSingleMesh(MODEL_FACTORY_SHED_PLATE, shaderProgram, &GeometryFactoryShedPlate) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_FACTORY_SHED_RIVETS, shaderProgram, &GeometryFactoryShedRivets) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_FACTORY_SHED_ROOF, shaderProgram, &GeometryFactoryShedRoof) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR(); 

		if(loadSingleMesh(MODEL_FACTORY_SHED_WALLS, shaderProgram, &GeometryFactoryShedWalls) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR(); 

		if(loadSingleMesh(MODEL_FACTORY_TANK_BODY, shaderProgram, &GeometryFactoryTankBody) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR(); 

		if(loadSingleMesh(MODEL_FACTORY_TANK_HOLDER, shaderProgram, &GeometryFactoryTankHolder) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR(); 

		if(loadSingleMesh(MODEL_FACTORY_TANK_HOOP, shaderProgram, &GeometryFactoryTankHoop) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR(); 

		if(loadSingleMesh(MODEL_FACTORY_TUBINGS, shaderProgram, &GeometryFactoryTubings) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR(); 
	}

	if(true){
		if(loadSingleMesh(MODEL_PIECES_EXHAUST, shaderProgram, &GeometryPiecesExhust) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_PIECES_EXHAUST_INSIDE, shaderProgram, &GeometryPiecesExhustInside) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_PIECES_LADDER_HOLDER, shaderProgram, &GeometryPiecesLadderHolder) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_PIECES_LADDER_LONGS, shaderProgram, &GeometryPiecesLadderLongs) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_PIECES_LADDER_SHORTS, shaderProgram, &GeometryPiecesLadderShorts) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_PIECES_PLANK, shaderProgram, &GeometryPiecesPlank) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_PIECES_PLANK_NAILS, shaderProgram, &GeometryPiecesPlankNails) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
	}

	if(true){
		if(loadSingleMesh(MODEL_ISLAND_MAIN_BASE, shaderProgram, &GeometryIslandMainBase) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_ISLAND_MAIN_GRASS, shaderProgram, &GeometryIslandMainGrass) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_ISLAND_GEARS_AXIS, shaderProgram, &GeometryIslandGearsAxis) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_ISLAND_GEARS_BIG_HOOP, shaderProgram, &GeometryIslandGearsBigHoop) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_ISLAND_GEARS_BIG_INSIDE, shaderProgram, &GeometryIslandGearsBigInside) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_ISLAND_GEARS_SMALL, shaderProgram, &GeometryIslandGearsSmall) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_ISLAND_CLOUD_TANK, shaderProgram, &GeometryIslandCloudTank) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_ISLAND_CLOUD_TANK_EXHAUST, shaderProgram, &GeometryIslandCloudTankExhaust) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_ISLAND_CLOUD_TANK_METALS, shaderProgram, &GeometryIslandCloudTankMetals) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
	}

    if(true){
		if(loadSingleMesh(MODEL_AIRSHIP_ANCHOR, shaderProgram, &GeometryAirshipAnchor) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_AIRSHIP_BALLOON, shaderProgram, &GeometryAirshipBalloon) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_AIRSHIP_BALLOON_BELT, shaderProgram, &GeometryAirshipBalloonBelt) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_AIRSHIP_BALLOON_BELT_RIVETS, shaderProgram, &GeometryAirshipBalloonBeltRivets) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_AIRSHIP_BALLOON_HOLDER_AXIS, shaderProgram, &GeometryAirshipHolderAxis) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_AIRSHIP_BALLOON_HOLDER_BOXES, shaderProgram, &GeometryAirshipHolderBoxes) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_AIRSHIP_BALLOON_HOLDER_ROPES, shaderProgram, &GeometryAirshipHolderRopes) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_AIRSHIP_BATH, shaderProgram, &GeometryAirshipBath) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_AIRSHIP_BOXES, shaderProgram, &GeometryAirshipBoxes) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_AIRSHIP_FAN_BASE, shaderProgram, &GeometryAirshipFanBase) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_AIRSHIP_FAN_SAIL_RAILS, shaderProgram, &GeometryAirshipFanSailRails) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

		if(loadSingleMesh(MODEL_AIRSHIP_FAN_SAILS, shaderProgram, &GeometryAirshipFanSails) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();

	}

	if(true){
		if(loadSingleMesh(MODEL_CLOUD_A, shaderProgram, &GeometryCloudA) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_CLOUD_B, shaderProgram, &GeometryCloudB) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_CLOUD_C, shaderProgram, &GeometryCloudC) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_CLOUD_D, shaderProgram, &GeometryCloudD) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_CLOUD_E, shaderProgram, &GeometryCloudE) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
	}

	
	if(true){
		if(loadSingleMesh(MODEL_BUTTERFLY_BOLT, shaderProgram, &GeometryButterflyBolt) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_BUTTERFLY_NUT_L, shaderProgram, &GeometryButterflyNutL) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_BUTTERFLY_NUT_S, shaderProgram, &GeometryButterflyNutS) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_BUTTERFLY_WING_L, shaderProgram, &GeometryButterflyWingL) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
		
		if(loadSingleMesh(MODEL_BUTTERFLY_WING_S, shaderProgram, &GeometryButterflyWingS) != true) {
			std::cerr << "initializeModels(): TREE CROWN model loading failed." << std::endl;
		}
		CHECK_GL_ERROR();
	}
	
	initCloudsGeometry(&GeometryClouds);
		CHECK_GL_ERROR();

	initStarGeometry(&GeometryStar);
		CHECK_GL_ERROR();

	initLanternGeometry(shaderProgram, &GeometryLantern);
		CHECK_GL_ERROR();
}

void printDebug(const std::string &msg){
	std::cerr << msg << std::endl;
}

/**
*	Turn on/off transformation of all nodes registred with this id. Not recursive.
*/
void activateNodes(unsigned char id){
	
	for(MyList::iterator it = registrations.begin(); it != registrations.end(); ++it) {
		Reaction* r = (Reaction *)(*it);
		if(r->id == id && r->node->dynamicTransformation != NULL){
			r->node->dynamicTransformation->transformable = !r->node->dynamicTransformation->transformable; 
		}
	}
}



void drawStaticScene(NodeStatic* tree, glm::mat4* transform, glm::mat4 view, glm::mat4 projection ){
	glm::mat4 *mat = new glm::mat4();
	if((tree->dynamicTransformation) != NULL){
		*mat = (*transform)*(*tree->transformation)*(*(*tree->dynamicTransformation).getTransformation());
	}else{
		*mat = (*transform)*(*tree->transformation);
	}
	for(MyList::iterator it = tree->nodes.begin(); it != tree->nodes.end(); ++it) {
    NodeStatic* node = (NodeStatic *)(*it);
			drawStaticScene( node, mat, view, projection );
	}

	if(tree->geometry != NULL){
		drawStaticObject(*mat,tree->geometry,view,projection,tree->id);
	}
}

void updateStaticScene(NodeStatic* tree, float deltaTime){
	glm::mat4 *mat = new glm::mat4();
	if((tree->dynamicTransformation) != NULL && (*tree->dynamicTransformation).transformable){
		(*tree->dynamicTransformation).updateTransformation(deltaTime);
	}

	for(MyList::iterator it = tree->nodes.begin(); it != tree->nodes.end(); ++it) {
    NodeStatic* node = (NodeStatic *)(*it);
			updateStaticScene(node, deltaTime);
  }
}

/*
* Following methods are for building all components of scene.
*/

NodeStatic* buildLeaf(glm::mat4 *transform, MeshGeometry *mesh, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;

	node->geometry = mesh;
	return node;
}

NodeStatic* buildLeafDynamic(glm::mat4 *transform, MeshGeometry *mesh, unsigned char incommingId, ObjectTransform *tr){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = tr;
	node->id=id;

	node->geometry = mesh;
	return node;
}


NodeStatic* buildTree(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = GeometryTreeTrunk;

	glm::mat4 *matC = new glm::mat4(1.0f);
	
	*matC = glm::translate(*matC,glm::vec3(0.0f,1.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.5f,1.5f,1.5f));

	node->nodes.push_back(buildLeaf(matC,GeometryTreeCrown,id));

	return node;
}

NodeStatic* buildHouseWindowRound(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = GeometryWindowRound;

	glm::mat4 *matC = new glm::mat4(1.0f);

	node->nodes.push_back(buildLeaf(matC,GeometryGlassRound,id));

	return node;
}

NodeStatic* buildHouseWindowSquare(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.f,0.95f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.f,0.7f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryFrameTop,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f,-0.95f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,0.7f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryFrameBottom,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f,0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryFrameSides,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f,0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(0.95f,0.95f,0.95f));
	node->nodes.push_back(buildLeaf(matC,GeometryGlassSquare,id));

	return node;
}

NodeStatic* buildHouseRoof(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = GeometryRoofHolder;

	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f,0.93f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.f));

	node->nodes.push_back(buildLeaf(matC,GeometryRoofTop,id));

	return node;
}
/*

	*matC = glm::translate(*matC,glm::vec3(0.f,0.f,0.f));
	*matC = glm::scale(*matC,glm::vec3(1.f,1.f,1.f));
*/

NodeStatic* buildTransformerRoof(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;
	
	matC = new glm::mat4(1.0f);
	*matC = glm::rotate(*matC,33.0f,glm::vec3(0.0f,0.0f,1.0f));
	*matC = glm::translate(*matC,glm::vec3(0.f,0.f,0.f));
	*matC = glm::scale(*matC,glm::vec3(1.2f,1.2f,1.2f));
	node->nodes.push_back(buildLeaf(matC,GeometryTransformerSpire,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::rotate(*matC,33.0f,glm::vec3(0.0f,0.0f,1.0f));
	*matC = glm::translate(*matC,glm::vec3(0.f,0.4f,0.f));
	*matC = glm::scale(*matC,glm::vec3(1.f,1.f,1.f));
	node->nodes.push_back(buildLeaf(matC,GeometryTransformerCoils,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.45f,-1.f,0.f));
	*matC = glm::scale(*matC,glm::vec3(.75f,.75f,.75f));
	node->nodes.push_back(buildLeaf(matC,GeometryTransformerRoofHalf,id));

	return node;
}

NodeStatic* buildHouseEnergyBox(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-1.15f,2.3f,0.f));
	*matC = glm::scale(*matC,glm::vec3(1.f,1.f,1.f));
	node->nodes.push_back(buildTransformerRoof(matC,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::rotate(*matC,180.0f,glm::vec3(0.0f,1.0f,0.0f));
	*matC = glm::translate(*matC,glm::vec3(-1.15f,2.3f,0.f));
	*matC = glm::scale(*matC,glm::vec3(1.f,1.f,1.f));
	node->nodes.push_back(buildTransformerRoof(matC,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.f,0.35f,0.f));
	*matC = glm::scale(*matC,glm::vec3(1.28f,1.28f,1.28f));
	node->nodes.push_back(buildLeaf(matC,GeometryTransformerBody,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.f,0.f,0.f));
	*matC = glm::scale(*matC,glm::vec3(1.f,1.f,1.f));
	node->nodes.push_back(buildLeaf(matC,GeometryTransformerFrame,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.f,0.f,1.15f));
	*matC = glm::scale(*matC,glm::vec3(.75f,.75f,.75f));
	node->nodes.push_back(buildLeaf(matC,GeometryTransformerHolder,id));

	return node;
}

NodeStatic* buildHouseDoors(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.f,-0.1f,0.f));
	*matC = glm::scale(*matC,glm::vec3(1.f,.9f,1.f));
	node->nodes.push_back(buildLeaf(matC,GeometryDoorBody,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.f,0.f,0.f));
	*matC = glm::scale(*matC,glm::vec3(1.f,1.f,1.f));
	node->nodes.push_back(buildLeaf(matC,GeometryDoorFrame,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.15f,-0.2f,0.4f));
	*matC = glm::scale(*matC,glm::vec3(.1f,.1f,.1f));
	node->nodes.push_back(buildLeaf(matC,GeometryDoorKnob,id));

	return node;
}




/*
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildRoof(matC));

	*/


NodeStatic* buildFactoryShedPlate(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryFactoryShedPlate,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.05f));
	*matC = glm::scale(*matC,glm::vec3(.9f,.9f,.9f));
	node->nodes.push_back(buildLeaf(matC,GeometryFactoryShedRivets,id));
	
	return node;
}

NodeStatic* buildFactoryShed(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-0.7f, -0.15f,0.4f));
	*matC = glm::scale(*matC,glm::vec3(.35f,.35f,.35f));
	node->nodes.push_back(buildFactoryShedPlate(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.13f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.2f,1.2f,1.2f));
	node->nodes.push_back(buildLeaf(matC,GeometryFactoryShedRoof,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryFactoryShedWalls,id));
	
	return node;
}

NodeStatic* buildFactoryTankHoop(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryFactoryTankHoop,id));

	return node;
}

NodeStatic* buildFactoryConnector(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.1f, 1.1f, 1.1f));
	node->nodes.push_back(buildLeaf(matC,GeometryFactoryConnectorBody,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.15f,1.15f,1.15f));
	node->nodes.push_back(buildLeaf(matC,GeometryFactoryConnectorFrame,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(2.65f, -4.2f, 0.924f));
	*matC = glm::scale(*matC,glm::vec3(5.0f,5.0f,5.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryFactoryCovers,id));

	return node;
}

NodeStatic* buildFactoryTank(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryFactoryTankBody,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(1.0f, -0.12f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(.6f,.6f,.6f));
	node->nodes.push_back(buildLeaf(matC,GeometryFactoryTankHolder,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.5f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(.83f,.83f,.83f));
	node->nodes.push_back(buildFactoryTankHoop(matC,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, -0.5f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(.83f,.83f,.83f));
	node->nodes.push_back(buildFactoryTankHoop(matC,id));
	
	return node;
}

NodeStatic* buildFactory(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-0.15f, -0.2f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(.3f,.3f,.3f));
	node->nodes.push_back(buildFactoryTank(matC,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-0.7f, -0.75f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(.34f,.3f,.34f));
	node->nodes.push_back(buildFactoryShed(matC,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-0.0f, -0.175f, 0.26f));
	*matC = glm::scale(*matC,glm::vec3(.64f,.62f, .64f));
	node->nodes.push_back(buildLeaf(matC,GeometryFactoryTubings,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-0.15f, 0.385f,-0.01f));
	*matC = glm::scale(*matC,glm::vec3(.1f,.1f,.1f));
	node->nodes.push_back(buildFactoryConnector(matC,id));

	return node;
}

NodeStatic* buildPiecesLadder(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(.35f,.35f,.35f));
	node->nodes.push_back(buildLeaf(matC,GeometryPiecesLadderHolder,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(.52f, -0.78f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryPiecesLadderLongs,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(.57f, -0.78f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(.9f,.9f,.9f));
	node->nodes.push_back(buildLeaf(matC,GeometryPiecesLadderShorts,id));
	
	return node;
}

NodeStatic* buildButterflyNutL(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = new ObjectTransformRotation(new glm::vec3(0.f, 0.f, 1.f), -500.0f);
	node->dynamicTransformation->transformable = true;
	node->id=id;
	
	node->geometry = GeometryButterflyNutL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.5f,-1.15f));
	*matC = glm::scale(*matC,glm::vec3(4.2f,4.2f,4.2f));
	node->nodes.push_back(buildLeaf(matC,GeometryButterflyWingL,id));

	return node;
}

NodeStatic* buildButterflyNutS(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = new ObjectTransformRotation(new glm::vec3(0.f, 0.f, 1.f),750.0f);
	node->dynamicTransformation->transformable = true;
	node->id=id;
	
	node->geometry = GeometryButterflyNutS;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.3f,.8f));
	*matC = glm::scale(*matC,glm::vec3(5.0f,5.0f,5.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryButterflyWingS,id));

	return node;
}

NodeStatic* buildButterfly(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.3f));
	*matC = glm::scale(*matC,glm::vec3(2.0f,2.0f,2.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryButterflyBolt,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildButterflyNutL(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f, 1.f));
	*matC = glm::scale(*matC,glm::vec3(.7f,.7f,.7f));
	node->nodes.push_back(buildButterflyNutS(matC,id));

	return node;
}

NodeStatic* buildPiecesPlank(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = GeometryPiecesPlank;
	
	glm::mat4 *matC;
	/*
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryPiecesPlank));
	*/
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.05f));
	*matC = glm::scale(*matC,glm::vec3(.9f,.9f,.9f));
	node->nodes.push_back(buildLeaf(matC,GeometryPiecesPlankNails,id));
	
	return node;
}

NodeStatic* buildHouse(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = GeometryHouseBase;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(.49f,0.12f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(0.5f,0.5f,0.4f));
	node->nodes.push_back(buildHouseRoof(matC,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.48f,0.48f,0.315f));
	*matC = glm::scale(*matC,glm::vec3(0.1f,0.1f,0.1f));
	node->nodes.push_back(buildHouseWindowRound(matC,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.48f,0.0f,0.3f));
	*matC = glm::scale(*matC,glm::vec3(.13f,.13f,.13f));
	node->nodes.push_back(buildHouseWindowSquare(matC,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(.57f,-0.8f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(.2f,.2f,.2f));
	node->nodes.push_back(buildHouseDoors(matC,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.5f, 0.0f,-0.5f));
	*matC = glm::scale(*matC,glm::vec3(.15f,.15f,.15f));
	node->nodes.push_back(buildHouseEnergyBox(matC,id));
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildFactory(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(1.07f, 0.2f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(.35f,.35f,.35f));
	node->nodes.push_back(buildLeaf(matC,GeometryPiecesExhust,id));
	/*
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(1.07f, 0.2f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(.349f,.35f,.349f));
	node->nodes.push_back(buildLeaf(matC,GeometryPiecesExhustInside));
	*/
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(.7f, -0.13f,-0.4f));
	*matC = glm::scale(*matC,glm::vec3(.5f,.5f,.5f));
	node->nodes.push_back(buildPiecesLadder(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.3f, -0.65f,.32f));
	*matC = glm::scale(*matC,glm::vec3(.2,.2f,.2f));
	node->nodes.push_back(buildPiecesPlank(matC,id));

	return node;
}


NodeStatic* buildIslandMainBase(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, -0.36f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryIslandMainBase,id));


	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.02f,1.02f,1.02f));
	node->nodes.push_back(buildLeaf(matC,GeometryIslandMainGrass,id));

	return node;
}

NodeStatic* buildIslandCloudMaker(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryIslandCloudTank,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.8f, -1.5f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryIslandCloudTankExhaust,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, -0.5f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.1f,1.1f,1.1f));
	node->nodes.push_back(buildLeaf(matC,GeometryIslandCloudTankMetals,id));

	return node;
}

NodeStatic* buildGearBig(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = new ObjectTransformRotation(new glm::vec3(0.0f,0.0f,1.0f),-33.333f);
	(*node->dynamicTransformation).transformable = true;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,-0.4f));
	*matC = glm::scale(*matC,glm::vec3(.7f,.7f,.8f));
	node->nodes.push_back(buildLeaf(matC,GeometryIslandGearsAxis,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(2.0f,2.0f,2.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryIslandGearsBigInside,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.55f,1.55f,1.55f));
	node->nodes.push_back(buildLeaf(matC,GeometryIslandGearsBigHoop,id));

	return node;
}

NodeStatic* buildGearSmall(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = new ObjectTransformRotation(new glm::vec3(0.0f,0.0f,1.0f),100.0f);
	(*node->dynamicTransformation).transformable = true;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,-0.4f));
	*matC = glm::scale(*matC,glm::vec3(.7f,.7f,.8f));
	node->nodes.push_back(buildLeaf(matC,GeometryIslandGearsAxis,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, -0.05f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryIslandGearsSmall,id));

	return node;
}

NodeStatic* buildAirshipBalloonBelt(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryAirshipBalloonBelt,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.05f,1.05f,1.05f));
	node->nodes.push_back(buildLeaf(matC,GeometryAirshipBalloonBeltRivets,id));

	return node;
}

NodeStatic* buildLantern(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f, 0.0f));
	*matC = glm::scale(*matC,glm::vec3(0.01f));
	node->nodes.push_back(buildLeaf(matC,GeometryLantern,id));

	return node;
}

NodeStatic* buildAirshipBalloon(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(2.5f,2.5f,2.5f));
	node->nodes.push_back(buildLeaf(matC,GeometryAirshipBalloon,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(1.1f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildAirshipBalloonBelt(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::rotate(*matC,180.0f,glm::vec3(0.0f,1.0f,0.0f));
	*matC = glm::translate(*matC,glm::vec3(1.1f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildAirshipBalloonBelt(matC,id));

	return node;
}

NodeStatic* buildAirshipHolder(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.005f,1.005f,1.005f));
	node->nodes.push_back(buildLeaf(matC,GeometryAirshipHolderAxis,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryAirshipHolderBoxes,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(.972f,.972f,.972f));
	node->nodes.push_back(buildLeaf(matC,GeometryAirshipHolderRopes,id));

	return node;
}

NodeStatic* buildAirshipFan(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;

	node->dynamicTransformation = new ObjectTransformRotation(new glm::vec3(1.f, 0.f, 0.f),270.0f);
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;
	/*
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(.2f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(.1f,.1f,.1f));
	node->nodes.push_back(buildLeaf(matC,GeometryDoorKnob));
	*/
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, -0.09f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryAirshipFanBase,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-0.295f, 0.04f,0.06f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryAirshipFanSailRails,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-0.31f, 0.04f,0.06f));
	*matC = glm::scale(*matC,glm::vec3(.995f,.995f,.995f));
	node->nodes.push_back(buildLeaf(matC,GeometryAirshipFanSails,id));

	return node;
}

NodeStatic* buildAirshipBath(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryAirshipBath,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-1.f, -0.08f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(.8f,.8f,.8f));
	node->nodes.push_back(buildAirshipFan(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-0.7f, 0.08f,0.21f));
	*matC = glm::scale(*matC,glm::vec3(.28f,.28f,.28f));
	node->nodes.push_back(buildLeaf(matC,GeometryAirshipBoxes,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.5f, -0.12f,0.35f));
	*matC = glm::scale(*matC,glm::vec3(.33f,.33f,.33f));
	node->nodes.push_back(buildLeaf(matC,GeometryAirshipAnchor,id));

	return node;
}

NodeStatic* buildAirshipBathD(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.0f,1.0f,1.0f));
	node->nodes.push_back(buildLeaf(matC,GeometryAirshipBath,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-1.f, -0.08f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(.8f,.8f,.8f));
	NodeStatic *n = buildAirshipFan(matC,id);
	registerNode(n->id,n);
	node->nodes.push_back(n);

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-0.7f, 0.08f,0.21f));
	*matC = glm::scale(*matC,glm::vec3(.28f,.28f,.28f));
	node->nodes.push_back(buildLeaf(matC,GeometryAirshipBoxes,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.5f, -0.12f,0.35f));
	*matC = glm::scale(*matC,glm::vec3(.33f,.33f,.33f));

	ObjectTransformTranslation *t = new ObjectTransformTranslation(new glm::vec3(0.0f,-1.0f, 0.0f),25.0f);

	currentId++;
	n = buildLeafDynamic(matC,GeometryAirshipAnchor,currentId, t);
	registerNode(n->id,n);
	node->nodes.push_back(n);

	return node;
}

NodeStatic* buildAirship(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-0.05f, 2.5f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.35f,1.35f,1.35f));
	node->nodes.push_back(buildAirshipBalloon(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-0.05f, 1.4f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.6f,1.6f,1.6f));
	node->nodes.push_back(buildAirshipHolder(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.f,1.f,1.f));
	node->nodes.push_back(buildAirshipBath(matC,id));

	return node;
}

NodeStatic* buildAirshipD(glm::mat4 *transform, unsigned char incommingId){
	currentId++;
	unsigned char id = currentId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = new ObjectTransformTranslation(new glm::vec3(1.0f,0.0f,0.0f),1);
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-0.05f, 2.5f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.35f,1.35f,1.35f));
	node->nodes.push_back(buildAirshipBalloon(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-0.05f, 1.4f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.6f,1.6f,1.6f));
	node->nodes.push_back(buildAirshipHolder(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.f,1.f,1.f));
	node->nodes.push_back(buildAirshipBathD(matC,id));

	return node;
}

NodeStatic* buildGearsD(glm::mat4 *transform, unsigned char incommingId){
	currentId++;
	unsigned char id = currentId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-0.45f, 0.f, 0.f));
	*matC = glm::scale(*matC,glm::vec3(.25f,.25f,.25f));
	NodeStatic *n = buildGearBig(matC,id);
	registerNode(n->id,n);
	node->nodes.push_back(n);

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.19f, 0.f, 0.f));
	*matC = glm::scale(*matC,glm::vec3(.21f,.21f,.21f));
	n = buildGearSmall(matC,id);
	registerNode(n->id,n);
	node->nodes.push_back(n);


	return node;
}


NodeStatic* buildIslandMain(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.f,1.f,1.f));
	node->nodes.push_back(buildIslandMainBase(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, -0.7f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(.4f,.4f,.4f));
	node->nodes.push_back(buildIslandCloudMaker(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.f, -0.28f, .9f));
	*matC = glm::scale(*matC,glm::vec3(1.f, 1.f, 1.f));
	node->nodes.push_back(buildGearsD(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.5f, 0.42f, -0.3f));
	*matC = glm::scale(*matC,glm::vec3(.35f,.35f,.35f));
	node->nodes.push_back(buildTree(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-0.22f, 0.5f,0.2f));
	*matC = glm::scale(*matC,glm::vec3(.5f,.5f,.5f));
	node->nodes.push_back(buildHouse(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(.75f, 0.2f, 0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.f,1.f,1.f));
	node->nodes.push_back(buildLantern(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(2.0f, 0.1f,-0.35f));
	*matC = glm::scale(*matC,glm::vec3(.3f,.3f,.3f));
	NodeStatic *n = buildAirshipD(matC,id);
	registerNode(n->id,n);
	node->nodes.push_back(n);

	return node;
}


NodeStatic* buildTempScene(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	
	node->geometry = NULL;
	
	glm::mat4 *matC;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.f,1.f,1.f));
	node->nodes.push_back(buildIslandMain(matC,id));

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, 0.0f,0.0f));
	*matC = glm::scale(*matC,glm::vec3(1.f,1.f,1.f));
	node->nodes.push_back(buildClouds(matC,id));

	return node;
}

NodeStatic* buildClouds(glm::mat4 *transform, unsigned char incommingId){
	unsigned char id = incommingId;

	NodeStatic* node = new NodeStatic();
	node->transformation = transform;
	node->dynamicTransformation = NULL;
	node->id=id;
	/*
	currentId++;
	n = buildLeafDynamic(matC,GeometryAirshipAnchor,currentId, t);
	registerNode(n->id,n);
	node->nodes.push_back(n);
	*/


	
	node->geometry = NULL;
	
	glm::mat4 *matC;
	NodeStatic* n;

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(0.0f, -2.0f,-3.0f));
	*matC = glm::scale(*matC,glm::vec3(3.f,3.f,3.f));
	currentId++;
	n = buildLeafDynamic(matC,GeometryCloudA,currentId,new ObjectTransformBouncing(0.2f,10.0f,0.0f));
	registerNode(n->id,n);
	node->nodes.push_back(n);
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-5.0f, 1.0f,-3.0f));
	*matC = glm::scale(*matC,glm::vec3(2.f,2.f,2.f));
	currentId++;
	n = buildLeafDynamic(matC,GeometryCloudB,currentId,new ObjectTransformBouncing(0.2f,22.0f,30.0f));
	registerNode(n->id,n);
	node->nodes.push_back(n);
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(3.0f, -4.5f,4.0f));
	*matC = glm::scale(*matC,glm::vec3(4.f,4.f,4.f));
	currentId++;
	n = buildLeafDynamic(matC,GeometryCloudC,currentId,new ObjectTransformBouncing(0.1f,20.f,7.f));
	registerNode(n->id,n);
	node->nodes.push_back(n);

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-3.0f, 2.0f,3.0f));
	*matC = glm::scale(*matC,glm::vec3(2.5f,2.5f,2.5f));
	currentId++;
	n = buildLeafDynamic(matC,GeometryCloudD,currentId,new ObjectTransformBouncing(0.5f,1.0f,20.0f));
	registerNode(n->id,n);
	node->nodes.push_back(n);
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(6.0f, 4.0f,-1.0f));
	*matC = glm::scale(*matC,glm::vec3(4.f,4.f,4.f));
	currentId++;
	n = buildLeafDynamic(matC,GeometryCloudE,currentId,new ObjectTransformBouncing(0.2f,32.0f,30.0f));
	registerNode(n->id,n);
	node->nodes.push_back(n);
	
	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-8.0f, -20.0f,-20.0f));
	*matC = glm::scale(*matC,glm::vec3(25.f,20.f,20.f));
	currentId++;
	n = buildLeafDynamic(matC,GeometryCloudA,currentId,new ObjectTransformBouncing(0.1f,15.0f,15.0f));
	registerNode(n->id,n);
	node->nodes.push_back(n);

	matC = new glm::mat4(1.0f);
	*matC = glm::translate(*matC,glm::vec3(-30.0f, -5.0f,8.0f));
	*matC = glm::scale(*matC,glm::vec3(15.f,15.f,15.f));
	currentId++;
	n = buildLeafDynamic(matC,GeometryCloudB,currentId,new ObjectTransformBouncing(0.2f,26.0f,6.0f));
	registerNode(n->id,n);
	node->nodes.push_back(n);

	return node;
}


}