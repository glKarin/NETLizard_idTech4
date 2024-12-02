#include "material.h"

const char *idMaterial::AREA_PORTAL_MATERIAL = "editor/visportal";
const char *idMaterial::NODRAW_MATERIAL = "common/nodraw";
const char *idMaterial::CAULK_MATERIAL = "common/caulk";

std::ostream & operator<<(std::ostream &o, const idMaterialStage &v)
{
	o << "\t{\n";
	if(!v.blend.empty())
		o << "\t\tblend\t\t" << v.blend << "\n";
	idStr mapName = v.texgen == "skybox" ? "cameraCubeMap" : "map";
	o << "\t\t" + mapName + "\t\t" << v.diffusemap << "\n";
	if(v.alphaTest > 0.0)
		o << "\t\talphaTest\t\t" << v.alphaTest << "\n";

	if(!v.texgen.empty())
		o << "\t\ttexgen\t\t" << v.texgen << "\n";

	if(!v.parms.IsEmpty())
		o << v.parms;

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
	if(v.ladder)
		o << "\tladder\n";
	if(v.noShadows)
		o << "\tnoShadows\n";
	if(v.noSelfShadow)
		o << "\tnoSelfShadow\n";
	if(v.ambientLight)
		o << "\tambientLight\n";
	if(v.nonsolid)
		o << "\tnonsolid\n";

	if(!v.parms.IsEmpty())
		o << v.parms;

	if(hasStage)
		o << "\n";
	for(const auto &stage : v.stages)
		o << stage;

	o << "}\n";
	return o;
}
