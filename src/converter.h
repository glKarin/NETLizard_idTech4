#ifndef _KARIN_CONVERTER_H
#define _KARIN_CONVERTER_H

#include "libnetlizard/netlizard.h"

#include "str.h"

class idMaterial;
class idBounds;
class idBrushDef3;
class idMap;
class idMat4;
class idBuffer;
class idDef;

class idNETLizardConverter
{
    public:
        idNETLizardConverter(NETLizard_Game game, const char *sourceDir, const char *targetDir);

        int ConvertTextures();
        int ConvertMaterials();
        int ConvertMaps();
        int ConvertMapDefs();

    private:
        int ConvertTextureToTGA(const char *file, int i);
        int ConvertMap(const char *file, int i);
        int ConvertMaterial(idMaterial &mat, const char *file, int i);
        int ConvertMapDef(idDef &def, const char *name, int i);
        bool ReadFile(idBuffer &buffer, const char *path) const;
        idStr GetDir(const char *path);
        idStr TargetFilePath(const char *path) const;
        idStr SourceFilePath(const char *path) const;
        bool GenMapBrush(idBrushDef3 &brush, idBounds &bounds, const NETLizard_3D_Primitive *p, const NLint *mesh_vertex, const idMat4 *mat = nullptr, bool isItem = false) const;
        bool GenMapBrush(idBrushDef3 &brush, const NETLizard_BSP_Tree_Node *node, bool invert = false) const;
        bool LoadNETLizard3DMapModel(NETLizard_3D_Model &model, const char *file, int level);
        void Log(const char *str, ...) const;
        template <class T>
        static T & ConvToIdTech4(T &v);
        template <class T>
        static T ConvToIdTech4(const T &v);

        static int TEXTURE_FILE_TYPE;
        static float NETLIZARD_MAP_TO_IDTECH4;

    private:
        NETLizard_Game game;
        idStr sourceDir;
        idStr targetDir;
        idStr gamename;
        const NETLizard_3D_Model_Config *config = nullptr;
		bool genPortalBrush = false;
};



template <class T>
inline T & idNETLizardConverter::ConvToIdTech4(T &v)
{
    v *= NETLIZARD_MAP_TO_IDTECH4;
    return v;
}

template <class T>
inline T idNETLizardConverter::ConvToIdTech4(const T &v)
{
    return v * NETLIZARD_MAP_TO_IDTECH4;
}
#endif
