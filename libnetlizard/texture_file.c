#include "netlizard.h"

#include "priv_local.h"
#include "soil/image.h"

NLboolean nlSavePixelDataToImageFile(const NLuchar *data, NLint width, NLint height, NETLizard_Texture_format format, NLenum img_type, const char *to)
{
    int channel = format != NL_RGB ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB;
    NLboolean res = SOIL_save_image(to, img_type, width, height, channel, data);
    return res;
}
