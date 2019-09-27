/*------------------------------------------------------------------------------
|
| COPYRIGHT (C) 2018 - 2026 All Right Reserved
|
| FILE NAME  : \pathfinder\inc\private\pathfinder_navmesh.h
| AUTHOR     : CLine
| PURPOSE    :
|
| SPEC       :
|
| MODIFICATION HISTORY
|
| Ver      Date            By              Details
| -----    -----------    -------------   ----------------------
| 1.0      2019-9-26      CLine           Created
|
+-----------------------------------------------------------------------------*/


#ifndef _PATHFINDER_NAVMESH_H_
#define _PATHFINDER_NAVMESH_H_


__BEGIN_NAMESPACE


// read from json for editor
struct navmesh_build_param_type
{
	// cell size in world units
	float32 cell_size;
	// cell height in world units
	float32 cell_height;
	// agent height in world units
	float32 agent_height;
	// agent radius in world units
	float32 agent_radius;
	// agent max climb in world units
	float32 agent_max_climb;
	// agent max slope in degrees
	float32 agent_max_slope;
	// region minimum size in voxels.
	// region_min_size = sqrt(region_min_area)
	float32 region_min_size;
	// region merge size in voxels.
	// region_merge_size = sqrt(region_merge_area)
	float32 region_merge_size;
	// edge max length in world units
	float32 edge_max_len;
	// edge max error in voxels
	float32 edge_max_error;
	float32 verts_per_poly;
	// detail sample distance in voxels
	float32 detail_sample_dist;
	// detail sample max error in voxel heights.
	float32 detail_sample_max_error;
	// partition type, see SamplePartitionType
	int32 partition_type;
	// bounds of the area to mesh
	float32 nav_mesh_bmin[3];
	float32 nav_mesh_bmax[3];
	// size of the tiles in voxels
	float32 tile_size;
};

struct navmesh_param_type
{
	int32 magic;
	int32 version;
	int32 num_tiles;
	float32 agent_radius;//TO CLine: navigation mesh with agent radius
	dtNavMeshParams params;
};

struct navmesh_tile_type
{
	dtTileRef tile_ref;
	int32 data_size;
};

class pathfinder_navmesh : public core
{
public:
	pathfinder_navmesh(void);
	~pathfinder_navmesh(void);

	// just for lower_bound tricker
	explicit pathfinder_navmesh(float32 radius);

	bool init(const char* file);
	bool init(void* data, size_t size);

	inline navmesh_param_type&	get_navmesh_param(void) { return m_navmesh_param; }
	inline dtNavMesh*		get_navmesh(void) { return m_navmesh; }
	inline dtNavMeshQuery*	get_navquary(void) { return m_navquary; }

private:
	bool build(void);

private:
	navmesh_param_type		m_navmesh_param;

	dtNavMesh*			m_navmesh;
	dtNavMeshQuery*		m_navquary;
};


__END_NAMESPACE


#endif //_PATHFINDER_NAVMESH_H_