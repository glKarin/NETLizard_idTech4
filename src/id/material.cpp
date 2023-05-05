#include "material.h"

const char *idMaterial::AREA_PORTAL_MATERIAL = "editor/visportal";
const char *idMaterial::NODRAW_MATERIAL = "common/nodraw";
const char *idMaterial::CAULK_MATERIAL = "common/caulk";

std::ostream & operator<<(std::ostream &o, const idMaterial &v)
{
	o << v.name << "\n";
	o << "{\n";
	o << "\tforceOpaque\n";
	o << "\tqer_editorimage\t\t" << v.diffusemap << "\n";
	o << "\tdiffusemap\t\t" << v.diffusemap << "\n";
	o << "}\n";
	return o;
}
