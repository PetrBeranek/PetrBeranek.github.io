
//----------------------------------------------------------------------------------------
/**
 * \file       Material.h
 * \author     Petr Beránek
 * \date       2016/01/04
 * \brief      File contains Material class and all about materials
 *
 *	File contains Material class, material buffer, struct fot compact Material and methods co compact materials.
*/
//----------------------------------------------------------------------------------------

#ifndef MATERIAL
#define MATERIAL

#define BASE_MATERIAL_COUNT 16

#include "Col.h"
#include <vector>


struct MaterialCompact
{
	float diff[3];
	float kD[3];
	float kS[3];

	float shine, reflection, refraction, transparency;
};

class Material;

bool materialsInitialized = false;
std::vector<Material *> GLOBAL_BUFFER_MATERIAL;

class Material
{
public:
	/// diffuse color
	Col diff;
	/// coeficients for diffuse color
	Col kD;
	/// coeficients for specular color
	Col kS;
	/// exponent for specular color
	float shine;
	/// how reflective material is
	float reflection;
	/// how transparent object is (1.0-solid, 0.0 transparent)
	float transparency;
	/// refraction index
	float refraction;

	static void initializeMaterials()
	{
		materialsInitialized = true;
		Material *m;
		for(int i = 0; i < BASE_MATERIAL_COUNT; i++)
		{
			m = new Material(i);
			GLOBAL_BUFFER_MATERIAL.push_back(m);

		}
	}

	static int buildMaterial(Col& d, Col& D, Col& S, float sh, float refl, float trans, float refr)
	{
		
		int id = GLOBAL_BUFFER_MATERIAL.size();
		Material *m = new Material(d, D, S, sh, refl, trans, refr);
		GLOBAL_BUFFER_MATERIAL.push_back(m);

		return id;
	}

	static Material *getMaterial(int id)
	{
		
		if(id < (int)GLOBAL_BUFFER_MATERIAL.size())
		{
			return GLOBAL_BUFFER_MATERIAL[id];
		}else{
			return NULL;
		}

	}

	static int getMaterialIndex(int id)
	{
		if(id < (int)GLOBAL_BUFFER_MATERIAL.size())
		{
			return id;
		}else{
			return 0;
		}
	}
	/**
	Fill array by copying and compacting material buffer.
	*/
	static void fillCompactArray(MaterialCompact *out)
	{
		MaterialCompact *mC;
		Material *m;

		int i;

		i = 0;
		std::vector<Material *>::iterator itM = GLOBAL_BUFFER_MATERIAL.begin();
		for(; itM != GLOBAL_BUFFER_MATERIAL.end();itM++)
		{
			m = *itM;
			mC = out + i;
			mC->diff[0] = m->diff.r;
			mC->diff[1] = m->diff.g;
			mC->diff[2] = m->diff.b;
			
			mC->kD[0] = m->kD.r;
			mC->kD[1] = m->kD.g;
			mC->kD[2] = m->kD.b;
			
			mC->kS[0] = m->kS.r;
			mC->kS[1] = m->kS.g;
			mC->kS[2] = m->kS.b;
			
			mC->shine = m->shine;
			mC->reflection = m->reflection;
			mC->refraction = m->refraction;
			mC->transparency = m->transparency;
			i++;
		}
	}

private:
	Material::Material(int i)
	{
		switch (i){
		case 0:
			diff = Col(0.0f, 1.0f, 0.0f);
			kD = Col(1.0f, 1.0f, 1.0f);
			kS = Col(1.0f, 1.0f, 1.0f);
			shine = 30;
			reflection = 1.0f;
			transparency = 1.0f;
			refraction = 1.0f;
			break;
		case 1:
			diff = Col(0.8f, 0.05f, 0.1f);
			kD = Col(1.0f, 1.0f, 1.0f);
			kS = Col(1.0f, 1.0f, 1.0f);
			shine = 50;
			reflection = 0.75f;
			transparency = 1.0f;
			refraction = 1.0f;
			break;
		case 2:
			diff = Col(0.15f, 0.2f, 0.8f);
			kD = Col(1.0f, 1.0f, 1.0f);
			kS = Col(1.0f, 1.0f, 1.0f);
			shine = 30;
			reflection = 1.0f;
			transparency = 1.0f;
			refraction = 1.0f;
			break;
		case 3:
			diff = Col(01.0f, 01.0f, 01.0f);
			kD = Col(1.0f, 1.0f, 1.0f);
			kS = Col(1.0f, 1.0f, 1.0f);
			shine = 30;
			reflection = 0.50f;
			transparency = 0.0f;
			refraction = 01.010f;
			break;
		case 4:
			diff = Col(.3f,1.0f,.8f);
			kD = Col(1.0f, 1.0f, 1.0f);
			kS = Col(1.0f, 1.0f, 1.0f);
			shine = 30;
			reflection = 1.0f;
			transparency = 1.0f;
			refraction = 1.0f;
			break;
		case 5:
			diff = Col(.81f,0.21f,.1f);
			kD = Col(1.0f, 1.0f, 1.0f);
			kS = Col(1.0f, 1.0f, 1.0f);
			shine = 30;
			reflection = 1.0f;
			transparency = 1.0f;
			refraction = 1.0f;
			break;
		case 6:
			diff = Col(1.0f, 0.60f, 0.60f);
			kD = Col(1.0f, 1.0f, 1.0f);
			kS = Col(1.0f, 1.0f, 1.0f);
			shine = 30;
			reflection = 0.0f;
			transparency = 0.0f;
			refraction = 10.00f;
		case 7:
			diff = Col(01.0f, 1.0f, 1.0f);
			kD = Col(0.50f, 00.50f, 01.0f);
			kS = Col(0.30f, 0.30f, 0.30f);
			shine = 30;
			reflection = 0.0f;
			transparency = 0.90f;
			refraction = 01.010f;
			break;
		case 8:
			diff = Col(0.10f, 0.50f, .10f);
			kD = Col(01.0f, 01.0f, 01.0f);
			kS = Col(01.0f, 01.0f, 01.0f);
			shine = 30;
			reflection = 0.40f;
			transparency = 01.0f;
			refraction = 01.0f;
			break;
		case 9:
			diff = Col(0.50f, 0.10f, .10f);
			kD = Col(01.0f, 01.0f, 01.0f);
			kS = Col(01.0f, 01.0f, 01.0f);
			shine = 30;
			reflection = 0.40f;
			transparency = 01.0f;
			refraction = 01.0f;
			break;
		case 10:
			diff = Col(0.80f, 0.80f, .80f);
			kD = Col(01.0f, 01.0f, 01.0f);
			kS = Col(01.0f, 01.0f, 01.0f);
			shine = 30;
			reflection = 0.40f;
			transparency = 01.0f;
			refraction = 01.0f;
			break;
		case 11:
			diff = Col(0.95f, 0.1f, 0.05f);
			kD = Col(1.0f, 1.0f, 1.0f);
			kS = Col(1.0f, 1.0f, 1.0f);
			shine = 30;
			reflection = 0.10f;
			transparency = 1.0f;
			refraction = 1.0f;
			break;
		case 12:
			diff = Col(0.05f, 0.95f, 0.05f);
			kD = Col(1.0f, 1.0f, 1.0f);
			kS = Col(1.0f, 1.0f, 1.0f);
			shine = 30;
			reflection = 0.10f;
			transparency = 1.0f;
			refraction = 1.0f;
			break;
		case 13:
			diff = Col(0.05f, 0.1f, 0.95f);
			kD = Col(1.0f, 1.0f, 1.0f);
			kS = Col(1.0f, 1.0f, 1.0f);
			shine = 30;
			reflection = 0.10f;
			transparency = 1.0f;
			refraction = 1.0f;
			break;
		case 14:
			diff = Col(0.75f, 0.75, 0.75f);
			kD = Col(1.0f, 1.0f, 1.0f);
			kS = Col(1.0f, 1.0f, 1.0f);
			shine = 30;
			reflection = 0.10f;
			transparency = 1.0f;
			refraction = 1.0f;
			break;
		case 15:
			diff = Col(01.0f, 01.0, 01.0f);
			kD = Col(1.0f, 1.0f, 1.0f);
			kS = Col(1.0f, 1.0f, 1.0f);
			shine = 20;
			reflection = 0.20f;
			transparency = 0.050f;
			refraction = 1.0f;
			break;

		}
	}

	Material::Material(Col& d, Col& D, Col& S, float sh, float refl, float trans, float refr)
	{
		diff = Col(d);
		kD = Col(D);
		kS = Col(S);
		shine = sh;
		reflection = refl;
		transparency = trans;
		refraction = refr;
	}
};
#endif