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
	mat.NonSolid(flags & NL_3D_TEXTURE_FLAG_NON_SOLID ? true : false);
	mat.SetTwoSided(flags & NL_3D_TEXTURE_FLAG_TOW_SIDED ? true : false);

	idMaterialStage stage;
	stage.Diffusemap() = fname;
	stage.SetAlphaTest(flags & NL_3D_TEXTURE_FLAG_ALPHA ? 0.1 : -1.0);
	if(flags & NL_3D_TEXTURE_FLAG_CUBE_MAP)
	{
		mat.NoShadows();
		mat.NoSelfShadow();
		mat.AmbientLight();
		mat.NonSolid();
		stage.Blend("blend");
	}
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

	if(config->sky_file && config->sky_file[0])
	{
		idMaterial mat;
		if(ConvertSkyMaterial(mat, config->sky_file) == 0)
		{
			mats.push_back(mat);
		}
		idMaterial envMat;
		if(ConvertSkyEnvMaterial(envMat, config->sky_file) == 0)
		{
			mats.push_back(envMat);
		}
	}

	std::ofstream os(mtrFile);
	for(const auto &mat : mats)
		os << mat << "\n";
	os.flush();
	os.close();
	return mats.size();
}

int idNETLizardConverter::ConvertSkyMaterial(idMaterial &mat, const char *file)
{
	NLuint flags = NL_3D_TEXTURE_FLAG_NORMAL;

	idStr fname("textures");
	fname /= gamename / file;
	fname.ReplaceExtension(nullptr);
	mat.Name() = fname;
	fname += ".tga";
	mat.Diffusemap() = fname;
	mat.NoShadows();
	mat.NoSelfShadow();
	mat.AmbientLight();
	mat.NonSolid();

	idMaterialStage stage;
	stage.Diffusemap() = fname;
	stage.Blend("blend");
	mat << stage;
	return 0;
}

int idNETLizardConverter::ConvertSkyEnvMaterial(idMaterial &mat, const char *file)
{
	idStr fname("textures");
	fname /= gamename / idStr::va("env_%s", file);
	fname.RemoveExtension();
	mat.Name() = fname;
	mat.NoShadows();
	mat("nooverlays");
	mat("forceOpaque");
	mat("noFragment");
	mat("noimpact");
	//mat.NonSolid();
	//mat("discrete");

	fname = "env";
	fname /= gamename / file;
	fname.RemoveExtension();
	idMaterialStage stage;
	stage.Diffusemap() = fname;
	stage.Blend("");
	stage.Texgen("skybox");
	stage("texgen", "wobblesky .05 .05 .05");
	mat << stage;
	return 0;
}

