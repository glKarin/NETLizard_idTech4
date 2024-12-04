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

float idNETLizardConverter::NETLIZARD_MAP_TO_IDTECH4 = 0.35f;
float idNETLizardConverter::NETLIZARD_RE_MAP_TO_IDTECH4 = 35.0f;

bool idNETLizardConverter::GenMapBrush(idBrushDef3 &brush, idBounds &bounds, const NETLizard_3D_Primitive *p, const NLint *mesh_vertex, const idMat4 *mat, bool isItem, float width) const
{
	int invert_texcoord_y = isItem ? config->item_invert_texcoord_y : config->invert_texcoord_y;
	NLuint index_factory = isItem ? config->item_index_factory : config->index_factory;
	int texIndex = p->tex_index;
    idStr material = gamename;
    float w;
    if(game == NL_RACING_EVOLUTION_3D)
    {
        const char **allTexes = nlGetRE3DMapTexes(nullptr);
        material /= idStr::va(config->tex_path_format, allTexes[texIndex]);
        w = texIndex == 1 ? 64.0f : (float)config->tex_width;
    }
    else
    {
        material /= idStr::va(config->tex_path_format, texIndex);
        w = (float)config->tex_width;
    }
    material.RemoveExtension();
	idStr invisibleMaterial = width >= 0.0f ? material : idStr(idMaterial::CAULK_MATERIAL);

	idDrawVert vertex[3];
    NLuint i0 = p->index[0] / index_factory;
    NLuint i1 = p->index[1] / index_factory;
    NLuint i2 = p->index[2] / index_factory;

	vertex[0].xyz[0] = IntToFloat(mesh_vertex[i0 * 3]);
	vertex[0].xyz[1] = IntToFloat(mesh_vertex[i0 * 3 + 1]);
	vertex[0].xyz[2] = IntToFloat(mesh_vertex[i0 * 3 + 2]);

	vertex[1].xyz[0] = IntToFloat(mesh_vertex[i1 * 3]);
	vertex[1].xyz[1] = IntToFloat(mesh_vertex[i1 * 3 + 1]);
	vertex[1].xyz[2] = IntToFloat(mesh_vertex[i1 * 3 + 2]);

	vertex[2].xyz[0] = IntToFloat(mesh_vertex[i2 * 3]);
	vertex[2].xyz[1] = IntToFloat(mesh_vertex[i2 * 3 + 1]);
	vertex[2].xyz[2] = IntToFloat(mesh_vertex[i2 * 3 + 2]);

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

    if(game == NL_RACING_EVOLUTION_3D)
    {
        vertex[0].st[0] = IntToFloat((int)p->texcoord[0]);
        vertex[0].st[1] = IntToFloat((int)p->texcoord[1]);

        vertex[1].st[0] = IntToFloat((int)p->texcoord[2]);
        vertex[1].st[1] = IntToFloat((int)p->texcoord[3]);

        vertex[2].st[0] = IntToFloat((int)p->texcoord[4]);
        vertex[2].st[1] = IntToFloat((int)p->texcoord[5]);
    }
    else
    {
        vertex[0].st[0] = (float)p->texcoord[0] / w;
        vertex[0].st[1] = (float)p->texcoord[1] / w;

        vertex[1].st[0] = (float)p->texcoord[2] / w;
        vertex[1].st[1] = (float)p->texcoord[3] / w;

        vertex[2].st[0] = (float)p->texcoord[4] / w;
        vertex[2].st[1] = (float)p->texcoord[5] / w;
    }

	if(invert_texcoord_y)
	{
		vertex[0].st[1] = 1.0f - vertex[0].st[1];
		vertex[1].st[1] = 1.0f - vertex[1].st[1];
		vertex[2].st[1] = 1.0f - vertex[2].st[1];
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
		side.TextureMatrix(0)[0] = 1.0f / w;
		side.TextureMatrix(1)[1] = 1.0f / w;
	}
	if(mat)
	{
		side.Plane() = tmpPlane;
	}

	brush << side;

	side.TextureMatrix(0).Zero();
	side.TextureMatrix(1).Zero();
	side.TextureMatrix(0)[0] = 1.0f / w;
	side.TextureMatrix(1)[1] = 1.0f / w;

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

bool idNETLizardConverter::LoadNETLizard3DMapModel(NETLizard_3D_Model &m, NETLizard_RE3D_Model &remodel, const char *file, int i)
{
    idStr name = SourceFilePath(file);
	idStr resource_path = SourceFilePath(nullptr);
    memset(&m, 0, sizeof(m));
    memset(&remodel, 0, sizeof(remodel));

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
        case NL_RACING_EVOLUTION_3D:
            if(!nlReadRE3DMeshFile(name, &remodel))
                return false;
            nlRE3DToNETLizardModel(&remodel, &m);
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
    NETLizard_RE3D_Model rem;
	NETLizard_3D_Model *model = &m;
	idMapFile map(version);

	if(!LoadNETLizard3DMapModel(m, rem, file, index))
		return -1;

	sky_file = config->sky_file;
	map << idEntity();
	idEntity worldspawn;
	worldspawn.worldspawn();

	map.StartPos() = ConvToIdTech4(Int3ToVec3(model->start_position[0], model->start_position[1], model->start_position[2] - 150));
	if(game == NL_CONTR_TERRORISM_3D)
		map.StartPos()[2] += DEFAULT_CLIP_SIZE;
	int startAngle = model->start_rotation[0];
	if(game != NL_CLONE_3D)
		startAngle = (startAngle + 180) % 360;
	map.StartAngle() = (float)startAngle;

	idBounds allBv;
	if(model->data.data)
	{
		for(int i = 0; i < model->data.count; i++)
		{
			const NETLizard_3D_Mesh *mesh = model->data.data + i;
			idVec3 min = Int3ToVec3(mesh->box.min[0], mesh->box.min[1], mesh->box.min[2]);
			idVec3 max = Int3ToVec3(mesh->box.max[0], mesh->box.max[1], mesh->box.max[2]);
			idBounds originBv = {min, max};
			allBv += originBv;
		}
	}

	if(model->data.data)
	{
		for(int i = 0; i < model->data.count; i++)
		{
			const NETLizard_3D_Mesh *mesh = model->data.data + i;
			const NLint *mesh_vertex = mesh->vertex.data;
			idVec3 min = Int3ToVec3(mesh->box.min[0], mesh->box.min[1], mesh->box.min[2]);
			idVec3 max = Int3ToVec3(mesh->box.max[0], mesh->box.max[1], mesh->box.max[2]);
			idBounds originBv = {min, max};

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

			// skyline: skyline road as scene, not item
			if(game == NL_SHADOW_OF_EGYPT_3D && (index == 8 || index == 9 || index == 12 || index == 10))
			{
				if(model->item_data.data)
				{
					for(NLuint n = mesh->item_index_range[0]; n <= mesh->item_index_range[1]; n++)
					{
						const NETLizard_3D_Item_Mesh *itemmesh = model->item_data.data + n;
						const NLint *itemmesh_vertex = itemmesh->item_mesh.vertex.data;

						NLuint item_type = nlGetItemType(game, (NLint)itemmesh->obj_index);
						if(!(item_type & (NL_3D_ITEM_TYPE_SKYLINE)))
							continue;

						if(itemmesh->item_mesh.vertex.count && itemmesh->item_mesh.primitive.count)
						{
							idMat4 m4;
							m4.Identity();
							m4.Rotate((float)itemmesh->rotation[0], {1.0f, 0.0f, 0.0f});
							//m4.Rotate(itemmesh->rotation[1], {0.0f, 0.0f, 1.0f});
							m4.Translate(idVec3((float)itemmesh->position[0], (float)itemmesh->position[1], (float)itemmesh->position[2]));

							NLint *nvd = (NLint *)malloc(sizeof(NLint) * itemmesh->item_mesh.vertex.count);
							for(int k = 0; k < itemmesh->item_mesh.vertex.count / 3; k++)
							{
								idVec3 ov((float)itemmesh_vertex[k * 3], (float)itemmesh_vertex[k * 3 + 1], (float)itemmesh_vertex[k * 3 + 2]);
								ov = m4 * ov;
								nvd[k * 3] = (int)ov[0];
								nvd[k * 3 + 1] = (int)ov[1];
								nvd[k * 3 + 2] = (int)ov[2];
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

			// skybox
#if 0 // has own skybox on every scene
			if(game == NL_SHADOW_OF_EGYPT_3D && (index == 0 || index == 8 || index == 9 || index == 12 || index == 10))
			{
				if(model->item_data.data)
				{
					for(int m = mesh->item_index_range[0]; m <= mesh->item_index_range[1]; m++)
					{
						const NETLizard_3D_Item_Mesh *itemmesh = model->item_data.data + m;
						const NLint *itemmesh_vertex = itemmesh->item_mesh.vertex.data;

						NLint item_type = nlGetItemType(game, itemmesh->obj_index);
						if(!(item_type & (NL_3D_ITEM_TYPE_SKYBOX)))
							continue;

						if(itemmesh->item_mesh.vertex.count && itemmesh->item_mesh.primitive.count)
						{
							idMat4 m4;
							m4.Identity();
							m4.Rotate(itemmesh->rotation[0], {1.0f, 0.0f, 0.0f});
							//m4.Rotate(itemmesh->rotation[1], {0.0f, 0.0f, 1.0f});
							m4.Translate(idVec3(itemmesh->position[0], itemmesh->position[1], itemmesh->position[2]));
							idBounds itemBv = {
								idVec3(itemmesh->item_mesh.box.min[0], itemmesh->item_mesh.box.min[1], itemmesh->item_mesh.box.min[2]), 
								idVec3(itemmesh->item_mesh.box.max[0], itemmesh->item_mesh.box.max[1], itemmesh->item_mesh.box.max[2])
							};
							idVec3 per = originBv.Size() / itemBv.Size();
							m4.Scale(per.GetMaxAxisLength());

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
#endif

			ConvToIdTech4(min);
			ConvToIdTech4(max);
			idBounds b = {min, max};
			map.AddAreaBounds(i, b);

			ID_INVOKE_CALLBACK(convertMapAreaCallback, (this, map, index, i, b));
		}
	}

	// skybox
	// only one skybox on all scene
	if(game == NL_SHADOW_OF_EGYPT_3D && (index == 0 || index == 8 || index == 9 || index == 12 || index == 10))
	{
		if(model->data.data)
		{
			for(int i = 0; i < model->data.count; i++)
			{
				const NETLizard_3D_Mesh *mesh = model->data.data + i;
				const NLint *mesh_vertex = mesh->vertex.data;
				idVec3 min((float)mesh->box.min[0], (float)mesh->box.min[1], (float)mesh->box.min[2]);
				idVec3 max((float)mesh->box.max[0], (float)mesh->box.max[1], (float)mesh->box.max[2]);
				bool handle = false;

				if(model->item_data.data)
				{
					for(NLuint n = mesh->item_index_range[0]; n <= mesh->item_index_range[1]; n++)
					{
						const NETLizard_3D_Item_Mesh *itemmesh = model->item_data.data + n;
						const NLint *itemmesh_vertex = itemmesh->item_mesh.vertex.data;

						NLuint item_type = nlGetItemType(game, itemmesh->obj_index);
						if(!(item_type & (NL_3D_ITEM_TYPE_SKYBOX)))
							continue;

						if(itemmesh->item_mesh.vertex.count && itemmesh->item_mesh.primitive.count)
						{
							idMat4 m4;
							m4.Identity();
							m4.Rotate((float)itemmesh->rotation[0], {1.0f, 0.0f, 0.0f});
							//m4.Rotate(itemmesh->rotation[1], {0.0f, 0.0f, 1.0f});
							//m4.Translate(idVec3(itemmesh->position[0], itemmesh->position[1], itemmesh->position[2]));
							m4.Translate(allBv.Center());
							idBounds itemBv = {
								idVec3((float)itemmesh->item_mesh.box.min[0], (float)itemmesh->item_mesh.box.min[1], (float)itemmesh->item_mesh.box.min[2]),
								idVec3((float)itemmesh->item_mesh.box.max[0], (float)itemmesh->item_mesh.box.max[1], (float)itemmesh->item_mesh.box.max[2])
							};
							idVec3 per = allBv.Size() / itemBv.Size() * 2.0; // x2
							m4.Scale(per.GetMaxAxisLength() + 10);

							NLint *nvd = (NLint *)malloc(sizeof(NLint) * itemmesh->item_mesh.vertex.count);
							for(int k = 0; k < itemmesh->item_mesh.vertex.count / 3; k++)
							{
								idVec3 ov((float)itemmesh_vertex[k * 3], (float)itemmesh_vertex[k * 3 + 1], (float)itemmesh_vertex[k * 3 + 2]);
								ov = m4 * ov;
								nvd[k * 3] = (int)ov[0];
								nvd[k * 3 + 1] = (int)ov[1];
								nvd[k * 3 + 2] = (int)ov[2];
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
							handle = true;
							break;
						}
						if(handle)
							break;
					}
				}
				if(handle)
					break;
			}
		}
	}

	map[0] = worldspawn;
	// skybox 2D
	if(sky_file && sky_file[0])
	{
		idVec3 v(allBv.Size().GetMaxAxisLength()/* / 2.0f*/);
		idBounds ob{-v, v};
		ob.Translate(allBv.Center());
		ob.Expand(10);
		idStr material = gamename / idStr::va("env_%s", sky_file);
		//material = "skies/sky1";
		material.RemoveExtension();
		idBrushDef3List brushes;
		if(GenMapBrush(brushes, ob, material.c_str(), true))
		{
#if 0
			printf("xxx\n");
			idEntity entity;
			entity.func_static();
			entity.Name("_%d", 99933544);
			entity.Model();
			entity.Origin(map.StartPos() + idVec3(0,0, 80));
			for(const auto &brush : brushes)
				entity << brush;
			//map << entity;
#endif
			for(const auto &brush : brushes)
				map[0] << brush;
		}
	}

	if(model->item_data.data)
	{
		for(int i = 0; i < model->item_data.count; i++)
		{
			const NETLizard_3D_Item_Mesh *mesh = model->item_data.data + i;
			const NLint *mesh_vertex = mesh->item_mesh.vertex.data;

			NLuint item_type = nlGetItemType(game, (NLint)mesh->obj_index);
			if(item_type & (NL_3D_ITEM_TYPE_SKYBOX))
				continue;
			if(game == NL_CLONE_3D && (item_type & (NL_3D_ITEM_TYPE_WEAPON | NL_3D_ITEM_TYPE_PARTICLE)))
				continue;
			if(game == NL_SHADOW_OF_EGYPT_3D && (index == 8 || index == 9 || index == 12 || index == 10) && (item_type & NL_3D_ITEM_TYPE_SKYLINE))
				continue;

			idVec3 itemPos = ConvToIdTech4(Int3ToVec3(mesh->position[0], mesh->position[1], mesh->position[2]));
			idEntity entity;
			idBounds itemBv = {
				ConvToIdTech4(Int3ToVec3(mesh->item_mesh.box.min[0], mesh->item_mesh.box.min[1], mesh->item_mesh.box.min[2])),
				ConvToIdTech4(Int3ToVec3(mesh->item_mesh.box.max[0], mesh->item_mesh.box.max[1], mesh->item_mesh.box.max[2]))
			};
			idVec3 itemSize = itemBv.Size();
			int sizeScalar = 0;
			bool nonSolid = false;
			idMat4 m4;
			m4.Rotate(IntToFloat(mesh->rotation[0]), {1.0f, 0.0f, 0.0f});
			m4.Rotate(IntToFloat(mesh->rotation[1]), {0.0f, 0.0f, 1.0f});
			idMat3 m3 = m4;
			m3.Transposed();

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
					idVec3 floor1 = ConvToIdTech4(Int3ToVec3(mesh->position[0], mesh->position[1], elevator->min));
					floor1[2] += 1.0;
					idVec3 floor2 = ConvToIdTech4(Int3ToVec3(mesh->position[0], mesh->position[1], elevator->max));
					floor2[2] += 1.0;
					idVec3 floors[] = {
						floor1, floor2,
					};
					entity.func_elevator(ELEVATOR_MOVE_SPEED, 0, game != NL_CLONE_3D, 2, floors);
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
					if(elevator->mask == 3)
					{
						idVec3 itemPosFloor2 = itemPos;
						itemPosFloor2[2] += ConvToIdTech4(float(elevator->max - elevator->min));
						entity.text(idStr::va("Use elevator(%d): down to 1", elevator->elevator_item));
						entity.NameByClass("_floor2_%d", i);
						entity.Origin(itemPosFloor2);
						map << entity;

						entity.Reset();
						entity.Classname("target_setkeyval");
						entity.NameByClass("_floor2_%d", i);
						entity.Origin(itemPosFloor2);
						entity("target", idStr::va("func_elevator_%d", elevator->elevator_item));
						entity("keyval", "trigger;1");
						entity("keyval2", "triggerFloor;1");
						map << entity;

						entity.func_waitforbutton(idStr::va("target_setkeyval_floor2_%d", i));
						entity.NameByClass("_floor2_%d", i);
						entity.Origin(itemPosFloor2);
						map << entity;

						idStr target = entity.Name();
						entity.trigger_multiple(target);
						entity.NameByClass("_floor2_%d", i);
						entity.Origin(itemPosFloor2);
						entity.Rotation(m3);
						entity.Size(itemSize * 2.0);
						map << entity;
						entity.Reset();
					}

					int gotoFloor;
					if(elevator->mask == 2)
						gotoFloor = 1;
					else // if(elevator->mask == 1)
						gotoFloor = 2;
					entity.text(idStr::va("Use elevator(%d): %s to %d", elevator->elevator_item, gotoFloor == 1 ? "down" : "up", gotoFloor));
					entity.NameByClass("_elevator_%d", i);
					entity.Origin(itemPos);
					map << entity;

					entity.Reset();
					entity.Classname("target_setkeyval");
					entity.NameByClass("_%d", i);
					entity.Origin(itemPos);
					entity("target", idStr::va("func_elevator_%d", elevator->elevator_item));
					entity("keyval", "trigger;1");
					entity("keyval2", idStr::va("triggerFloor;%d", gotoFloor));
					map << entity;

					entity.func_waitforbutton(idStr::va("target_setkeyval_%d", i));
					entity.NameByClass("_%d", i);
					entity.Origin(itemPos);
					map << entity;

					idStr target = entity.Name();
					entity.trigger_multiple(target);
					sizeScalar = 2;
				}
			}
			else if(item_type & NL_3D_ITEM_TYPE_BOX)
				nonSolid = true;

			entity.NameByClass("_%d", i);
			if(nonSolid)
				entity.NoClipModel(true);
			entity.Model();
			entity.Origin(itemPos);
			entity.Rotation(m3);
			if(sizeScalar > 0)
				entity.Size(itemSize * float(sizeScalar));
			
			if(mesh->item_mesh.vertex.count && mesh->item_mesh.primitive.count)
			{
				m4.Identity();
				m4.Rotate(IntToFloat(mesh->rotation[0]), {1.0f, 0.0f, 0.0f});
				//m4.Rotate(mesh->rotation[1], {0.0f, 0.0f, 1.0f});
				m4.Translate(ConvToIdTech4(Int3ToVec3(mesh->position[0], mesh->position[1], mesh->position[2])));

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
						idVec3 teleportPos = Int3ToVec3(teleport->position[0] >> 16, teleport->position[1] >> 16, teleport->position[2] >> 16);
						ConvToIdTech4(teleportPos);
						idVec3 triggerPos = Int3ToVec3(mesh->position[0], mesh->position[1], mesh->position[2]);
						ConvToIdTech4(triggerPos);

						if(teleport->end_level)
						{
                            idStr lvlName = idStr::va(config->lvl_path_format, index + 1);
                            lvlName.RemoveExtension();
							entity.text(idStr::va("End level: %s", lvlName.c_str()));
							entity.NameByClass("_endlevel%d_%d", teleport->end_level, i);
							entity.Origin(teleportPos);
							map << entity;

							entity.target_endLevel(idStr::va("%s/%s", gamename.c_str(), lvlName.c_str()));
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
							entity.info_player_teleport(teleportPos, IntToFloat(teleport->rotation[0]));
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
			idVec3 v = Int3ToVec3(node->plane[0][0], node->plane[0][1], node->plane[0][2]);
			ConvToIdTech4(v);
			idBounds bv({v, v});
			bv += ConvToIdTech4(Int3ToVec3(node->plane[1][0], node->plane[1][1], node->plane[1][2]));
			bv += ConvToIdTech4(Int3ToVec3(node->plane[2][0], node->plane[2][1], node->plane[2][2]));
			bv += ConvToIdTech4(Int3ToVec3(node->plane[3][0], node->plane[3][1], node->plane[3][2]));

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
    nlDeleteNETLizardRE3DModel(&rem);
	return res > 0 ? 0 : -2;
}


bool idNETLizardConverter::GenMapBrush(idBrushDef3List &brushes, const idBounds &bv, const char *material, bool invert) const
{
	//idBounds bv{{5000.0f},{5000.0f}};
	float minx = bv[0][0];
	float miny = bv[0][1];
	float minz = bv[0][2];
	float maxx = bv[1][0];
	float maxy = bv[1][1];
	float maxz = bv[1][2];
#if 0
	idBounds bb{{-30.0f},{30.0f}};
	float v[24];
	// 前左下 0
	v[0] = minx;
	v[1] = miny;
	v[2] = minz;
	// 前左上 1
	v[3] = minx;
	v[4] = miny;
	v[5] = maxz;
	// 前右下 2
	v[6] = maxx;
	v[7] = miny;
	v[8] = minz;
	// 前右上 3
	v[9] = maxx;
	v[10] = miny;
	v[11] = maxz;
	// 后左下 4
	v[12] = minx;
	v[13] = maxy;
	v[14] = minz;
	// 后左上 5
	v[15] = minx;
	v[16] = maxy;
	v[17] = maxz;
	// 后右下 6
	v[18] = maxx;
	v[19] = maxy;
	v[20] = minz;
	// 后右上 7
	v[21] = maxx;
	v[22] = maxy;
	v[23] = maxz;

	/*
	 1  3  |  5  7
	 0  2  |  4  6
	 */
	const int CubePlane4Index[] =
	{
		// 正
		0, 2, 1, 1, 2, 3,
		// 背
		4, 5, 6, 6, 5, 7,
		// 左
		4, 0, 5, 5, 0, 1,
		// 右
		2, 6, 3, 3, 6, 7,
		// 上
		1, 3, 5, 5, 3, 7,
		// 下
		0, 4, 2, 2, 4, 6
	};
	const float CubePlane4Texcoord[] =
	{
		// 正
		0.0, 1.0,  1.0, 1.0,  0.0, 0.0,  0,0, 1.0,  1.0, 1.0,  0.0, 0.0,
		// 背
		1.0, 1.0,  1.0, 0.0,  0.0, 0.0,  0.0, 0.0,  1.0, 0.0,  1.0, 1.0,
		// 左
		0.0, 0.0,  1.0, 0.0,  0.0, 1.0,  0.0, 1.0,  1.0, 0.0,  1.0, 1.0,
		// 右
		0.0, 0.0,  1.0, 1.0,  0.0, 1.0,  0.0, 1.0,  1.0, 1.0,  1.0, 0.0,
		// 上
		0.0, 0.0,  1.0, 0.0,  0.0, 1.0,  0.0, 1.0,  1.0, 0.0,  1.0, 1.0,
		// 下
		0.0, 1.0,  0.0, 0.0,  1.0, 1.0,  1.0, 1.0,  0.0, 0.0,  1.0, 0.0
	};

	const float CubePlane4Normal[] =
	{
		// 正
		0.0, -1.0, 0.0,
		// 背
		0.0, 1.0, 0.0,
		// 左
		-1.0, 0.0, 0.0,
		// 右
		1.0, 0.0, 0.0,
		// 上
		0.0, 0.0, 1.0,
		// 下
		0.0, 0.0, -1.0
	};
	const int count = sizeof(CubePlane4Index) / sizeof(CubePlane4Index[0]) / 3;
	idDrawVert vertexs[count * 3];
	for(int i = 0; i < count; i++)
	{
		for(int m = 0; m < 3; m++)
		{
			idDrawVert *dv = vertexs + i * 3 + m;
			int p = CubePlane4Index[i * 3 + m];
			dv->xyz[0] = v[p * 3];
			dv->xyz[1] = v[p * 3 + 1];
			dv->xyz[2] = v[p * 3 + 2];
			dv->normal[0] = CubePlane4Normal[i / 2 * 3];
			dv->normal[1] = CubePlane4Normal[i / 2 * 3 + 1];
			dv->normal[2] = CubePlane4Normal[i / 2 * 3 + 2];
			dv->st[0] = CubePlane4Texcoord[p * 2];
			dv->st[1] = CubePlane4Texcoord[p * 2 + 1];
			printf("%d %d: %f %f %f | %d\n", i, m, dv->normal[0],dv->normal[1], dv->normal[2], p   );

			ConvToIdTech4(dv->xyz);
		}
		idDrawVert *dv = vertexs + i * 3;
		idBrushDef3Side side;
		side.PlaneFromPoints(dv[0].xyz, dv[1].xyz, dv[2].xyz);
		printf("nnn %s\n", side.Plane().Normal().ToString().c_str());
	}
#else	

	float v[] = {
		-1.000000, -1.000000, 1.000000,
		-1.000000, -1.000000, -1.000000,
		-1.000000, 1.000000, 1.000000,
		-1.000000, 1.000000, -1.000000,
		1.000000, -1.000000, 1.000000,
		1.000000, -1.000000, -1.000000,
		1.000000, 1.000000, 1.000000,
		1.000000, 1.000000, -1.000000,
	};
	float vt[] = {
		0.875000,0.500000,
		0.625000,0.750000,
		0.625000,0.500000,
		0.375000,1.000000,
		0.375000,0.750000,
		0.625000,0.000000,
		0.375000,0.250000,
		0.375000,0.000000,
		0.375000,0.500000,
		0.125000,0.750000,
		0.125000,0.500000,
		0.625000,0.250000,
		0.875000,0.750000,
		0.625000,1.000000,
	};
	float vn[] = {
		0.0000, 0.0000, 1.0000,
		0.0000, 1.0000, 0.0000,
		1.0000, 0.0000, 0.0000,
		0.0000, 0.0000, -1.0000,
		-1.0000, 0.0000, 0.0000,
		0.0000, -1.0000, 0.0000,
	};
	int f[] = {
		5,1,1, 3,2,1, 1,3,1,
		3,2,2, 8,4,2, 4,5,2,
		7,6,3, 6,7,3, 8,8,3,
		2,9,4, 8,10,4, 6,11,4,
		1,3,5, 4,5,5, 2,9,5,
		5,12,6, 2,9,6, 6,7,6,
		5,1,1, 7,13,1, 3,2,1,
		3,2,2, 7,14,2, 8,4,2,
		7,6,3, 5,12,3, 6,7,3,
		2,9,4, 4,5,4, 8,10,4,
		1,3,5, 3,2,5, 4,5,5,
		5,12,6, 1,3,6, 2,9,6
	};
	const int count = sizeof(f) / sizeof(f[0]) / 3 / 3;
	idDrawVert vertexs[count * 3];
	for(int i = 0; i < count; i++)
	{
		for(int m = 0; m < 3; m++)
		{
			idDrawVert *dv = vertexs + i * 3 + m;
			const int *ptr = f + i * 9 + (invert ? (2 - m) : m) * 3;
			const int p[3] = {
				ptr[0] - 1,
				ptr[1] - 1,
				ptr[2] - 1,
			};
			dv->xyz[0] = v[p[0] * 3] * fabs(v[p[0] * 3] > 0 ? maxx : minx);
			dv->xyz[1] = v[p[0] * 3 + 1] * fabs(v[p[0] * 3 + 1] > 0 ? maxy : miny);
			dv->xyz[2] = v[p[0] * 3 + 2] * fabs(v[p[0] * 3 + 2] > 0 ? maxz : minz);
			dv->normal[0] = vn[p[2] * 3];
			dv->normal[1] = vn[p[2] * 3 + 1];
			dv->normal[2] = vn[p[2] * 3 + 2];
			dv->st[0] = vt[p[1] * 2];
			dv->st[1] = vt[p[1] * 2 + 1];
			if(invert)
				dv->normal = -dv->normal;
			//printf("%d %d: %f %f %f | %d %d %d\n", i, m, dv->normal[0],dv->normal[1], dv->normal[2], ptr[0],ptr[1], ptr[2]   );

			ConvToIdTech4(dv->xyz);
		}
		/*
		idDrawVert *dv = vertexs + i * 3;
		idBrushDef3Side side;
		side.PlaneFromPoints(dv[0].xyz, dv[1].xyz, dv[2].xyz);
		printf("nnn %s\n", side.Plane().Normal().ToString().c_str());
		*/
	}
#endif

	idStr invisibleMaterial = /*material : */idStr(idMaterial::CAULK_MATERIAL);
	float w = (float)config->tex_width;
	for(int i = 0; i < count; i++)
	{
		idDrawVert *vertex = vertexs + i * 3;

		idBrushDef3 brush;
		idBrushDef3Side side;
		side.Material() = material;
		idVec3 points[3] = {
			vertex[0].xyz,
			vertex[1].xyz,
			vertex[2].xyz,
		};
		idVec3 v_normal = vertex[0].normal;

		// raw front
		if(!side.PlaneFromPoints(points[0], points[1], points[2]))
		{
			return false;
		}

		idPlane tmpPlane = side.Plane();
		if(!side.FromDrawVerts(vertex))
		{
			Log("Vertex((xyz(%s), uv(%s)), (xyz(%s), uv(%s)), (xyz(%s), uv(%s)), (normal(%s))) can't generate texture matrix", vertex[0].xyz.ToString().c_str(), vertex[0].st.ToString().c_str(), vertex[1].xyz.ToString().c_str(), vertex[1].st.ToString().c_str(), vertex[2].xyz.ToString().c_str(), vertex[2].st.ToString().c_str(), v_normal.ToString().c_str());
			side.TextureMatrix(0)[0] = 1.0f / w;
			side.TextureMatrix(1)[1] = 1.0f / w;
		}

		brush << side;

		side.TextureMatrix(0).Zero();
		side.TextureMatrix(1).Zero();
		side.TextureMatrix(0)[0] = 1.0f / w;
		side.TextureMatrix(1)[1] = 1.0f / w;

		// back
		side.Material() = invisibleMaterial;
		idVec3 normal_ = -v_normal;
		idVec3 points_[3];
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

		brushes.push_back(brush);
	}
	return true;
}

#if 0
bool idNETLizardConverter::GenMapBrush(idBrushDef3 &brush, const idBounds &bv, const char *material, bool invert) const
{
	idVec3 _points[8];
	idBounds bb{{-50.0f},{50.0f}};
	bb.ToPoints(_points);
	/*
	        7--------6
          / |      / |
	   4--------5    |
	   |    |   |    |
       |    |   |    |
	   |    3--------2
	   |  /     |  /	
	   0--------1
	 */

	idVec3 vs[24];
	// bottom
	vs[0] = _points[0];
	vs[1] = _points[3];
	vs[2] = _points[2];
	vs[3] = _points[1];
	// top
	vs[4] = _points[4];
	vs[5] = _points[5];
	vs[6] = _points[6];
	vs[7] = _points[7];
	// left
	vs[8] = _points[0];
	vs[9] = _points[4];
	vs[10] = _points[7];
	vs[11] = _points[3];
	// right
	vs[12] = _points[1];
	vs[13] = _points[2];
	vs[14] = _points[6];
	vs[15] = _points[5];
	// front
	vs[16] = _points[0];
	vs[17] = _points[1];
	vs[18] = _points[5];
	vs[19] = _points[4];
	// back
	vs[20] = _points[3];
	vs[21] = _points[7];
	vs[22] = _points[6];
	vs[23] = _points[2];

	for(int i = 0; i < 6; i++)
	{
		if(!GenMapBrush(brush, &vs[i * 4], material, invert))
			return false;
	}

	return true;
}
#endif

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
	side.TextureMatrix(0)[0] = 1.0f / w;
	side.TextureMatrix(1)[1] = 1.0f / w;
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
    format = config->lvl_path_format;
	if(game == NL_SHADOW_OF_EGYPT_3D) // has a survise mode(dm1) map and main menu map(lvl0)
	{
		{
			if(ConvertMap("dm1.png", -1) == 0)
				res++;
		}
		for(i = 0; i < config->level_count; i++)
		{
			file = idStr::va(format, i);
			if(ConvertMap(idStr(file), i) == 0)
				res++;
		}
	}
	else
	{
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
