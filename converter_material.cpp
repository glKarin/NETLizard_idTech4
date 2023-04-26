#include "converter.h"

#include <vector>
#include <fstream>
#include "material.h"

int idNETLizardConverter::ConvertMaterial(idMaterial &mat, const char *file)
{
	idStr fname("textures");
	fname /= gamename / file;
	fname.ReplaceExtension(nullptr);
	mat.name = fname;
	fname += ".tga";
	mat.diffusemap = fname;
	return 0;
}

int idNETLizardConverter::ConvertMaterials()
{
    int i;
    const char *format;
    const char *file;
    int res;
	idMaterial mat;
	std::vector<idMaterial> mats;
	
    res = 0;

	idStr path("materials");
	path /= gamename;
	path += ".mtr";
	GetDir(path);
	idStr mtrFile = TargetFilePath(path);
	format = config->tex_path_format;
    for(i = 1; i <= config->tex_count; i++)
	{
        file = idStr::va(format, i);
		if(ConvertMaterial(mat, file) == 0)
		{
			mats.push_back(mat);
			i++;
		}
	}
	std::ofstream os(mtrFile);
	for(const auto &mat : mats)
		os << mat << "\n";
	os.flush();
	os.close();
	return mats.size();
}
