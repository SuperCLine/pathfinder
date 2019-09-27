#include "stdafx.h"
#include "pathfinder_navmesh.h"


__BEGIN_NAMESPACE

static const int32 NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
static const int32 NAVMESHSET_VERSION = 1;
static const int32 MAX_NODE_NUM = 8192;

pathfinder_navmesh::pathfinder_navmesh(void)
: m_navmesh(0)
, m_navquary(0)
{

}

pathfinder_navmesh::pathfinder_navmesh(float32 radius)
	: m_navmesh(0)
	, m_navquary(0)
{
	m_navmesh_param.agent_radius = radius;
}

pathfinder_navmesh::~pathfinder_navmesh(void)
{
	if (m_navquary)
	{
		dtFreeNavMeshQuery(m_navquary);
		m_navquary = 0;
	}
	if (m_navmesh)
	{
		dtFreeNavMesh(m_navmesh);
		m_navmesh = 0;
	}
}

bool pathfinder_navmesh::init(const char* file)
{
	std::FILE* fp = app_fopen(file, "rb");
	if (!fp)
		return false;

	app_fread(&m_navmesh_param, sizeof(navmesh_param_type), 1, fp);
	if (m_navmesh_param.magic != NAVMESHSET_MAGIC || m_navmesh_param.version != NAVMESHSET_VERSION)
	{
		app_fclose(fp);
		return false;
	}

	if (!build())
	{
		app_fclose(fp);
		return false;
	}

	for (int32 i = 0; i < m_navmesh_param.num_tiles; ++i)
	{
		navmesh_tile_type tile_param;
		app_fread(&tile_param, sizeof(navmesh_tile_type), 1, fp);
		if (!tile_param.tile_ref || !tile_param.data_size)
			break;

		unsigned char* tile = (unsigned char*)dtAlloc(tile_param.data_size, DT_ALLOC_PERM);
		if (!tile)
			break;

		app_fread(tile, tile_param.data_size, 1, fp);
		if (dtStatusFailed(m_navmesh->addTile(tile, tile_param.data_size, DT_TILE_FREE_DATA, tile_param.tile_ref, 0)))
		{
			dtFree(tile);
		}
	}

	fclose(fp);
	return true;
}

bool pathfinder_navmesh::init(void* data, size_t size)
{
	char* pcur = app_cast_static(char*, data);
	char* pend = pcur + size;

	navmesh_param_type* nav_param = (navmesh_param_type*)(pcur);
	pcur += sizeof(navmesh_param_type);

	if (nav_param->magic != NAVMESHSET_MAGIC || nav_param->version != NAVMESHSET_VERSION)
		return false;

	m_navmesh_param = (*nav_param);
	if (!build())
		return false;

	while (pcur < pend)
	{
		navmesh_tile_type* tile_param = (navmesh_tile_type*)(pcur);
		pcur += sizeof(navmesh_tile_type);

		if (!tile_param->tile_ref || !tile_param->data_size)
			break;

		unsigned char* tile = (unsigned char*)dtAlloc(tile_param->data_size, DT_ALLOC_PERM);
		if (!tile)
			break;

		app_memcpy(tile, pcur, tile_param->data_size);
		pcur += tile_param->data_size;

		if (dtStatusFailed(m_navmesh->addTile(tile, tile_param->data_size, DT_TILE_FREE_DATA, tile_param->tile_ref, 0)))
		{
			dtFree(tile);
		}
	}

	return true;
}

bool pathfinder_navmesh::build(void)
{
	m_navmesh = dtAllocNavMesh();
	if (dtStatusFailed(m_navmesh->init(&m_navmesh_param.params)))
		return false;

	m_navquary = dtAllocNavMeshQuery();
	if (dtStatusFailed(m_navquary->init(m_navmesh, MAX_NODE_NUM)))
		return false;

	return true;
}

__END_NAMESPACE
