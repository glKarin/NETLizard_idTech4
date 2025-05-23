#include "converter.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <iostream>

#include "filesystem.h"
#include "buffer.h"
#include "vector.h"
#include "id/mapfile.h"
#include "math/maths.h"

int idNETLizardConverter::TEXTURE_FILE_TYPE = 0; // tga

idNETLizardConverter::idNETLizardConverter(NETLizard_Game game, const char *sourceDir, const char *targetDir)
    : game(game),
        sourceDir(sourceDir),
        targetDir(targetDir),
        config(nullptr),
		genPortalBrush(false),
		version(MAP_VERSION_DOOM3)
{
    config = nlGet3DGameModelConfig(this->game);
    gamename = idStr(nlGet3DGameLiteName(this->game)).Replace(" ", "").Replace(":", "_").ToLower();
}

bool idNETLizardConverter::ReadFile(idBuffer &buffer, const char *path) const
{
	return buffer.file_get_contents(path) > 0;
}

idStr idNETLizardConverter::GetDir(const char *path)
{
    idStr dir;
	idStr(targetDir / path).GetFilePath(dir);
    if(!idFileSystem::FileExists(dir))
        idFileSystem::mkdir_p(dir);
    return dir;
}

idStr idNETLizardConverter::TargetFilePath(const char *path) const
{
    idStr str(targetDir);
    str /= path;
    return str;
}

idStr idNETLizardConverter::SourceFilePath(const char *path) const
{
    idStr str(sourceDir);
    if(path)
        str /= path;
    return str;
}

void idNETLizardConverter::Log(const char *str, ...) const
{
    va_list va;
    va_start(va, str);
    vprintf(str, va);
    va_end(va);
    printf("\n");
}

idVec3 idNETLizardConverter::Int3ToVec3(int a, int b, int c) const
{
    return { IntToFloat(a), IntToFloat(b), IntToFloat(c) };
}

float idNETLizardConverter::IntToFloat(int i) const
{
    if(game == NL_RACING_EVOLUTION_3D)
    {
        return idMath::IntBitsToFloat(i);
    }
    else
    {
        return float(i);
    }
}
