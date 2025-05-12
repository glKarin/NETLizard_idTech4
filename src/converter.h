#ifndef _KARIN_CONVERTER_H
#define _KARIN_CONVERTER_H

#include <string.h>

#include "libnetlizard/netlizard.h"

#include "str.h"
#include "list.h"

class idMaterial;
class idBounds;
class idBrushDef3;
typedef idList<idBrushDef3> idBrushDef3List;
class idMap;
class idMat4;
class idBuffer;
class idDef;
class idMapFile;
class idVec3;
class idSndShader;
class idPlane;

class idNETLizardConverter;

typedef void (* idConvertMapAreaPortalCallback_f)(const idNETLizardConverter *converter, idMapFile &map, int level, int index, int prev_scene, int next_scene, const idVec3 bounds[4]);
typedef void (* idConvertMapAreaCallback_f)(const idNETLizardConverter *converter, idMapFile &map, int level, int index, const idBounds &bounds);
typedef void (* idConvertMapCallback_f)(const idNETLizardConverter *converter, idMapFile &map, int index);

#define ID_INVOKE_CALLBACK(func, args) \
	if(func) { \
		func args; \
	}

class idNETLizardConverter
{
    public:
        idNETLizardConverter(NETLizard_Game game, const char *sourceDir, const char *targetDir);

        int ConvertTextures();
        int ConvertMaterials();
        int ConvertMaps();
        int ConvertMapDefs();
        int ConvertSounds();
        idStr GenDmapCommand();
		void Version(const char *version);

		void SetConvertMapAreaCallback(idConvertMapAreaCallback_f func);
		void SetConvertMapAreaPortalCallback(idConvertMapAreaPortalCallback_f func);
		void SetConvertMapCallback(idConvertMapCallback_f func);
		const char * Gamename() const;
		NETLizard_Game Game() const;

    private:
        int ConvertTextureToTGA(const char *file, int i);
		int ConvertSkyTextureToTGA(const char *name);
		bool WriteEnvTextureSide(const void *data, int width, int format, const char *path, const char *what);
		bool WriteEnvTexture(const NETLizard_Texture *tex, const char *name);
        int ConvertMap(const char *file, int i);
        int ConvertMaterial(idMaterial &mat, const char *file, int i);
		int ConvertSkyMaterial(idMaterial &mat, const char *file);
		int ConvertSkyEnvMaterial(idMaterial &mat, const char *file);
        int ConvertMapDef(idDef &def, const char *name, int i);
        int ConvertSound(idSndShader &snd, const char *name);
		idStr GenDmap(const char *map);
		idStr GenDmap_init(const char *map);
        bool ReadFile(idBuffer &buffer, const char *path) const;
        idStr GetDir(const char *path);
        idStr TargetFilePath(const char *path) const;
        idStr SourceFilePath(const char *path) const;
        bool GenMapBrush(idBrushDef3 &brush, idBounds &bounds, const NETLizard_3D_Primitive *p, const NLint *mesh_vertex, const idMat4 *mat = nullptr, bool isItem = false, float width = -1.0f, idPlane *ret = nullptr) const;
        bool GenMapBrush(idBrushDef3 &brush, const NETLizard_BSP_Tree_Node *node, bool invert = false) const;
        bool GenMapBrush(idBrushDef3 &brushes, const idVec3 points[4], const char *material, bool invert = false) const;
		bool GenMapBrush(idBrushDef3List &brush, const idBounds &bv, const char *material, bool invert, bool relative = false) const;
        bool LoadNETLizard3DMapModel(NETLizard_3D_Model &model, NETLizard_RE3D_Model &remodel, const char *file, int level);
		int RemoveLinkedPlane(idList<int> &res, const NETLizard_BSP_Tree_Node *data, int count) const;
        void Log(const char *str, ...) const;
        template <class T>
        T & ConvToIdTech4(T &v) const;
        template <class T>
        T ConvToIdTech4(const T &v) const;
        float IntToFloat(int i) const;
        idVec3 Int3ToVec3(int a, int b, int c) const;

        static int TEXTURE_FILE_TYPE;
        static float NETLIZARD_MAP_TO_IDTECH4;
        static float NETLIZARD_RE_MAP_TO_IDTECH4;

    private:
        NETLizard_Game game;
        idStr sourceDir;
        idStr targetDir;
        idStr gamename;
        const NETLizard_3D_Model_Config *config = nullptr;
		bool genPortalBrush = true;
		idStr version;
		int removeFraction = 2;

		idConvertMapAreaCallback_f convertMapAreaCallback = nullptr;
		idConvertMapAreaPortalCallback_f convertMapAreaPortalCallback = nullptr;
		idConvertMapCallback_f convertMapCallback = nullptr;
};



void remove_fraction(float &f, int prec);
void remove_fraction(idVec3 &v, int prec);
void remove_fraction(idBounds &v, int prec);

template <class T>
inline T & idNETLizardConverter::ConvToIdTech4(T &v) const
{
    v *= (game == NL_RACING_EVOLUTION_3D ? NETLIZARD_RE_MAP_TO_IDTECH4 : NETLIZARD_MAP_TO_IDTECH4);
	if(removeFraction >= 0)
    remove_fraction(v, removeFraction); // remove fraction and make integer as float
    return v;
}

template <class T>
inline T idNETLizardConverter::ConvToIdTech4(const T &v) const
{
    T r = v * (game == NL_RACING_EVOLUTION_3D ? NETLIZARD_RE_MAP_TO_IDTECH4 : NETLIZARD_MAP_TO_IDTECH4);
	if(removeFraction >= 0)
    remove_fraction(r, removeFraction); // remove fraction and make integer as float
    return r;
}

inline void idNETLizardConverter::SetConvertMapAreaCallback(idConvertMapAreaCallback_f func)
{
	convertMapAreaCallback = func;
}

inline void idNETLizardConverter::SetConvertMapAreaPortalCallback(idConvertMapAreaPortalCallback_f func)
{
	convertMapAreaPortalCallback = func;
}

inline void idNETLizardConverter::SetConvertMapCallback(idConvertMapCallback_f func)
{
	convertMapCallback = func;
}

inline const char * idNETLizardConverter::Gamename() const
{
	return gamename.c_str();
}

inline NETLizard_Game idNETLizardConverter::Game() const
{
	return game;
}

inline void idNETLizardConverter::Version(const char *version)
{
	this->version = version;
}
#endif
