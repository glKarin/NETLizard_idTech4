#include <iostream>

#include "converter.h"

#include "id/mapfile.h"
#include "math/maths.h"

#define CHECK_ARG(x) N_CheckArg(argc, argv, #x, &x)
#define LIGHT_POS_Z_OFFSET 10.0f

using std::cout;
using std::endl;

static idVec3 ENV_LIGHT_COLOR = { 0.6, 0.6, 0.6 };
static bool noshadows = false;

static void N_ConvertMapAreaPortalFunc(const idNETLizardConverter *converter, idMapFile &map, int level, int i, int prev_scene, int next_scene, const idBounds &bv)
{
	idEntity e;
	idVec3 center = bv.Center();
	e.info_player_deathmatch({center[0], center[1], bv[0][2] + 1.0f});
	e.NameByClass("_%d", i);
	map << e;

#if 1
	idBounds bounds = map.AreaBounds(prev_scene);
	bounds += map.AreaBounds(next_scene);
	idVec3 radius = bounds.Size() * idVec3{0.5f, 0.5f, 2.0f};
	e.light({center[0], center[1], idMath::Max(bv[1][2] - LIGHT_POS_Z_OFFSET, center[2])}, radius, noshadows, true, ENV_LIGHT_COLOR);
	e.Name("area_portal_light_%d", i);
	map << e;
#endif
}

static void N_ConvertMapAreaFunc(const idNETLizardConverter *converter, idMapFile &map, int index, int i, const idBounds &bv)
{
	idEntity e;
	idVec3 center = bv.Center();
#if 0
	e.info_player_deathmatch({center[0], center[1], bv[0][2] + 1.0f});
	e.NameByClass("_%d", i);
	map << e;
#endif

	idVec3 radius = bv.Size();// * idVec3{0.5f, 0.5f, 1.0f};
	bool noshadow = true;
	if(converter->Game() == NL_SHADOW_OF_EGYPT_3D && (index == 8 || index == 9 || index == 12 || (index == 10 && i == 0)))
	{
		center[2] = bv[1][2];
		radius/*[2]*/ *= 2.0f;
		noshadow = false;
	}
	e.light(
			//{center[0], center[1], idMath::Max(bv[1][2] - LIGHT_POS_Z_OFFSET, center[2])}
			center
			, radius, noshadow, true, ENV_LIGHT_COLOR);
	e.Name("area_light_%d", i);
	map << e;
}

static void N_ConvertMapFunc(const idNETLizardConverter *converter, idMapFile &map, int level)
{
	int index = 0;

	auto endlevel_f = [&](const NETLizard_Game_Level_Start_End *area) {
		char *to = nullptr;
		NLint end;
		idVec3 pos;
		idVec3 size;

		if(area)
		{
			if(area->end < 0)
				end = area->start;
			else
				end = area->end;
			idBounds bv = map.AreaBounds(end);
			pos = bv.Center();
			pos[2] = bv[0][2];
			size = {50, 50, bv.Center()[2] - bv[0][2]};
			if(area->to >= 0)
				to = nlGet3DGameLevelFileName(converter->Game(), area->to);
			end = area->to;
		}
		else
		{
			pos = map.StartPos();
			size = {50, 50, 100};
			to = strdup(idStr::va("lvl%d", level + 1));
			end = level + 1;
		}

		if(to)
		{
			idEntity entity;
			entity.text(idStr::va("End level: %d. %s", end, to));
			entity.NameByClass("_endlevel_%d", index);
			entity.Origin(pos + idVec3{0.0, 0.0, size[2]});
			map << entity;

			entity.target_endLevel(idStr::va("%s/%s", converter->Gamename(), to));
			entity.NameByClass("_endlevel_%d", index);
			entity.Origin(pos);
			map << entity;

			idStr target = entity.Name();
			entity.func_waitforbutton(target);
			entity.NameByClass("_endlevel_%d", index);
			entity.Size({size[0] * 0.5f, size[1] * 0.5f, size[2]});
			entity.Origin(pos);
			map << entity;

			target = entity.Name();
			entity.trigger_multiple(target);
			entity.NameByClass("_endlevel_%d", index);
			entity.Size(size);
			entity.Origin(pos);
			map << entity;

			free(to);
		}
	};

	idEntity e;
	idVec3 startpos = map.StartPos();
	if(converter->Game() == NL_SHADOW_OF_EGYPT_3D && (level == 8 || level == 9 || level == 12 || level == 10))
	{
		startpos[2] += 20;
	}
	e.info_player_start(startpos, map.StartAngle());
	e.Name("info_player_start_1");
	map << e;

    char menuMusic[8];
    bool hasMenuMusic = nullptr != nlGet3DGameMenuMusicFileName(converter->Game(), menuMusic);

    char levelMusic[8];
    bool hasLevelMusic = nullptr != nlGet3DGameLevelMusicFileName(converter->Game(), level, levelMusic);
    if(hasMenuMusic || hasLevelMusic)
    {
        const char *mf = hasLevelMusic ? levelMusic : menuMusic;
		idStr music(converter->Gamename());
		music += "_";
		music += mf;
        e.speaker(music, true, true, false);
        e.Name("global_background_music_1");
        map << e;
    }

	if(converter->Game() == NL_SHADOW_OF_EGYPT_3D && (level == 2 || level == 4 || level == 6 || level == 7 || level == 8 || level == 9 || /*level == 10 ||*/ level == 11 || level == 12 || level == 13 || level == 14 || level == 15))
		return;
	NLint length;
	const NETLizard_Game_Level_Start_End *area = nlGet3DGameStartEndArea(converter->Game(), -1, -1, &length);
	if(area)
	{
		for(int i = 0; i < length; i++)
		{
			const NETLizard_Game_Level_Start_End *a = area + i;
			if(a->level == level)
			{
				endlevel_f(a);
				index++;
			}
		}
	}
	else
		endlevel_f(nullptr);
}

static bool N_CheckArg(int argc, char *argv[], const char *target, bool def)
{
	bool res = def;
	idStr yes = idStr::va("--%s", target);
	idStr no = idStr::va("--no-%s", target);

	for(int i = 1; i < argc; i++)
	{
		if(!idStr::Cmp(yes, argv[i]))
			res = true;
		else if(!idStr::Cmp(no, argv[i]))
			res = false;
	}

	return res;
}

static bool N_CheckArg(int argc, char *argv[], const char *target, bool *ret)
{
	bool res = false;
	idStr yes = idStr::va("--%s", target);
	idStr no = idStr::va("--no-%s", target);

	for(int i = 1; i < argc; i++)
	{
		if(!idStr::Cmp(yes, argv[i]))
		{
			res = true;
			if(ret)
				*ret = true;
		}
		else if(!idStr::Cmp(no, argv[i]))
		{
			res = true;
			if(ret)
				*ret = false;
		}
	}

	return res;
}

static const char * N_CheckArg(int argc, char *argv[], const char *target, const char *def)
{
	const char *res = def;
	idStr yes = idStr::va("--%s", target);

	for(int i = 1; i < argc; i++)
	{
		if(!idStr::Cmp(yes, argv[i]))
		{
			i++;
			if(i < argc)
				res = argv[i];
		}
	}

	return res;
}

static bool N_CheckArg(int argc, char *argv[], const char *target, const char **ret)
{
	bool res = false;
	idStr yes = idStr::va("--%s", target);

	for(int i = 1; i < argc; i++)
	{
		if(!idStr::Cmp(yes, argv[i]))
		{
			i++;
			if(i < argc)
			{
				res = true;
				if(ret)
					*ret = argv[i];
			}
		}
	}

	return res;
}

int main(int argc, char *argv[])
{
	using namespace std;

#if 0
	const char *game = argv[1];
	const char *source_path = argv[2];
	const char *target_path = argv[3];
#else
	const char *game = "4";
#ifdef _MSC_VER
	const char *source_path = R"(D:/project/harmattan/NETLizard_idTech4/resource/re3d)";
	const char *target_path = R"(F:/pak/test/doom3/base)";
#elif defined(__MINGW32__) || defined(__MINGW64__)
	const char *source_path = R"(D:/project/harmattan/NETLizard_idTech4/resource/egypt3d)";
	const char *target_path = R"(F:/pak/test/doom3/base)";
#else
	const char *source_path = "./re3d";
	const char *target_path = "/sdcard/diii4a/base";
#endif
#endif
	idNETLizardConverter converter(static_cast<NETLizard_Game>(atoi(game)), source_path, target_path);

	converter.SetConvertMapAreaCallback(N_ConvertMapAreaFunc);
	converter.SetConvertMapAreaPortalCallback(N_ConvertMapAreaPortalFunc);
	converter.SetConvertMapCallback(N_ConvertMapFunc);

	const char *version = MAP_VERSION_DOOM3;
	CHECK_ARG(version);
	CHECK_ARG(noshadows);

	cout << "noshadows: " << noshadows << endl;
	cout << "version: " << version << endl;

	converter.Version(version);
	converter.ConvertMaterials();
	converter.ConvertTextures();
	converter.ConvertMaps();
	converter.ConvertMapDefs();
    converter.ConvertSounds();
	idStr cmd = converter.GenDmapCommand();
	std::cout << cmd.c_str() << std::endl;
	std::cout << "done" << std::endl;
	return 0;
}
