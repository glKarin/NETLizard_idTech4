#ifndef _KARIN_MATERIAL_H
#define _KARIN_MATERIAL_H

#include <iostream>

#include "str.h"
#include "list.h"
#include "dict.h"

class idMaterialStage
{
	public:
		idMaterialStage();
		idStr & Diffusemap();
		const idStr & Diffusemap() const;
		void SetAlphaTest(float alpha);
		void Blend(const char *blend);
		void Texgen(const char *texgen);
		friend std::ostream & operator<<(std::ostream &o, const idMaterialStage &v);
		idMaterialStage & operator()(const char *name, const char *value = "");

	private:
		idStr blend = "diffusemap";
		idStr diffusemap;
		float alphaTest = 0;
		idStr texgen;
		idDict parms;
		
		friend class idMaterial;
};
using idMaterialStageList = idList<idMaterialStage>;

class idMaterial
{
	public:
		idMaterial();
		idStr & Name();
		idStr & Diffusemap();
		const idStr & Name() const;
		const idStr & Diffusemap() const;
		void SetTwoSided(bool twoSided);
		void SetLadder(bool ladder);
		void NoShadows(bool b = true);
		void NoSelfShadow(bool b = true);
		void AmbientLight(bool b = true);
		void NonSolid(bool b = true);
		idMaterialStage & operator[](int i);
		const idMaterialStage & operator[](int i) const;
		idMaterial & operator<<(const idMaterialStage &stage);
		idMaterial & operator()(const char *name, const char *value = "");

		friend std::ostream & operator<<(std::ostream &o, const idMaterial &v);

		static const char *AREA_PORTAL_MATERIAL;
		static const char *NODRAW_MATERIAL;
		static const char *CAULK_MATERIAL;

	private:
		idStr name;
		idStr diffusemap;
		bool twoSided = false;
		bool ladder = false;
		bool noShadows = false;
		bool noSelfShadow = false;
		bool ambientLight = false;
		bool nonsolid = false;
		idDict parms;
		idMaterialStageList stages;
};
using idMaterialList = idList<idMaterial>;



inline idMaterialStage::idMaterialStage()
{
	parms.NoQuota(true);
}

inline idStr & idMaterial::Name()
{
	return name;
}

inline idStr & idMaterial::Diffusemap()
{
	return diffusemap;
}

inline const idStr & idMaterial::Name() const
{
	return name;
}

inline const idStr & idMaterial::Diffusemap() const
{
	return diffusemap;
}

inline void idMaterialStage::Blend(const char *b)
{
	blend = b;
}

inline void idMaterialStage::Texgen(const char *b)
{
	texgen = b;
}

inline void idMaterial::SetTwoSided(bool twoSided)
{
	this->twoSided = twoSided;
}

inline void idMaterial::SetLadder(bool ladder)
{
	this->ladder = ladder;
}

inline idMaterialStage & idMaterial::operator[](int i)
{
	return stages[i];
}

inline idMaterialStage & idMaterialStage::operator()(const char *name, const char *value)
{
	parms.Set(name, value);
	return *this;
}



inline idMaterial::idMaterial()
{
	parms.NoQuota(true);
}

inline const idMaterialStage & idMaterial::operator[](int i) const
{
	return stages[i];
}

inline idMaterial & idMaterial::operator<<(const idMaterialStage &stage)
{
	stages.push_back(stage);
	return *this;
}

inline void idMaterialStage::SetAlphaTest(float alpha)
{
	alphaTest = alpha;
}

inline idStr & idMaterialStage::Diffusemap()
{
	return diffusemap;
}

inline const idStr & idMaterialStage::Diffusemap() const
{
	return diffusemap;
}

inline void idMaterial::NoShadows(bool b)
{
	noShadows = b;
}

inline void idMaterial::NoSelfShadow(bool b)
{
	noSelfShadow = b;
}

inline void idMaterial::AmbientLight(bool b)
{
	ambientLight = b;
}

inline void idMaterial::NonSolid(bool b)
{
	nonsolid = b;
}

inline idMaterial & idMaterial::operator()(const char *name, const char *value)
{
	parms.Set(name, value);
	return *this;
}

#endif
