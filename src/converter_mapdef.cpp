#include "converter.h"

#include <fstream>
#include "id/def.h"

int idNETLizardConverter::ConvertMapDef(idDef &def, const char *name, int index)
{
	idStr fname(gamename);
	fname /= name;
	def.mapDef(fname, idStr::va("%d. %s", index, nlGet3DGameLevelName(game, index)));
	def("devname", fname);
	def("singleplayer", true);
	def("Deathmatch", true);
	def("Team DM", true);
	def("Last Man", true);
	def("Tourney", true);
	return 0;
}

int idNETLizardConverter::ConvertMapDefs()
{
    int i;
    const char *format;
    const char *file;
	idDefList defs;
	
	idStr path("def");
	path /= gamename;
	path += "_map.def";
	GetDir(path);
	idStr defFile = TargetFilePath(path);
	if(game == NL_SHADOW_OF_EGYPT_3D) // has a survise mode(dm1) map and main menu map(lvl0)
	{
		{
			idDef map;
			file = idStr::va(format, i);
			if(ConvertMapDef(map, "dm1", config->level_count - 1) == 0)
				defs.push_back(map);
		}
		format = "lvl%d";
		for(i = 0; i < config->level_count - 1; i++)
		{
			idDef map;
			file = idStr::va(format, i);
			if(ConvertMapDef(map, idStr(file), i) == 0)
			{
				defs.push_back(map);
			}
		}
	}
	else
	{
		format = "lvl%d";
		for(i = 1; i <= config->level_count; i++)
		{
			if(game == NL_CONTR_TERRORISM_3D_EPISODE_3 && (i == 13 || i == 15))
				continue; // lvl13 15 not supprot in CT3D-Ep3
			idDef map;
			file = idStr::va(format, i);
			if(ConvertMapDef(map, idStr(file), i) == 0)
			{
				defs.push_back(map);
			}
		}
	}

	std::ofstream os(defFile);
	for(const auto &map : defs)
		os << map << "\n";
	os.flush();
	os.close();
	return defs.size();
}

