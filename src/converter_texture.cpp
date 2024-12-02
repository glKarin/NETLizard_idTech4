#include "converter.h"

#include <string.h>
#include <memory>

#include "filesystem.h"
#include "buffer.h"

int idNETLizardConverter::ConvertTextureToTGA(const char *name, int i)
{
	idBuffer buffer;
    idStr srcPath = SourceFilePath(name);
	if(!ReadFile(buffer, srcPath))
        return -1;

	NETLizard_Texture tex;
	memset(&tex, 0, sizeof(NETLizard_Texture));
	NLboolean ok = NL_FALSE;
    switch (config->texture_type)
    {
        case NL_TEXTURE_3D_ENGINE_V2:
            ok = nlLoadTextureV2Data(buffer.Data(), buffer.Size(), &tex);
                break;
        case NL_TEXTURE_3D_ENGINE_V3:
            ok = nlLoadTextureV3Data(buffer.Data(), buffer.Size(), game == NL_CONTR_TERRORISM_3D_EPISODE_3 ? -1 : i, &tex);
                break;
        case NL_TEXTURE_3D_ENGINE_V3_COMPRESS:
            ok = nlLoadCompressTextureV3Data(buffer.Data(), buffer.Size(), &tex);
                break;
        case NL_TEXTURE_ENCODE_PNG:
        case NL_TEXTURE_NORMAL_PNG:
        default:
        return -2;
            break;
    }
	if(!ok)
		return -2;

	idStr fname("textures");
	fname /= gamename;
    fname /= name;
    GetDir(fname);
	idStr target = TargetFilePath(fname);
	target.ReplaceExtension("tga");
    switch (config->texture_type)
    {
        case NL_TEXTURE_3D_ENGINE_V2:
            ok = nlSaveTextureV2DataToImageFile(&tex, target, TEXTURE_FILE_TYPE);
                break;
        case NL_TEXTURE_3D_ENGINE_V3:
            ok = nlSaveTextureV3DataToImageFile(&tex, target, TEXTURE_FILE_TYPE);
                break;
        case NL_TEXTURE_3D_ENGINE_V3_COMPRESS:
            ok = nlSaveTextureV3CompressDataToImageFile(&tex, target, TEXTURE_FILE_TYPE);
                break;
        case NL_TEXTURE_ENCODE_PNG:
        case NL_TEXTURE_NORMAL_PNG:
        default:
            ok = NL_FALSE;
            break;
    }

	nlDeleteNETLizardTexture(&tex);
	return ok ? 0 : -3;
}

bool idNETLizardConverter::WriteEnvTexture(const NETLizard_Texture *tex, const char *name)
{
	idStr fname("env");
	fname /= gamename;
    fname /= name;
	fname.RemoveExtension();
    GetDir(fname);
	const char *cameraSides[] = {
		"forward",
		"back",
		"left",
		"right",
		"up",
		"down",
	};
	const char *cubeSides[] = {
		"nx",
		"ny",
		"nz",
		"px",
		"py",
		"pz",
	};
	bool same = tex->width == tex->height;
	NLuchar *data;
	int width;
	int len;
	NLuchar *rawData = nlMakePixelData(tex, &len);
	if(!rawData)
		return false;
	if(same)
	{
		data = rawData;
		width = tex->width;
	}
	else
	{
		width = tex->width > tex->height ? tex->height : tex->width;
		int channel = tex->format != NL_RGB ? 4 : 3;
		data = (NLuchar *)malloc(width * width * channel);
		for(int i = 0; i < width; i++)
		{
			for(int m = 0; m < width; m++)
			{
				NLuchar *src = rawData + i * tex->width * channel + m * channel;
				NLuchar *dst = data + i * width * channel + m * channel;
				memcpy(dst, src, sizeof(NLuchar) * channel);
			}
		}
		free(rawData);
	}

	auto deletor = [](NLuchar *x) {
		free(x);
	};
	std::unique_ptr<NLuchar, std::function<void(NLuchar *)> > ptr(data, deletor);

	for(const auto &n : cameraSides)
	{
		if(!idNETLizardConverter::WriteEnvTextureSide(data, width, tex->format, fname.c_str(), n))
			return false;
	}
	for(const auto &n : cubeSides)
	{
		if(!idNETLizardConverter::WriteEnvTextureSide(data, width, tex->format, fname.c_str(), n))
			return false;
	}
	return true;
}

bool idNETLizardConverter::WriteEnvTextureSide(const void *data, int width, int format, const char *path, const char *what)
{
	idStr fname(path);
	fname += "_";
	fname += what;
	idStr target = TargetFilePath(fname);
	target += ".tga";
	return nlSavePixelDataToImageFile((const NLuchar *)data, width, width, (NETLizard_Texture_format_e)format, TEXTURE_FILE_TYPE, target.c_str());
}

int idNETLizardConverter::ConvertTextures()
{
    int i;
    const char *format;
    const char *file;
    int res;

    res = 0;
    format = config->tex_path_format;
    for(i = 1; i <= config->tex_count; i++)
    {
        file = idStr::va(format, i);
        if(ConvertTextureToTGA(file, i) == 0)
            res++;
    }

	if(config->sky_file && config->sky_file[0])
	{
		if(ConvertSkyTextureToTGA(config->sky_file) == 0)
            res++;
	}
    return res;
}

int idNETLizardConverter::ConvertSkyTextureToTGA(const char *name)
{
	idBuffer buffer;
    idStr srcPath = SourceFilePath(name);
	if(!ReadFile(buffer, srcPath))
        return -1;

	NETLizard_Texture tex;
	memset(&tex, 0, sizeof(NETLizard_Texture));
	NLboolean ok = NL_FALSE;
    switch (config->texture_type)
    {
        case NL_TEXTURE_3D_ENGINE_V2:
            ok = nlLoadTextureV2Data(buffer.Data(), buffer.Size(), &tex);
                break;
        case NL_TEXTURE_3D_ENGINE_V3:
            ok = nlLoadTextureV3Data(buffer.Data(), buffer.Size(), -1, &tex);
                break;
        case NL_TEXTURE_3D_ENGINE_V3_COMPRESS:
            ok = nlLoadCompressTextureV3Data(buffer.Data(), buffer.Size(), &tex);
                break;
        case NL_TEXTURE_ENCODE_PNG:
        case NL_TEXTURE_NORMAL_PNG:
        default:
        return -2;
            break;
    }
	if(!ok)
		return -2;

	idStr fname("textures");
	fname /= gamename;
    fname /= name;
    GetDir(fname);
	idStr target = TargetFilePath(fname);
	target.ReplaceExtension("tga");
    switch (config->texture_type)
    {
        case NL_TEXTURE_3D_ENGINE_V2:
            ok = nlSaveTextureV2DataToImageFile(&tex, target, TEXTURE_FILE_TYPE);
                break;
        case NL_TEXTURE_3D_ENGINE_V3:
            ok = nlSaveTextureV3DataToImageFile(&tex, target, TEXTURE_FILE_TYPE);
                break;
        case NL_TEXTURE_3D_ENGINE_V3_COMPRESS:
            ok = nlSaveTextureV3CompressDataToImageFile(&tex, target, TEXTURE_FILE_TYPE);
                break;
        case NL_TEXTURE_ENCODE_PNG:
        case NL_TEXTURE_NORMAL_PNG:
        default:
            ok = NL_FALSE;
            break;
    }

	ok = WriteEnvTexture(&tex, name) && ok;

	nlDeleteNETLizardTexture(&tex);
	return ok ? 0 : -3;
}

