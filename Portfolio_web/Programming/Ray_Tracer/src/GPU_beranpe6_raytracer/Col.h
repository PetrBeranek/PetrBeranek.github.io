
//----------------------------------------------------------------------------------------
/**
 * \file       Col.h
 * \author     Petr Beránek
 * \date       2016/01/04
 * \brief      File contains Col class used to represent colors.
 *
 *
*/
//----------------------------------------------------------------------------------------

#ifndef COLOR
#define COLOR

#define COL_BLACK	0
#define COL_WHITE	1
#define COL_RED		2
#define COL_GREEN	3
#define COL_BLUE	4
#define COL_DEFAULT	5

#define DEFAULT_COL_R 0.0f
#define DEFAULT_COL_G 0.0f
#define DEFAULT_COL_B 0.0f

#include <stdio.h>
/**
It is color
*/
class Col
{
public:
	float r, g, b, a;
	
	Col::Col()
	{
		r = g = b = 0;
	}
	
	Col::Col (Col& c)
	{
		r = c.r; 
		g = c.g;
		b = c.b;
		a = c.a;
	}
	
	Col::Col (int i)
	{
		switch (i){
		case COL_WHITE:
			r = 1; 
			g = 1;
			b = 1;
			a = 1.0f;
			break;
		case COL_BLACK:
			r = 0; 
			g = 0;
			b = 0;
			a = 1.0f;
			break;
		case COL_RED:
			r = 1; 
			g = 0;
			b = 0;
			a = 1.0f;
			break;
		case COL_GREEN:
			r = 0; 
			g = 1;
			b = 0;
			a = 1.0f;
			break;
		case COL_BLUE:
			r = 0; 
			g = 0;
			b = 1;
			a = 1.0f;
			break;
		case COL_DEFAULT:
			r = DEFAULT_COL_R; 
			g = DEFAULT_COL_G;
			b = DEFAULT_COL_B;
			a = 1.0f;
			break;
		}
	}
	
	Col::Col (float x, float y, float z)
	{
		r = x;
		g = y;
		b = z;
		a = 1;
	}
	
	Col::Col (float x, float y, float z, float w)
	{
		r = x;
		g = y;
		b = z;
		a = w;
	}

	Col copyColor()
	{
		return Col(r, g, b, a);
	}
	
	void addLoc(Col& c1)
	{
		r += c1.r;
		g += c1.g;
		b += c1.b;
		a += c1.a;
	}

	void mulLoc(float f)
	{
		r *= f;
		g *= f;
		b *= f;
		a *= f;
	}

	void setCol(Col& c1)
	{
		r = c1.r;
		g = c1.g;
		b = c1.b;
		a = c1.a;
	}
	
	void multiply(Col& c1)
	{
		r *= c1.r;
		g *= c1.g;
		b *= c1.b;
		a *= c1.a;
	}

	void addMultipliedLoc(Col& c1, float s)
	{
		r += s*c1.r;
		g += s*c1.g;
		b += s*c1.b;
		a += s*c1.a;
	}

	static void multiplySet(Col& c1, Col& c2, Col& out)
	{
		out.r = c1.r*c2.r;
		out.g = c1.g*c2.g;
		out.b = c1.b*c2.b;
		out.a = c1.a*c2.a;
	}


	static void Black(Col& output)
	{
		output.r = 0;
		output.g = 0;
		output.b = 0;
		output.a = 0;
	}
};

#endif