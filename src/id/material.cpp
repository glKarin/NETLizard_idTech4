#include "material.h"

const char *idMaterial::AREA_PORTAL_MATERIAL = "editor/visportal";
const char *idMaterial::NODRAW_MATERIAL = "common/nodraw";
const char *idMaterial::CAULK_MATERIAL = "common/caulk";

std::ostream & operator<<(std::ostream &o, const idMaterialStage &v)
{
	o << "\t{\n";
	o << "\t\tblend\t\tdiffusemap\n";
	o << "\t\tmap\t\t" << v.diffusemap << "\n";
	if(v.alphaTest > 0.0)
		o << "\t\talphaTest\t\t" << v.alphaTest << "\n";
	o << "\t}\n";

	return o;
}

std::ostream & operator<<(std::ostream &o, const idMaterial &v)
{
	bool hasStage = !v.stages.empty();

	o << v.name << "\n";
	o << "{\n";
	//o << "\tforceOpaque\n";
	o << "\tqer_editorimage\t\t" << v.diffusemap << "\n";
	if(!v.diffusemap.empty() && !hasStage)
		o << "\tdiffusemap\t\t" << v.diffusemap << "\n";

	o << "\tnullNormal\n";

	if(v.twoSided)
		o << "\ttwoSided\n";
	if(v.noCollision)
		o << "\tnonsolid\n";
	if(v.ladder)
		o << "\tladder\n";

	if(hasStage)
		o << "\n";
	for(const auto &stage : v.stages)
		o << stage;

	o << "}\n";
	return o;
}
