
//----------------------------------------------------------------------------------------
/**
 * \file       CloudFactory.h
 * \author     Petr Beránek
 * \date       2015/05/09
 * \brief      CloudFactory.h and CloudFactory.cpp are core parts of CloudFactory application.
 *
 *  These files contains basic part of application like game loop, objects in scene, callbacks for keyboard and mouse events.
 *
*/
//----------------------------------------------------------------------------------------

#ifndef __CLOUD_FACTORY_H
#define __CLOUD_FACTORY_H

#include "pgr.h"

namespace beranpe6
{

void initializeApplication();
void initializeCamera(CameraData *data);
void updateCameraLoc();
void changeCameraAngles(float dx, float dy);
glm::mat4 getCameraTransformation();
void finalizeApplication(void);
void timerCallback(int);
void reshapeCallback(int newWidth, int newHeight);
void displayCallback();
void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY);
void keyboardUpCallback(unsigned char keyPressed, int mouseX, int mouseY);
void specialKeyboardCallback(int specKeyPressed, int mouseX, int mouseY);
void specialKeyboardUpCallback(int specKeyPressed, int mouseX, int mouseY);
void mouseMovementPas(int x, int y);
void mouseMovementAct(int x, int y);
void mouseClick(int buttonPressed, int buttonState, int mouseX, int mouseY );
void drawWindowContents();
void testStart();
void buildScene();

void appendCamera(DynamicObject *obj);
void selection(int x, int y);

}
#endif