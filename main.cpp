#include <iostream>

#include "converter.h"

int main(int argc, char *argv[])
{
	using namespace std;

#if 0
	const char *game = argv[1];
	const char *source_path = argv[2];
	const char *target_path = argv[3];
#else
	const char *game = "5";
	const char *source_path = "D:/clone3d";
	const char *target_path = "D:/d";
#endif
	idNETLizardConverter converter(static_cast<NETLizard_Game>(atoi(game)), source_path, target_path);
	converter.ConvertMaterials();
	converter.ConvertTextures();
	converter.ConvertMaps();
	std::cout << "done" << std::endl;
	return 0;
}
