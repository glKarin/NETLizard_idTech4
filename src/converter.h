#ifndef _KARIN_CONVERTER_H
#define _KARIN_CONVERTER_H

#include "libnetlizard/netlizard.h"

#include "str.h"
#include "buffer.h"

class idMaterial;
class idBounds;
class idBrushDef3;
class idMap;

class idNETLizardConverter
{
    public:
        idNETLizardConverter(NETLizard_Game game, const char *sourceDir, const char *targetDir);

        int ConvertTextures();
        int ConvertMaterials();
        int ConvertMaps();

    private:
        int ConvertTextureToTGA(const char *file, int i);
        int ConvertMap(const char *file, int i);
        int ConvertMaterial(idMaterial &mat, const char *file);
        bool ReadFile(idBuffer &buffer, const char *path) const;
        idStr GetDir(const char *path);
        idStr TargetFilePath(const char *path) const;
        idStr SourceFilePath(const char *path) const;
        bool GenMapBrush(idBrushDef3 &brush, idBounds &bounds, const NETLizard_3D_Primitive *p, const NLint *mesh_vertex, bool isItem = false) const;
        bool GenMapBrush(idBrushDef3 &brush, const NETLizard_BSP_Tree_Node *node, bool invert = false) const;
        bool LoadNETLizard3DMapModel(NETLizard_3D_Model &model, const char *file, int level);
        void Log(const char *str, ...) const;

        static int TEXTURE_FILE_TYPE;
        static float NETLIZARD_MAP_TO_IDTECH4;

    private:
        NETLizard_Game game;
        idStr sourceDir;
        idStr targetDir;
        idStr gamename;
        const NETLizard_3D_Model_Config *config;
};

#endif
