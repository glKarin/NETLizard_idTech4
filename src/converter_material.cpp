#include "converter.h"

#include <fstream>
#include "material.h"

int idNETLizardConverter::ConvertMaterial(idMaterial &mat, const char *file, int index)
{
	NLuint flags = NL_3D_TEXTURE_FLAG_NORMAL;

	idStr fname("textures");
	fname /= gamename / file;
	fname.ReplaceExtension(nullptr);
	mat.Name() = fname;
	fname += ".tga";
	mat.Diffusemap() = fname;
	flags = nlGetTextureFlag(game, index);
	mat.SetLadder(flags & NL_3D_TEXTURE_FLAG_LADDER ? true : false);
	mat.SetNoCollision(flags & NL_3D_TEXTURE_FLAG_NON_SOLID ? true : false);
	mat.SetTwoSided(flags & NL_3D_TEXTURE_FLAG_TOW_SIDED ? true : false);

	idMaterialStage stage;
	stage.Diffusemap() = fname;
	stage.SetAlphaTest(flags & NL_3D_TEXTURE_FLAG_ALPHA ? 0.1 : -1.0);
	mat << stage;
	return 0;
}

int idNETLizardConverter::ConvertMaterials()
{
    int i;
    const char *format;
    const char *file;
    int res;
	idMaterialList mats;
	
    res = 0;

	idStr path("materials");
	path /= gamename;
	path += ".mtr";
	GetDir(path);
	idStr mtrFile = TargetFilePath(path);
	format = config->tex_path_format;
    for(i = 1; i <= config->tex_count; i++)
	{
		idMaterial mat;
        file = idStr::va(format, i);
		if(ConvertMaterial(mat, file, i) == 0)
		{
			mats.push_back(mat);
		}
	}
	std::ofstream os(mtrFile);
	for(const auto &mat : mats)
		os << mat << "\n";
	os.flush();
	os.close();
	return mats.size();
}
