#include "converter.h"

#include <sstream>
#include <iostream>

idStr idNETLizardConverter::GenDmap_init(const char *map)
{
	const char format[] = "+dmap -noflood -noopt %s ";
	idStr fname("maps");
	fname /= gamename;
    fname /= map;
	return idStr::va(format, fname.c_str());
}

idStr idNETLizardConverter::GenDmap(const char *map)
{
	const char format[] = "dmap -noflood -noopt %s; ";
	idStr fname("maps");
	fname /= gamename;
    fname /= map;
	return idStr::va(format, fname.c_str());
}

idStr idNETLizardConverter::GenDmapCommand()
{
    int i;
    idStr file;
	const char *format = config->lvl_path_format;
	std::ostringstream os_init;
	std::ostringstream os;

	if(game == NL_SHADOW_OF_EGYPT_3D) // has a survise mode(dm1) map and main menu map(lvl0)
	{
		{
			os << GenDmap("dm1");
			os_init << GenDmap_init("dm1");
		}
		for(i = 0; i < config->level_count - 1; i++)
		{
			file = idStr::va(format, i);
            file.RemoveExtension();
			os << GenDmap(file);
			os_init << GenDmap_init(file);
		}
	}
	else
	{
		for(i = 1; i <= config->level_count; i++)
		{
			if(game == NL_CONTR_TERRORISM_3D_EPISODE_3 && (i == 13 || i == 15))
				continue; // lvl13 15 not supprot in CT3D-Ep3
			file = idStr::va(format, i);
            file.RemoveExtension();
			os << GenDmap(file);
			os_init << GenDmap_init(file);
		}
	}

	idStr res;
	res = os.str() + "\n\n" + os_init.str();
	return res;
}

