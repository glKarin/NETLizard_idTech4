#include "material.h"


std::ostream & operator<<(std::ostream &o, const idMaterial &v)
{
	o << v.name << "\n";
	o << "{\n";
	o << "\tqer_editorimage\t\t" << v.diffusemap << "\n";
	o << "\tdiffusemap\t\t" << v.diffusemap << "\n";
	o << "}\n";
	return o;
}
