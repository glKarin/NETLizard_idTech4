#include "converter.h"

#include <fstream>
#include "id/snd.h"
#include "io/filesystem.h"

int idNETLizardConverter::ConvertSound(idSndShader &snd, const char *name)
{
	idStr fname(gamename);
    fname += "_";
	fname += name;
    fname.RemoveExtension();

    snd.Name() = fname;

    idStr srcfname(gamename);
    srcfname /= name;
    srcfname.ReplaceExtension("ogg");

    snd.SetSoundFile(idStr("sound") / srcfname);

    srcfname = SourceFilePath(name);
    srcfname.ReplaceExtension("ogg");

    idStr dir("sound");
    dir /= gamename;
    idStr tofile = dir / name;
    dir = TargetFilePath(dir);
    idFileSystem::mkdir_p(dir);

    tofile.ReplaceExtension("ogg");
    tofile = TargetFilePath(tofile);
    idFileSystem::cp(srcfname, tofile);

    snd.SetSoundClass(idSndShader::SOUNDCLASS_MUSIC);
    snd.Looping();
    snd.Global();
    snd.Plain();
    snd.NoOcclusion();

	return 0;
}

int idNETLizardConverter::ConvertSounds()
{
    int i;
    idStr file;
	idSndShaderList snds;
	
	idStr path("sound");
	path /= gamename;
	path += ".sndshd";
	GetDir(path);
	idStr sndFile = TargetFilePath(path);

    idStrList files;

    char menuMusic[8];
    bool hasMenuMusic = nullptr != nlGet3DGameMenuMusicFileName(game, menuMusic);
    if(hasMenuMusic)
    {
        files.Append(menuMusic);
    }

    char levelMusic[8];
    bool hasLevelMusic;
	if(game == NL_SHADOW_OF_EGYPT_3D)
	{
		for(i = 0; i <= config->level_count; i++)
		{
            hasLevelMusic = nullptr != nlGet3DGameLevelMusicFileName(game, i, levelMusic);
            if(hasLevelMusic && !files.Contains(levelMusic))
                files.Append(levelMusic);
		}
	}
	else
	{
		for(i = 1; i <= config->level_count; i++)
		{
            hasLevelMusic = nullptr != nlGet3DGameLevelMusicFileName(game, i, levelMusic);
            if(hasLevelMusic && !files.Contains(levelMusic))
                files.Append(levelMusic);
		}
	}

	std::ofstream os(sndFile);
    for (const auto &item: files)
    {
        idSndShader sndshd;
        if(ConvertSound(sndshd, item) == 0)
        {
            snds.push_back(sndshd);
        }
    }

	for(const auto &map : snds)
		os << map << "\n";
	os.flush();
	os.close();
	return (int)snds.size();
}

