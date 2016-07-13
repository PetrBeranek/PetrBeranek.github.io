
//----------------------------------------------------------------------------------------
/**
 * \file       Bitmap.h
 * \author     Petr Beránek
 * \date       2016/01/04
 * \brief      File contains Bitmap class used to represent rendered image.
 *
 *
*/
//----------------------------------------------------------------------------------------

#ifndef BITMAP
#define BITMAP

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

class Bitmap
{
public:
  int width;			
  int height;			
  uchar4 *pixels;	

  Bitmap::Bitmap()
  {
	  width = 0;
	  height = 0;
	  pixels = NULL;
  }

  Bitmap::Bitmap(int w, int h)
  {
	  width = w;
	  height = h;
	  pixels = new uchar4[width * height];
  }

  void setBMapPixel(int row, int col, unsigned char r, unsigned char g, unsigned char b )
  {	
	  pixels[row * width + col].x = r;
	  pixels[row * width + col].y = g;
	  pixels[row * width + col].z = b;
	  pixels[row * width + col].w = 255;
  }

  void clear()
  {
	  for(int i = 0; i < width*height; i++)
	  {
		  pixels[i].x = 0;
		  pixels[i].y = 0;
		  pixels[i].z = 0;
		  pixels[i].w = 255;
	  }
  }

};


#endif