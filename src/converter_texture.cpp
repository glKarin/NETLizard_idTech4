#include "converter.h"

#include <string.h>
#include <memory>
#include <functional>

#include "filesystem.h"
#include "buffer.h"

int idNETLizardConverter::ConvertTextureToTGA(const char *name, int i)
{
	NETLizard_Texture tex;
	memset(&tex, 0, sizeof(NETLizard_Texture));
	NLboolean ok = NL_FALSE;
    NLuchar color[4];

    if(nlGetTextureColor(game, i, color))
    {
        nlMakeColorTextureRGBA(&tex, color, config->tex_width, config->tex_width);
        ok = NL_TRUE;
    }
    else
    {
        idBuffer buffer;
        idStr srcPath = SourceFilePath(name);
        if(!ReadFile(buffer, srcPath))
            return -1;

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
                ok = nlLoadAndMakePNGRGBAData(buffer.Data(), buffer.Size(), &tex);
                break;
            default:
                return -2;
        }
    }

	if(!ok)
		return -2;

	idStr fname("textures");
	fname /= gamename;
    fname /= name;
    GetDir(fname);
	idStr target = TargetFilePath(fname);
	target.ReplaceExtension("tga");
    switch (tex.type /* config->texture_type */)
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
        case NL_TEXTURE_RAW:
            ok = nlSavePNGRGBADataToImageFile(&tex, target, TEXTURE_FILE_TYPE);
            break;
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
		"left",
		"back",
		"right",
		"up",
		"down",
	};
	const char *cubeSides[] = {
		"py",
		"nx",
		"ny",
		"px",
		"pz",
		"nz",
	};
	NLuchar *data;
	NLuchar *dataMirror = nullptr;
	int width;
	int len;
	NLuchar *rawData;
    bool delRawData;

    if(game == NL_RACING_EVOLUTION_3D)
    {
        rawData = tex->color_index.data;
        delRawData = false;
    }
    else
    {
        rawData = nlMakePixelData(tex, &len);
        delRawData = true;
    }
	if(!rawData)
		return false;
	if(tex->height > tex->width)
	{
		width = tex->width;
		int channel = tex->format != NL_RGB ? 4 : 3;
		data = (NLuchar *)malloc(width * width * channel);
		dataMirror = (NLuchar *)malloc(width * width * channel);
		for(int i = 0; i < tex->width; i++)
		{
			for(int m = 0; m < tex->width; m++)
			{
				NLuchar *src = rawData + i * tex->width * channel + m * channel;
				NLuchar *dst = data + i * width * channel + m * channel;
				NLuchar *dstMirror = data + i * width * channel + (width - 1 - m) * channel;
				memcpy(dst, src, sizeof(NLuchar) * channel);
				memcpy(dstMirror, src, sizeof(NLuchar) * channel);
	        }
		}
        if(delRawData)
		    free(rawData);
	}
	else if(tex->width > tex->height)
	{
		width = tex->width;
		int channel = tex->format != NL_RGB ? 4 : 3;
		data = (NLuchar *)malloc(width * width * channel);
		dataMirror = (NLuchar *)malloc(width * width * channel);
		for(int i = 0; i < tex->height; i++)
		{
			for(int m = 0; m < tex->width; m++)
			{
				NLuchar *src = rawData + i * tex->width * channel + m * channel;
				NLuchar *dst = data + i * width * channel + m * channel;
				NLuchar *dstMirror = dataMirror + i * width * channel + (width - 1 - m) * channel;
				memcpy(dst, src, sizeof(NLuchar) * channel);
				memcpy(dstMirror, src, sizeof(NLuchar) * channel);
			}
		}
		for(int i = tex->height; i < tex->width; i++)
		{
			for(int m = 0; m < tex->width; m++)
			{
				NLuchar *src = rawData + (tex->height - 1 - (i % tex->height)) * tex->width * channel + m * channel;
				NLuchar *dst = data + i * width * channel + m * channel;
				NLuchar *dstMirror = dataMirror + i * width * channel + (width - 1 - m) * channel;
				memcpy(dst, src, sizeof(NLuchar) * channel);
				memcpy(dstMirror, src, sizeof(NLuchar) * channel);
			}
		}
        if(delRawData)
		    free(rawData);
	}
	else
	{
		data = rawData;
		width = tex->width;
	}

	auto deletor = [](NLuchar *x) {
		free(x);
	};
	std::unique_ptr<NLuchar, std::function<void(NLuchar *)> > ptr(data, deletor);
	std::unique_ptr<NLuchar, std::function<void(NLuchar *)> > ptrMirror(dataMirror, deletor);
	NLuchar *datas[] = {
		data,
		dataMirror,
	};

	int i = 0;
	for(const auto &n : cameraSides)
	{
		NLuchar *d = datas[i % 2];
		if(!d)
			d = datas[0];
		if(!idNETLizardConverter::WriteEnvTextureSide(d, width, tex->format, fname.c_str(), n))
			return false;
		i++;
	}
	i = 0;
	for(const auto &n : cubeSides)
	{
		NLuchar *d = datas[i % 2];
		if(!d)
			d = datas[0];
		if(!idNETLizardConverter::WriteEnvTextureSide(d, width, tex->format, fname.c_str(), n))
			return false;
		i++;
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
    idStr file;
    int res;

    res = 0;
    format = config->tex_path_format;

    if(game == NL_RACING_EVOLUTION_3D)
    {
        NLint len;
        const char **texes = nlGetRE3DMapTexes(&len);
        for(i = 0; i < len; i++)
        {
            file = idStr::va(format, texes[i]);
            if(ConvertTextureToTGA(file.c_str(), i) == 0)
                res++;
        }
    }
    else
    {
        for(i = 1; i <= config->tex_count; i++)
        {
            file = idStr::va(format, i);
            if(ConvertTextureToTGA(file.c_str(), i) == 0)
                res++;
        }
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
            ok = nlLoadAndMakePNGRGBAData(buffer.Data(), buffer.Size(), &tex);
            break;
        default:
            return -2;
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
            ok = nlSavePNGRGBADataToImageFile(&tex, target, TEXTURE_FILE_TYPE);
            break;
        default:
            ok = NL_FALSE;
            break;
    }

	ok = WriteEnvTexture(&tex, name) && ok;

	nlDeleteNETLizardTexture(&tex);
	return ok ? 0 : -3;
}

