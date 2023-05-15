#include <iostream>

#include "converter.h"

int main(int argc, char *argv[])
{
	using namespace std;

#if 1
	const char *game = argv[1];
	const char *source_path = argv[2];
	const char *target_path = argv[3];
#else
	const char *game = "4";
	const char *source_path = "D:/project/harmattan/netlizard-game/egypt3d";
	const char *target_path = "D:/d2";
#endif
	idNETLizardConverter converter(static_cast<NETLizard_Game>(atoi(game)), source_path, target_path);
	converter.ConvertMaterials();
	converter.ConvertTextures();
	converter.ConvertMaps();
	converter.ConvertMapDefs();
	std::cout << "done" << std::endl;
	return 0;
}
