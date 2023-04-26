#include "converter.h"

#include <string.h>
#include "filesystem.h"

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
            ok = nlLoadTextureV3Data(buffer.Data(), buffer.Size(), i, &tex);
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
    return res;
}