#include "converter.h"

#include <vector>
#include <fstream>
#include <iostream>

#include "material.h"
#include "mapfile.h"
#include "bounds.h"
#include "drawvert.h"
#include "matrix.h"
#include "filesystem.h"
#include "matrix.h"
#include "precompiled.h"

using std::cout;
using std::endl;

#define THIN_ELEVATOR_WIDTH 1.0
#define ELEVATOR_MOVE_SPEED 100
#define DEFAULT_CLIP_SIZE 20

float idNETLizardConverter::NETLIZARD_MAP_TO_IDTECH4 = 0.35;

bool idNETLizardConverter::GenMapBrush(idBrushDef3 &brush, idBounds &bounds, const NETLizard_3D_Primitive *p, const NLint *mesh_vertex, const idMat4 *mat, bool isItem, float width) const
{
	int invert_texcoord_y = isItem ? config->item_invert_texcoord_y : config->invert_texcoord_y;
	int index_factory = isItem ? config->item_index_factory : config->index_factory;
	int texIndex = p->tex_index;
	idStr material = gamename / idStr::va(config->tex_path_format, texIndex);
	material.RemoveExtension();
	idStr invisibleMaterial = width > 0.0f ? material : idStr(idMaterial::CAULK_MATERIAL);
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

	ConvToIdTech4(vertex[0].xyz);
	ConvToIdTech4(vertex[1].xyz);
	ConvToIdTech4(vertex[2].xyz);

	idVec3 v_normal = idVec3::TriangleCaleNormal(vertex[0].xyz, vertex[1].xyz, vertex[2].xyz);
	if(v_normal.IsZero())
	{
		Log("Point(%s, %s, %s) all in one line", vertex[0].xyz.ToString().c_str(), vertex[1].xyz.ToString().c_str(), vertex[2].xyz.ToString().c_str());
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

	idBrushDef3Side side;
	side.Material() = material;
	idVec3 points[3] = {
		vertex[0].xyz,
		vertex[1].xyz,
		vertex[2].xyz,
	};

	// raw front
	if(!side.PlaneFromPoints(points[0], points[1], points[2]))
	{
		return false;
	}

	idPlane tmpPlane = side.Plane();
	if(mat)
	{
		vertex[0].xyz = *mat * vertex[0].xyz;
		vertex[1].xyz = *mat * vertex[1].xyz;
		vertex[2].xyz = *mat * vertex[2].xyz;
	}
	if(!side.FromDrawVerts(vertex))
	{
		Log("Vertex((xyz(%s), uv(%s)), (xyz(%s), uv(%s)), (xyz(%s), uv(%s)), (normal(%s))) can't generate texture matrix", vertex[0].xyz.ToString().c_str(), vertex[0].st.ToString().c_str(), vertex[1].xyz.ToString().c_str(), vertex[1].st.ToString().c_str(), vertex[2].xyz.ToString().c_str(), vertex[2].st.ToString().c_str(), v_normal.ToString().c_str());
		side.TextureMatrix(0)[0] = 1.0 / w;
		side.TextureMatrix(1)[1] = 1.0 / w;
	}
	if(mat)
	{
		side.Plane() = tmpPlane;
	}

	brush << side;

	side.TextureMatrix(0).Zero();
	side.TextureMatrix(1).Zero();
	side.TextureMatrix(0)[0] = 1.0 / w;
	side.TextureMatrix(1)[1] = 1.0 / w;

	// back
	side.Material() = invisibleMaterial;
	idVec3 normal_ = -v_normal;
	idVec3 points_[3];
	if(width > 0.0f)
	{
		points_[0] = points[0] + normal_ * width;
		points_[1] = points[1] + normal_ * width;
		points_[2] = points[2] + normal_ * width;
	}
	else
	{
	points_[0] = points[0] + normal_;
	points_[1] = points[1] + normal_;
	points_[2] = points[2] + normal_;
	}
	if(!side.PlaneFromPoints(points_[2], points_[1], points_[0]))
	{
		return false;
	}
	brush << side;

	// 3 sides
	idVec3 mid = points[1] >> points[0];
	idVec3 nor = v_normal ^ mid;
	if(!side.PlaneFromPointAndNormal(points[0], nor))
	{
		return false;
	}
	brush << side;

	mid = points[2] >> points[1];
	nor = v_normal ^ mid;
	if(!side.PlaneFromPointAndNormal(points[1], nor))
	{
		return false;
	}
	brush << side;

	mid = points[0] >> points[2];
	nor = v_normal ^ mid;
	if(!side.PlaneFromPointAndNormal(points[2], nor))
	{
		return false;
	}
	brush << side;

	bounds[0] = points[0];
	bounds[1] = points[0];
	bounds += points[1];
	bounds += points[2];
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

int idNETLizardConverter::ConvertMap(const char *file, int index)
{
	const char *sky_file;
	NETLizard_3D_Model m;
	NETLizard_3D_Model *model = &m;
	idMapFile map(version);

	if(!LoadNETLizard3DMapModel(m, file, index))
		return -1;

	sky_file = config->sky_file;
	map << idEntity();
	idEntity worldspawn;
	worldspawn.worldspawn();

	map.StartPos() = ConvToIdTech4(idVec3(model->start_position[0], model->start_position[1], model->start_position[2] - 150));
	if(game == NL_CONTR_TERRORISM_3D)
		map.StartPos()[2] += DEFAULT_CLIP_SIZE;
	map.StartAngle() = model->start_rotation[0];
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
						map += b;
						worldspawn << brush;
					}
				}
			}

			if(game == NL_SHADOW_OF_EGYPT_3D && (index == 8 || index == 9 || index == 12 || index == 10))
			{
				if(model->item_data.data)
				{
					for(int m = mesh->item_index_range[0]; m <= mesh->item_index_range[1]; m++)
					{
						const NETLizard_3D_Item_Mesh *itemmesh = model->item_data.data + m;
						const NLint *itemmesh_vertex = itemmesh->item_mesh.vertex.data;

						NLint item_type = nlGetItemType(game, itemmesh->obj_index);
						if(!(item_type & (NL_3D_ITEM_TYPE_SKYLINE)))
							continue;

						if(itemmesh->item_mesh.vertex.count && itemmesh->item_mesh.primitive.count)
						{
							idMat4 m4;
							m4.Identity();
							m4.Rotate(itemmesh->rotation[0], {1.0f, 0.0f, 0.0f});
							//m4.Rotate(itemmesh->rotation[1], {0.0f, 0.0f, 1.0f});
							m4.Translate(idVec3(itemmesh->position[0], itemmesh->position[1], itemmesh->position[2]));

							NLint *nvd = (NLint *)malloc(sizeof(NLint) * itemmesh->item_mesh.vertex.count);
							for(int k = 0; k < itemmesh->item_mesh.vertex.count / 3; k++)
							{
								idVec3 ov(itemmesh_vertex[k * 3], itemmesh_vertex[k * 3 + 1], itemmesh_vertex[k * 3 + 2]);
								ov = m4 * ov;
								nvd[k * 3] = ov[0];
								nvd[k * 3 + 1] = ov[1];
								nvd[k * 3 + 2] = ov[2];
							}

							for(int o = 0; o < itemmesh->item_mesh.primitive.count; o++)
							{
								idBrushDef3 brush;
								idBounds b;
								if(GenMapBrush(brush, b, itemmesh->item_mesh.primitive.data + o, nvd, nullptr, true, -1))
								{
									worldspawn << brush;
								}
							}
						}
					}
				}
			}

#if 1
			idVec3 min(mesh->box.min[0], mesh->box.min[1], mesh->box.min[2]);
			idVec3 max(mesh->box.max[0], mesh->box.max[1], mesh->box.max[2]);
			ConvToIdTech4(min);
			ConvToIdTech4(max);
			idBounds b = {min, max};
			map.AddAreaBounds(i, b);
#endif

			ID_INVOKE_CALLBACK(convertMapAreaCallback, (this, map, index, i, b));
		}
		map[0] = worldspawn;
	}

	if(model->item_data.data)
	{
		for(int i = 0; i < model->item_data.count; i++)
		{
			const NETLizard_3D_Item_Mesh *mesh = model->item_data.data + i;
			const NLint *mesh_vertex = mesh->item_mesh.vertex.data;

			NLint item_type = nlGetItemType(game, mesh->obj_index);
			if(item_type & (NL_3D_ITEM_TYPE_SKYBOX))
				continue;
			if(game == NL_CLONE_3D && (item_type & (NL_3D_ITEM_TYPE_WEAPON | NL_3D_ITEM_TYPE_PARTICLE)))
				continue;
			if(game == NL_SHADOW_OF_EGYPT_3D && (index == 8 || index == 9 || index == 12 || index == 10) && (item_type & NL_3D_ITEM_TYPE_SKYLINE))
				continue;

			idEntity entity;
			idBounds itemBv = {
				ConvToIdTech4(idVec3(mesh->item_mesh.box.min[0], mesh->item_mesh.box.min[1], mesh->item_mesh.box.min[2])), 
				ConvToIdTech4(idVec3(mesh->item_mesh.box.max[0], mesh->item_mesh.box.max[1], mesh->item_mesh.box.max[2]))
			};
			if(item_type & NL_3D_ITEM_TYPE_WEAPON)
			{
				entity.func_bobbing(10, 0.4f);
			}
			else if(item_type & (NL_3D_ITEM_TYPE_DOOR_VERTICAL | NL_3D_ITEM_TYPE_DOOR_HORIZONTAL))
			{
				const NETLizard_Level_Door *door = nlGet3DGameDoor(game, index, i, nullptr);
				if(door)
				{
					int doori = 0; 
					int movedir;
					int buddies[1] = {-1};
					switch(door->orientation)
					{
						case 2:
							if(door->mask == 1)
							{
								doori = 1;
							}
							else if(door->mask == 2)
							{
								doori = 0;
							}
							else
							{
								for(int n = 0; n < 2; n++)
								{
									int itemid = door->item[n].item;
									if(itemid == i)
										doori = n;
									else
										buddies[0] = itemid;
								}
							}
							if(doori == 1)
								movedir = (int)id::idDoor_movedir::RIGHT;
							else
								movedir = (int)id::idDoor_movedir::LEFT;
							break;

						case 3:
							if(door->mask == 1)
							{
								doori = 1;
							}
							else if(door->mask == 2)
							{
								doori = 0;
							}
							else
							{
								for(int n = 0; n < 2; n++)
								{
									int itemid = door->item[n].item;
									if(itemid == i)
										doori = n;
									else
										buddies[0] = itemid;
								}
							}
							if(doori == 1)
								movedir = (int)id::idDoor_movedir::FORWARD;
							else
								movedir = (int)id::idDoor_movedir::BACKWARD;
							break;

						case 1:
						default:
							if(door->mask == 1)
							{
								doori = 0;
							}
							else if(door->mask == 2)
							{
								doori = 1;
							}
							else
							{
								for(int n = 0; n < 2; n++)
								{
									int itemid = door->item[n].item;
									if(itemid == i)
										doori = n;
									else
										buddies[0] = itemid;
								}
							}
							if(doori == 1)
								movedir = (int)id::idDoor_movedir::DOWN;
							else
								movedir = (int)id::idDoor_movedir::UP;
							break;
					}
					entity.func_door(movedir);
					if(buddies[0] >= 0)
					{
						entity.NameByClass("_%d", buddies[0]);
						entity("buddy", entity.Name());
						entity.Name("");
					}
				}
			}
			else if(item_type & NL_3D_ITEM_TYPE_FAN_Z_AXIS)
			{
				entity.func_rotating();
			}
			else if(item_type & NL_3D_ITEM_TYPE_FAN_X_AXIS)
			{
				entity.func_rotating(true);
			}
			else if(item_type & NL_3D_ITEM_TYPE_FAN_Y_AXIS)
			{
				//entity.func_rotating(true);
				entity.func_rotating(false, true);
			}
			else if(item_type & NL_3D_ITEM_TYPE_ELEVATOR)
			{
				const NETLizard_Level_Elevator *elevator = nlGet3DGameElevator(game, index, i, nullptr);
				if(elevator && elevator->elevator_item == i)
				{
					idVec3 floor1 = ConvToIdTech4(idVec3(mesh->position[0], mesh->position[1], elevator->min));
					floor1[2] += 1.0;
					idVec3 floor2 = ConvToIdTech4(idVec3(mesh->position[0], mesh->position[1], elevator->max));
					floor2[2] += 1.0;
					idVec3 floors[] = {
						floor1, floor2,
					};
					entity.func_elevator(ELEVATOR_MOVE_SPEED, 0, true, 2, floors);
					//entity("trigger", true);
					//entity("triggerFloor", 2);
					if((item_type & NL_3D_ITEM_TYPE_SCENE) == 0)
					{
						idBounds bv(itemBv);
						bv[0][2] -= 1.0;
						bv[1][2] += 1.0;
						entity.MinMax(bv);
					}
				}
			}
			else if(item_type & NL_3D_ITEM_TYPE_SWITCH)
			{
				const NETLizard_Level_Elevator *elevator = nlGet3DGameElevator(game, index, i, nullptr);
				if(elevator && elevator->switch_item == i)
				{
					entity.Reset();
					entity.func_waitforbutton(idStr::va("func_elevator_%d", elevator->elevator_item));
					entity.NameByClass("_%d", i);
					entity.Origin(ConvToIdTech4(idVec3(mesh->position[0], mesh->position[1], mesh->position[2])));
					map << entity;
					idStr target = entity.Name();
					entity.Reset();
					entity.trigger_multiple(target);
				}
			}
			entity.NameByClass("_%d", i);
			entity.Model();
			entity.Origin(ConvToIdTech4(idVec3(mesh->position[0], mesh->position[1], mesh->position[2])));
			idMat4 m4;
			m4.Rotate(mesh->rotation[0], {1.0f, 0.0f, 0.0f});
			m4.Rotate(mesh->rotation[1], {0.0f, 0.0f, 1.0f});
			idMat3 m3 = m4;
			m3.Transposed();
			entity("rotation", m3);
			
			if(mesh->item_mesh.vertex.count && mesh->item_mesh.primitive.count)
			{
				m4.Identity();
				m4.Rotate(mesh->rotation[0], {1.0f, 0.0f, 0.0f});
				//m4.Rotate(mesh->rotation[1], {0.0f, 0.0f, 1.0f});
				m4.Translate(ConvToIdTech4(idVec3(mesh->position[0], mesh->position[1], mesh->position[2])));

				for(int o = 0; o < mesh->item_mesh.primitive.count; o++)
				{
					idBrushDef3 brush;
					idBounds b;
					float width = -1.0;
					if((item_type & (NL_3D_ITEM_TYPE_ELEVATOR | NL_3D_ITEM_TYPE_THIN)) == (NL_3D_ITEM_TYPE_ELEVATOR | NL_3D_ITEM_TYPE_THIN))
						width = THIN_ELEVATOR_WIDTH;
					if(GenMapBrush(brush, b, mesh->item_mesh.primitive.data + o, mesh_vertex, &m4, true, width))
					{
						entity << brush;
					}
				}
				map << entity;
			}

			if(item_type & NL_3D_ITEM_TYPE_PORTAL)
			{
				const NETLizard_Level_Teleport *teleport = nlGet3DGameTeleport(game, index, i, nullptr);
				if(teleport)
				{
					idBrushDef3 brush;
					if(idBrushDef3::FromBounds(brush, itemBv, idMaterial::CAULK_MATERIAL))
					{
						idStr target;
						idVec3 teleportPos(teleport->position[0] >> 16, teleport->position[1] >> 16, teleport->position[2] >> 16);
						ConvToIdTech4(teleportPos);
						idVec3 triggerPos(mesh->position[0], mesh->position[1], mesh->position[2]);
						ConvToIdTech4(triggerPos);

						if(teleport->end_level)
						{
							entity.target_endLevel(idStr::va("%s/lvl%d", gamename.c_str(), index + 1));
							entity.NameByClass("_%d", i);
							entity.Origin(teleportPos);
							map << entity;

							target = entity.Name();
							entity.func_waitforbutton(target);
							entity.NameByClass("_%d", i);
							entity.Origin(triggerPos);
							map << entity;
							target = entity.Name();
						}
						else
						{
							entity.info_player_teleport(teleportPos, teleport->rotation[0]);
							entity.NameByClass("_%d", i);
							entity.Origin(teleportPos);
							map << entity;
							target = entity.Name();
						}

						entity.trigger_multiple(target);
						entity.NameByClass("_%d", i);
						entity.Model();
						entity.Origin(triggerPos);
						entity << brush;
						map << entity;
					}
				}
			}
		}
	}

	/* cout << bounds[0][0] << ", " << bounds[0][1] << "," << bounds[0][2]
		<< " | " << bounds[1][0] << ", " << bounds[1][1] << ", " << bounds[1][2] << endl; */
		
	if(model->bsp_data.data)
	{
		for(int i = 0; i < model->bsp_data.count; i++)
		{
			const NETLizard_BSP_Tree_Node *node = model->bsp_data.data + i;
			idVec3 v(node->plane[0][0], node->plane[0][1], node->plane[0][2]);
			ConvToIdTech4(v);
			idBounds bv({v, v});
			bv += ConvToIdTech4(idVec3(node->plane[1][0], node->plane[1][1], node->plane[1][2]));
			bv += ConvToIdTech4(idVec3(node->plane[2][0], node->plane[2][1], node->plane[2][2]));
			bv += ConvToIdTech4(idVec3(node->plane[3][0], node->plane[3][1], node->plane[3][2]));

			ID_INVOKE_CALLBACK(convertMapAreaPortalCallback, (this, map, index, i, node->prev_scene, node->next_scene, bv));

			if(genPortalBrush)
			{
				idBrushDef3 brush;
				if(GenMapBrush(brush, node))
				{
					map[0] << brush;
				}
			}
		}
	}
		
	ID_INVOKE_CALLBACK(convertMapCallback, (this, map, index));

	idStr fname("maps");
	fname /= gamename;
    fname /= file;
    GetDir(fname);
	fname.ReplaceExtension("map");
	int res = idFileSystem::Output(TargetFilePath(fname), map);
	
	nlDeleteNETLizard3DModel(&m);
	return res > 0 ? 0 : -2;
}

bool idNETLizardConverter::GenMapBrush(idBrushDef3 &brush, const idVec3 _points[4], const char *material, bool invert) const
{
	idStr invisibleMaterial = idMaterial::NODRAW_MATERIAL;

	idVec3 points[4];
	for(int i = 0; i < 4; i++)
	{
		points[i] = ConvToIdTech4(_points[i]);
	}
	if(invert)
	{
		std::swap(points[0], points[1]);
		std::swap(points[2], points[3]);
	}

	idVec3 v_normal = idVec3::TriangleCaleNormal(points[0], points[1], points[2]);

	idBrushDef3Side side;
	side.Material() = material;

	// raw front
	idVec3 points_[4];
	if(!side.PlaneFromPoints(points[0], points[1], points[2]))
	{
		return false;
	}
	float w = 256.0;
	side.TextureMatrix(0)[0] = 1.0 / w;
	side.TextureMatrix(1)[1] = 1.0 / w;
	brush << side;

	side.Material() = invisibleMaterial;
	// back
	idVec3 normal_ = -v_normal;
	points_[0] = points[0] + normal_;
	points_[1] = points[1] + normal_;
	points_[2] = points[2] + normal_;
	points_[3] = points[3] + normal_;
	if(!side.PlaneFromPoints(points_[2], points_[1], points_[0]))
	{
		return false;
	}
	brush << side;

	// 4 sides
	idVec3 mid = points[1] >> points[0];
	idVec3 nor = v_normal ^ mid;
	if(!side.PlaneFromPointAndNormal(points[0], nor))
	{
		return false;
	}
	brush << side;

	mid = points[3] >> points[1];
	nor = v_normal ^ mid;
	if(!side.PlaneFromPointAndNormal(points[1], nor))
	{
		return false;
	}
	brush << side;

	mid = points[2] >> points[3];
	nor = v_normal ^ mid;
	if(!side.PlaneFromPointAndNormal(points[3], nor))
	{
		return false;
	}
	brush << side;

	mid = points[0] >> points[2];
	nor = v_normal ^ mid;
	if(!side.PlaneFromPointAndNormal(points[2], nor))
	{
		return false;
	}
	brush << side;

	return true;
}

bool idNETLizardConverter::GenMapBrush(idBrushDef3 &brush, const NETLizard_BSP_Tree_Node *node, bool invert) const
{
	idVec3 points[4];
	for(int i = 0; i < 4; i++)
	{
		for(int m = 0; m < 3; m++)
			points[i][m] = (float)node->plane[i][m];
	}
	return GenMapBrush(brush, points, idMaterial::AREA_PORTAL_MATERIAL, invert);
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
			if(ConvertMap("dm1.png", -1) == 0)
				res++;
		}
		format = "lvl%d.png";
		for(i = 0; i < config->level_count; i++)
		{
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
