#ifndef _KARIN_MATERIAL_H
#define _KARIN_MATERIAL_H

#include <iostream>
#include <vector>

#include "str.h"

class idMaterialStage
{
	public:
		idStr & Diffusemap();
		const idStr & Diffusemap() const;
		void SetAlphaTest(float alpha);
		friend std::ostream & operator<<(std::ostream &o, const idMaterialStage &v);

	private:
		idStr diffusemap;
		float alphaTest = 0;
		
		friend class idMaterial;
};
using idMaterialStageList = std::vector<idMaterialStage>;

class idMaterial
{
	public:
		idStr & Name();
		idStr & Diffusemap();
		const idStr & Name() const;
		const idStr & Diffusemap() const;
		void SetTwoSided(bool twoSided);
		void SetNoCollision(bool noCollision);
		void SetLadder(bool ladder);
		idMaterialStage & operator[](int i);
		const idMaterialStage & operator[](int i) const;
		idMaterial & operator<<(const idMaterialStage &stage);

		friend std::ostream & operator<<(std::ostream &o, const idMaterial &v);

		static const char *AREA_PORTAL_MATERIAL;
		static const char *NODRAW_MATERIAL;
		static const char *CAULK_MATERIAL;

	private:
		idStr name;
		idStr diffusemap;
		bool twoSided = false;
		bool noCollision = false;
		bool ladder = false;
		idMaterialStageList stages;
};

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

inline void idMaterial::SetTwoSided(bool twoSided)
{
	this->twoSided = twoSided;
}

inline void idMaterial::SetNoCollision(bool noCollision)
{
	this->noCollision = noCollision;
}

inline void idMaterial::SetLadder(bool ladder)
{
	this->ladder = ladder;
}

inline idMaterialStage & idMaterial::operator[](int i)
{
	return stages[i];
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

#endif
