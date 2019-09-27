#include "stdafx.h"
#include "pathfinder_finder.h"


__BEGIN_NAMESPACE

dtPolyRef pathfinder_finder::find_nearest_poly(dtNavMeshQuery* query, 
	dtQueryFilter* filter, 
	const float32* center, 
	float32 extent, 
	float32 height, 
	int32 extent_times /*= 6*/, 
	int32 height_times /*= 6*/)
{
	dtPolyRef ref = 0;
	float32 poly_pick_ext[3] = { extent, height, extent };
	
	extent_times = util_clamp<int32>(extent_times, 1, 10);
	height_times = util_clamp<int32>(height_times, 1, 10);

	for (int32 i = 0; i < height_times; ++i)
	{
		poly_pick_ext[1] = height * (1 << i);
		for (int32 j = 0; j < extent_times; ++j)
		{
			poly_pick_ext[0] = poly_pick_ext[2] = extent * (1 << j);

			dtStatus status = query->findNearestPoly(center, poly_pick_ext, filter, &ref, 0);

			if (dtStatusSucceed(status) && ref)
				return ref;
		}
	}

	return ref;
}

EPathResult pathfinder_finder::find_path(dtNavMeshQuery* query, 
	dtQueryFilter* filter, 
	pathfinder_path* path, 
	dtPolyRef start_ref, 
	dtPolyRef end_ref, 
	pathfinder_pathpoint_pool::pathpoint_chunk** data, 
	uint32& data_len)
{
	pathfinder_mgr::polyref_chunk* poly_chunk = app_cast_static(pathfinder_mgr::polyref_chunk*, path->get_response()->get_data());

	int32 poly_count = 0;
	dtStatus status = query->findPath(start_ref, 
		end_ref, 
		path->get_start_point(), 
		path->get_end_point(), 
		filter,
		poly_chunk->chunk_data, &poly_count, pathfinder_mgr::polyref_chunk::CHUNKSIZE);

	if (!poly_count)
		return EPR_NOPATH;

	app_assert(poly_count <= pathfinder_mgr::polyref_chunk::CHUNKSIZE);
	int32 min_poly_count = util_min<int32>(poly_count, (int32)pathfinder_mgr::polyref_chunk::CHUNKSIZE);

	pathfinder_pathpoint_pool::pathpoint_chunk* path_point_chunk = path->get_path_pointpool_policy()->get(min_poly_count * 3);

	int32 used_count = 0;
	float32 epos[3] = { 0 };
	dtVcopy(epos, path->get_end_point());
	if (poly_chunk->chunk_data[min_poly_count - 1] != end_ref)
	{
		query->closestPointOnPoly(poly_chunk->chunk_data[min_poly_count - 1], path->get_end_point(), epos, 0);
	}

	status = query->findStraightPath(path->get_start_point(), 
		epos, 
		poly_chunk->chunk_data, 
		min_poly_count,
		path_point_chunk->get_chunk(), 
		poly_chunk->chunk_flag, 
		poly_chunk->chunk_temp, 
		&used_count, 
		path_point_chunk->get_capacity(), 
		DT_STRAIGHTPATH_AREA_CROSSINGS);

	data_len = 0;
	EPathResult result = EPR_NOPATH;
	if (used_count > 1)
	{
		if (poly_chunk->chunk_data[min_poly_count - 1] != end_ref)
			result = EPR_NOCONNECT;
		else
			result = EPR_SUCCEED;

		*data = path_point_chunk;
		data_len = used_count;
	}
	else
	{
		result = EPR_NOPATH;
		path->get_path_pointpool_policy()->cycle(path_point_chunk);
	}

	return result;
}

__END_NAMESPACE
