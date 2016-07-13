
cmake_minimum_required(VERSION 2.8.1)
project(AntTweakBar)

add_definitions(-D_UNIX -D__PLACEMENT_NEW_INLINE)
file(GLOB TW_SOURCES *.h)
set(TW_SOURCES ${TW_SOURCES} TwColors.cpp TwFonts.cpp TwOpenGL.cpp TwOpenGLCore.cpp TwBar.cpp TwMgr.cpp TwPrecomp.cpp LoadOGL.cpp LoadOGLCore.cpp TwEventGLFW.c TwEventGLUT.c TwEventSDL.c TwEventSDL12.c TwEventSDL13.c TwEventSFML.cpp TwEventX11.c)

add_library(AntTweakBar SHARED ${TW_SOURCES})
