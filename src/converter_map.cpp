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
	idStr invisibleMaterial = idMaterial::CAULK_MATERIAL;
	float w = config->tex_width;

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
	if(v_normal.IsZero())
	{
		return false;
	}
	vertex[0].normal[0] = v_normal[0];
	vertex[0].normal[1] = v_normal[1];
	vertex[0].normal[2] = v_normal[2];
	vertex[1].normal[0] = v_normal[0];
	vertex[1].normal[1] = v_normal[1];
	vertex[1].normal[2] = v_normal[2];
	vertex[2].normal[0] = v_normal[0];
	vertex[2].normal[1] = v_normal[1];
	vertex[2].normal[2] = v_normal[2];

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
	vertex[0].xyz *= NETLIZARD_MAP_TO_IDTECH4;
	vertex[1].xyz *= NETLIZARD_MAP_TO_IDTECH4;
	vertex[2].xyz *= NETLIZARD_MAP_TO_IDTECH4;

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
	{
		return false;
	}
	side.textureMatrix[0][0] = 1.0 / w;
	side.textureMatrix[1][1] = 1.0 / w;

	side.FromDrawVerts(vertex);
	{
		return false;
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
	{
		return false;
	}
	brush.sides.push_back(side);

	// 3 sides
	idVec3 mid = (points[0] - points[1]);
	mid.Normalized();
	idVec3 nor = v_normal ^ mid;
	side.plane.FromPointAndNormal(points[0], nor);
	if(side.plane.Normal().IsZero())
	{
		return false;
	}
	brush.sides.push_back(side);

	mid = (points[1] - points[2]);
	mid.Normalized();
	nor = v_normal ^ mid;
	side.plane.FromPointAndNormal(points[1], nor);
	if(side.plane.Normal().IsZero())
	{
		return false;
	}
	brush.sides.push_back(side);

	mid = (points[2] - points[0]);
	mid.Normalized();
	nor = v_normal ^ mid;
	side.plane.FromPointAndNormal(points[2], nor);
	if(side.plane.Normal().IsZero())
	{
		return false;
	}
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

			NLint item_type = nlGetItemType(game, mesh->obj_index);
			if(item_type & (NL_3D_ITEM_TYPE_DOOR_HORIZONTAL | NL_3D_ITEM_TYPE_DOOR_VERTICAL))
				continue;

			idEntity entity;
			entity.classname = "func_static";
			entity.name = idStr::va("entity%d_%d", mesh->obj_index, i);
			entity.spawnArgs.Set("model", entity.name);
			entity.spawnArgs.SetVec3("origin", idVec3(mesh->position[0] * NETLIZARD_MAP_TO_IDTECH4, mesh->position[1] * NETLIZARD_MAP_TO_IDTECH4, mesh->position[2] * NETLIZARD_MAP_TO_IDTECH4));
			idMat4 m4;
			m4.Rotate(mesh->rotation[0], {1.0f, 0.0f, 0.0f});
			m4.Rotate(mesh->rotation[1], {0.0f, 0.0f, 1.0f});
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
		}
	}

	/* cout << bounds[0][0] << ", " << bounds[0][1] << "," << bounds[0][2]
		<< " | " << bounds[1][0] << ", " << bounds[1][1] << ", " << bounds[1][2] << endl; */
		
	if(model->bsp_data.data)
	{
		for(int i = 0; i < model->bsp_data.count; i++)
		{
			idEntity e;
			e.classname = "info_player_deathmatch";
			e.name = idStr::va("info_player_deathmatch_%d", i);
			const NETLizard_BSP_Tree_Node *node = model->bsp_data.data + i;
			idVec3 v(node->plane[0][0], node->plane[0][1], node->plane[0][2]);
			v *= NETLIZARD_MAP_TO_IDTECH4;
			idBounds bv({v, v});
			bv.AddPoint(idVec3(node->plane[1][0], node->plane[1][1], node->plane[1][2]) *= NETLIZARD_MAP_TO_IDTECH4);
			bv.AddPoint(idVec3(node->plane[2][0], node->plane[2][1], node->plane[2][2]) *= NETLIZARD_MAP_TO_IDTECH4);
			bv.AddPoint(idVec3(node->plane[3][0], node->plane[3][1], node->plane[3][2]) *= NETLIZARD_MAP_TO_IDTECH4);
			idVec3 center = bv.Center();
			e.spawnArgs.SetVec3("origin", {center[0], center[1], bv[0][2] + 1.0f});
			map.entitys.push_back(e);

			bv = map.areas[node->prev_scene];
			bv += map.areas[node->next_scene];
			idVec3 radius = bv.Size();
			e.spawnArgs.Clear();
			e.classname = "light";
			e.name = idStr::va("area_light_%d", i);
			e.spawnArgs.SetVec3("origin", center);
			e.spawnArgs.SetVec3("light_radius", radius);
			e.spawnArgs.SetBool("noshadows", true);
			e.spawnArgs.SetBool("nospecular", true);
			e.spawnArgs.SetFloat("falloff", 0);
			e.spawnArgs.SetVec3("_color", {0.78, 0.78, 0.84});
			map.entitys.push_back(e);

			idBrushDef3 brush;
			if(GenMapBrush(brush, node))
			{
				map.entitys[0].brushs.push_back(brush);
			}
		}
	}
		
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

bool idNETLizardConverter::GenMapBrush(idBrushDef3 &brush, const NETLizard_BSP_Tree_Node *node, bool invert) const
{
	idStr material = idMaterial::AREA_PORTAL_MATERIAL;
	idStr invisibleMaterial = idMaterial::NODRAW_MATERIAL;

	idVec3 points[4];
	for(int i = 0; i < 4; i++)
	{
		for(int m = 0; m < 3; m++)
			points[i][m] = (float)node->plane[i][m] * NETLIZARD_MAP_TO_IDTECH4;
	}
	if(invert)
	{
		std::swap(points[0], points[1]);
		std::swap(points[2], points[3]);
	}

	idVec3 v_normal = idVec3::TriangleCaleNormal(points[0], points[1], points[2]);

	idBrushDef3Side side;
	side.material = material;

	// raw front
	idVec3 points_[4];
	side.plane.FromPoints(points[0], points[1], points[2]);
	if(side.plane.Normal().IsZero())
	{
		return false;
	}
	float w = 256.0;
	side.textureMatrix[0][0] = 1.0 / w;
	side.textureMatrix[1][1] = 1.0 / w;
	brush.sides.push_back(side);

	side.material = invisibleMaterial;
	// back
	idVec3 normal_ = -v_normal;
	points_[0] = points[0] + normal_;
	points_[1] = points[1] + normal_;
	points_[2] = points[2] + normal_;
	points_[3] = points[3] + normal_;
	side.plane.FromPoints(points_[2], points_[1], points_[0]);
	if(side.plane.Normal().IsZero())
	{
		return false;
	}
	brush.sides.push_back(side);

	// 4 sides
	idVec3 mid = (points[0] - points[1]);
	mid.Normalized();
	idVec3 nor = v_normal ^ mid;
	side.plane.FromPointAndNormal(points[0], nor);
	if(side.plane.Normal().IsZero())
	{
		return false;
	}
	brush.sides.push_back(side);

	mid = (points[1] - points[3]);
	mid.Normalized();
	nor = v_normal ^ mid;
	side.plane.FromPointAndNormal(points[1], nor);
	if(side.plane.Normal().IsZero())
	{
		return false;
	}
	brush.sides.push_back(side);

	mid = (points[3] - points[2]);
	mid.Normalized();
	nor = v_normal ^ mid;
	side.plane.FromPointAndNormal(points[3], nor);
	if(side.plane.Normal().IsZero())
	{
		return false;
	}
	brush.sides.push_back(side);

	mid = (points[2] - points[0]);
	mid.Normalized();
	nor = v_normal ^ mid;
	side.plane.FromPointAndNormal(points[2], nor);
	if(side.plane.Normal().IsZero())
	{
		return false;
	}
	brush.sides.push_back(side);

	return true;
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
			if(game == NL_CONTR_TERRORISM_3D_EPISODE_3 && (i == 13 || i == 15))
				continue; // lvl13 15 not supprot in CT3D-Ep3
			idMap map;
			file = idStr::va(format, i);
			Log("Handle map %d - %s", i, file);
			if(ConvertMap(idStr(file), i) == 0)
			{
				res++;
				Log("Handle map success");
			}
		}
	}
    return res;
}