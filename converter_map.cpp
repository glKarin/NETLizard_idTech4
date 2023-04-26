#include "converter.h"

#include <vector>
#include <fstream>

#include "material.h"
#include "map.h"
#include "bounds.h"
#include "drawvert.h"
#include "matrix.h"
#include "filesystem.h"

float idNETLizardConverter::NETLIZARD_MAP_TO_IDTECH4 = 0.35;

bool idNETLizardConverter::GenMapBrush(idBrushDef3 &brush, idBounds &bounds, const NETLizard_3D_Primitive *p, const NLint *mesh_vertex, bool isItem) const
{
	int invert_texcoord_y = isItem ? config->item_invert_texcoord_y : config->invert_texcoord_y;
	int index_factory = isItem ? config->item_index_factory : config->index_factory;
	int texIndex = p->tex_index;
	idStr material = gamename / idStr::va(config->tex_path_format, texIndex);
	material.RemoveExtension();
	//idStr material = "textures/base_wall/lfwall13f3";
	idStr invisibleMaterial = "common/caulk";

	idDrawVert vertex[3];
	int i0 = p->index[0] / index_factory;
	int i1 = p->index[1] / index_factory;
	int i2 = p->index[2] / index_factory;

	vertex[0].xyz[0] = mesh_vertex[i0 * 3];
	vertex[0].xyz[1] = mesh_vertex[i0 * 3 + 1];
	vertex[0].xyz[2] = mesh_vertex[i0 * 3 + 2];

	vertex[1].xyz[0] = mesh_vertex[i1 * 3];
	vertex[1].xyz[1] = mesh_vertex[i1 * 3 + 1];
	vertex[1].xyz[2] = mesh_vertex[i1 * 3 + 2];

	vertex[2].xyz[0] = mesh_vertex[i2 * 3];
	vertex[2].xyz[1] = mesh_vertex[i2 * 3 + 1];
	vertex[2].xyz[2] = mesh_vertex[i2 * 3 + 2];

	idVec3 v_normal = idVec3::TriangleCaleNormal(vertex[0].xyz, vertex[1].xyz, vertex[2].xyz);
	vertex[0].normal[0] = v_normal[0];
	vertex[0].normal[1] = v_normal[1];
	vertex[0].normal[2] = v_normal[2];
	vertex[1].normal[0] = v_normal[0];
	vertex[1].normal[1] = v_normal[1];
	vertex[1].normal[2] = v_normal[2];
	vertex[2].normal[0] = v_normal[0];
	vertex[2].normal[1] = v_normal[1];
	vertex[2].normal[2] = v_normal[2];

	//GLfloat w = tex ? tex->width : 1.0;
	float w = 128;
	vertex[0].st[0] = (float)p->texcoord[0] / w;
	vertex[0].st[1] = (float)p->texcoord[1] / w;

	vertex[1].st[0] = (float)p->texcoord[2] / w;
	vertex[1].st[1] = (float)p->texcoord[3] / w;

	vertex[2].st[0] = (float)p->texcoord[4] / w;
	vertex[2].st[1] = (float)p->texcoord[5] / w;

	if(invert_texcoord_y)
	{
		vertex[0].st[1] = 1.0 - vertex[0].st[1];
		vertex[1].st[1] = 1.0 - vertex[1].st[1];
		vertex[2].st[1] = 1.0 - vertex[2].st[1];
	}

	idVec3 t0(vertex[0].st[0], vertex[0].st[1], 0.0);
	idBounds tb({t0, t0});
	tb.AddPoint({vertex[1].st[0], vertex[1].st[1], 0.0});
	tb.AddPoint({vertex[2].st[0], vertex[2].st[1], 0.0});

	idBrushDef3Side side;
	side.material = material;
	idVec3 points[3];
	points[0] = vertex[0].xyz;
	points[1] = vertex[1].xyz;
	points[2] = vertex[2].xyz;
	points[0] *= NETLIZARD_MAP_TO_IDTECH4;
	points[1] *= NETLIZARD_MAP_TO_IDTECH4;
	points[2] *= NETLIZARD_MAP_TO_IDTECH4;

	// raw front
	idVec3 points_[3];
	side.plane.FromPoints(points[0], points[1], points[2]);
	if(side.plane.Normal().IsZero())
		return false;
	if(invert_texcoord_y)
	{
		side.textureMatrix[0][0] = tb[1][1] / 128.0;
		side.textureMatrix[1][1] = tb[1][1] / 128.0;
	}
	else
	{
		side.textureMatrix[0][0] = 1.0 / 128.0;
		side.textureMatrix[1][1] = 1.0 / 128.0;
	}
	brush.sides.push_back(side);

	// back
	side.material = invisibleMaterial;
	idVec3 normal_ = -v_normal;
	points_[0] = points[0] + normal_;
	points_[1] = points[1] + normal_;
	points_[2] = points[2] + normal_;
	side.plane.FromPoints(points_[2], points_[1], points_[0]);
	if(side.plane.Normal().IsZero())
		return false;
	brush.sides.push_back(side);

	// 3 sides
	idVec3 mid = (points[0] - points[1]);
	mid.Normalize();
	idVec3 nor = v_normal ^ mid;
	side.plane.FromPointAndNormal(points[0], nor);
	if(side.plane.Normal().IsZero())
		return false;
	brush.sides.push_back(side);

	mid = (points[1] - points[2]);
	mid.Normalize();
	nor = v_normal ^ mid;
	side.plane.FromPointAndNormal(points[1], nor);
	if(side.plane.Normal().IsZero())
		return false;
	brush.sides.push_back(side);

	mid = (points[2] - points[0]);
	mid.Normalize();
	nor = v_normal ^ mid;
	side.plane.FromPointAndNormal(points[2], nor);
	if(side.plane.Normal().IsZero())
		return false;
	brush.sides.push_back(side);

	bounds[0] = points[0];
	bounds[1] = points[0];
	bounds.AddPoint(points[1]);
	bounds.AddPoint(points[2]);
	return true;
}

bool idNETLizardConverter::LoadNETLizard3DMapModel(NETLizard_3D_Model &m, const char *file, int i)
{
    idStr name = SourceFilePath(file);
	idStr resource_path = SourceFilePath(nullptr);

	switch(game)
	{
		case NL_CONTR_TERRORISM_3D:
			if(!nlReadCT3DModelFile(name, i, resource_path, &m))
				return false;
			break;
		case NL_CONTR_TERRORISM_3D_EPISODE_2:
			if(!nlReadCT3DEp2ModelFile(name, i, resource_path, &m))
				return false;
			break;
		case NL_ARMY_RANGER_3D:
			if(!nlReadSpecnaz3DModelFile(name, i, resource_path, &m))
				return false;
			break;
		case NL_SHADOW_OF_EGYPT_3D:
			if(!nlReadEgypt3DModelFile(name, resource_path, &m))
				return false;
			break;
		case NL_CLONE_3D:
			if(!nlReadClone3DModelFile(name, resource_path, &m))
				return false;
			break;
		case NL_CONTR_TERRORISM_3D_EPISODE_3:
			if(!nlReadCT3DEp3ModelFile(name, i, resource_path, &m))
				return false;
			break;
		default:
			return false;
	}
	return true;
}

int idNETLizardConverter::ConvertMap(const char *file, int i)
{
	const char *sky_file;
	NETLizard_3D_Model m;
	NETLizard_3D_Model *model = &m;
	idMap map;

	if(!LoadNETLizard3DMapModel(m, file, i))
		return -1;

	sky_file = config->sky_file;
	idBounds bounds;
	bool boundsInit = false;
	idEntity worldspawn;
	worldspawn.classname = "worldspawn";

	map.startPos = idVec3(model->start_position[0], model->start_position[1], model->start_position[2]) * NETLIZARD_MAP_TO_IDTECH4;
	map.startAngle = model->start_rotation[0];
	if(model->data.data)
	{
		for(int i = 0; i < model->data.count; i++)
		{
			const NETLizard_3D_Mesh *mesh = model->data.data + i;
			const NLint *mesh_vertex = mesh->vertex.data;

			if(mesh->primitive.data)
			{
				for(int n = 0; n < mesh->primitive.count; n++)
				{
					idBrushDef3 brush;
					idBounds b;
					if(GenMapBrush(brush, b, mesh->primitive.data + n, mesh_vertex))
					{
						if(!boundsInit)
						{
							bounds = b;
							boundsInit = true;
						}
						else
							bounds += b;
						worldspawn.brushs.push_back(brush);
					}
				}
			}

#if 1
			idVec3 min(mesh->box.min[0], mesh->box.min[1], mesh->box.min[2]);
			idVec3 max(mesh->box.max[0], mesh->box.max[1], mesh->box.max[2]);
			min *= NETLIZARD_MAP_TO_IDTECH4;
			max *= NETLIZARD_MAP_TO_IDTECH4;
			idBounds b = {min, max};
			map.areas.insert({i, b});
#endif
		}
		map.entitys.push_back(worldspawn);
	}

	map.bounds = bounds;

	if(model->item_data.data)
	{
		for(int i = 0; i < model->item_data.count; i++)
		{
			const NETLizard_3D_Item_Mesh *mesh = model->item_data.data + i;
			const NLint *mesh_vertex = mesh->item_mesh.vertex.data;
			idEntity entity;
			entity.classname = "func_static";
			entity.name = idStr::va("entity%d_%d", mesh->obj_index, i);
			entity.spawnArgs.Set("model", entity.name);
			entity.spawnArgs.SetVec3("origin", idVec3(mesh->position[0] * NETLIZARD_MAP_TO_IDTECH4, mesh->position[1] * NETLIZARD_MAP_TO_IDTECH4, mesh->position[2] * NETLIZARD_MAP_TO_IDTECH4));
			idMat4 m4;
			m4.Rotate(mesh->rotation[0], {1.0f, 0.0f, 0.0f});
			m4.Rotate(mesh->rotation[1], {0.0f, 1.0f, 0.0f});
			//m->item_type = nlGetItemType(game, mesh->obj_index);
			entity.spawnArgs.SetMat3("rotation", m4);
			if(mesh->item_mesh.vertex.count && mesh->item_mesh.primitive.count)
			{
				for(int o = 0; o < mesh->item_mesh.primitive.count; o++)
				{
					idBrushDef3 brush;
					idBounds b;
					if(GenMapBrush(brush, b, mesh->item_mesh.primitive.data + o, mesh_vertex, true))
					{
						entity.brushs.push_back(brush);
					}
				}
				map.entitys.push_back(entity);
			}

#if 0
			m->box.min[0] = (GLfloat)mesh->item_mesh.box.min[0];
			m->box.min[1] = (GLfloat)mesh->item_mesh.box.min[1];
			m->box.min[2] = (GLfloat)mesh->item_mesh.box.min[2];
			m->box.max[0] = (GLfloat)mesh->item_mesh.box.max[0];
			m->box.max[1] = (GLfloat)mesh->item_mesh.box.max[1];
			m->box.max[2] = (GLfloat)mesh->item_mesh.box.max[2];
#endif

#if 0
			if(/*(m->item_type & NL_3D_ITEM_TYPE_DOOR_VERTICAL) || */(m->item_type & NL_3D_ITEM_TYPE_DOOR_HORIZONTAL))
			{
			}
#endif
		}
	}

	/* cout << bounds[0][0] << ", " << bounds[0][1] << "," << bounds[0][2]
		<< " | " << bounds[1][0] << ", " << bounds[1][1] << ", " << bounds[1][2] << endl; */
		
	map.FillExtras();

	idStr fname("maps");
	fname /= gamename;
    fname /= file;
    GetDir(fname);
	fname.ReplaceExtension("map");
	int res = idFileSystem::Output(TargetFilePath(fname), map);
	
	nlDeleteNETLizard3DModel(&m);
	return res > 0 ? 0 : -2;
}

int idNETLizardConverter::ConvertMaps()
{
    int i;
    const char *format;
    const char *file;
    int res;

    res = 0;
	if(game == NL_SHADOW_OF_EGYPT_3D) // has a survise mode(dm1) map and main menu map(lvl0)
	{
		{
			idMap map;
			file = idStr::va(format, i);
			if(ConvertMap("dm1.png", -1) == 0)
				res++;
		}
		format = "lvl%d.png";
		for(i = 0; i <= config->level_count; i++)
		{
			idMap map;
			file = idStr::va(format, i);
			if(ConvertMap(idStr(file), i) == 0)
				res++;
		}
	}
	else
	{
		format = "lvl%d.png";
		for(i = 1; i <= config->level_count; i++)
		{
			idMap map;
			file = idStr::va(format, i);
			if(ConvertMap(idStr(file), i) == 0)
				res++;
		}
	}
    return res;
}
