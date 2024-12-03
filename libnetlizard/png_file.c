#include "netlizard.h"

#include "priv_local.h"
#include "soil/stb_image_aug.h"
#include "soil/image.h"

char * nlReadAndHandlePNGFile(const char *name, NLint *rlen)
{
    array arr;
    jlong len;
    char *data;

    len = file_get_contents(name, &arr);
    if(len <= 0)
        return NULL;

    if(rlen)
        *rlen = array_size(&arr);
    data = nlLoadAndHandlePNGData(arr.array, array_size(&arr));
    delete_array(&arr);

    return data;
}

char * nlLoadAndHandlePNGData(const char *arr, NLint len)
{
    if(nlIsPNG(arr, len))
    {
        nllogfln("Encode NETLizard 2D image PNG");
    }
    else if(nlIsNLPNG(arr, len))
    {
        nllogfln("Decode NETLizard 2D image PNG");
    }
    else
    {
        nlflogfln(NL_LOG_ERR, "Unsupported PNG");
        return NULL;
    }

    char *res = NEW_II(char, len);
    return nlEncodeDecodeData(arr, res, len);
}

NLboolean nlConvertAndHandlePNGFile(const char *from, const char *to)
{
    char *data;
    NLint rlen;
    NLboolean res;

    data = nlReadAndHandlePNGFile(from, &rlen);
    if(!data)
        return NL_FALSE;

    array arr;
    make_array(&arr, 1, rlen, data);
    res = file_put_contents(to, &arr) > 0;

    free(data);

    return res;
}

NLboolean nlSaveAndHandlePNGData(const char *in_data, NLint len, const char *to)
{
    char *data;
    NLboolean res;

    data = nlLoadAndHandlePNGData(in_data, len);
    if(!data)
        return NL_FALSE;

    array arr;
    make_array(&arr, 1, len, data);
    res = file_put_contents(to, &arr) > 0;

    free(data);
    return res;
}

NLboolean nlLoadAndMakePNGRGBAData(const char *arr, NLint len, NETLizard_Texture *tex)
{
    char *decodeData = NULL;
    NETLizard_Texture_Type type;

    if(nlIsPNG(arr, len))
    {
        nllogfln("Read NETLizard 2D image PNG");
        type = NL_TEXTURE_NORMAL_PNG;
    }
    else if(nlIsNLPNG(arr, len))
    {
        nllogfln("Read and decode NETLizard 2D image PNG");
        type = NL_TEXTURE_ENCODE_PNG;
        decodeData = NEW_II(char, len);
        arr = nlEncodeDecodeData(arr, decodeData, len);
    }
    else
    {
        nlflogfln(NL_LOG_ERR, "Unsupported PNG");
        return NL_FALSE;
    }

    int w, h;
    int comp;
    stbi_uc *data = stbi_png_load_from_memory((const stbi_uc *)arr, len, &w, &h, &comp, 4);
    NLboolean res = data != NULL;
    if(data)
    {
        memset(tex, 0, sizeof(*tex));
        tex->type = type;
        tex->width = w;
        tex->height = h;
        tex->depth = 1;
        tex->format = NL_RGBA;
        tex->color_index.data = data;
        tex->color_index.count = w * h * 4;
    }

    free(decodeData);

    return res;
}

NLboolean nlSavePNGRGBADataToImageFile(const NETLizard_Texture *tex, const char *to, NLenum img_type)
{
    int channel = tex->format != NL_RGB ? SOIL_LOAD_RGBA : SOIL_LOAD_RGBA;
    NLboolean res = SOIL_save_image(to, img_type, tex->width, tex->height, channel, tex->color_index.data);
    return res;
}

