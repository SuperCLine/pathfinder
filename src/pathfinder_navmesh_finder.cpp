#include "stdafx.h"
#include "pathfinder_navmesh_finder.h"


__BEGIN_NAMESPACE

pathfinder_navmesh_finder::pathfinder_navmesh_finder(void)
{

}

pathfinder_navmesh_finder::~pathfinder_navmesh_finder(void)
{

}

void pathfinder_navmesh_finder::path_finding(pathfinder_path* path)
{
	pathfinder_navmesh* mesh = path->get_path_map()->find_navmesh(path->get_radius());
	if (!mesh)
	{
		path->set_path_result(EPR_NOMAP);
		return;
	}

	dtNavMeshQuery* query = mesh->get_navquary();
	dtQueryFilter* filter = path->get_path_filter();
	if (!filter)
		filter = &m_filter;

	dtPolyRef start_ref = find_nearest_poly(query, filter, path->get_start_point(), 2, 4, 3, 1);
	if (!start_ref)
	{
		path->set_path_result(EPR_NOSTART);
		return;
	}

	dtPolyRef end_ref = find_nearest_poly(query, filter, path->get_end_point(), 2, 4, 3, 1);
	if (!end_ref)
	{
		path->set_path_result(EPR_NOEND);
		return;
	}

	uint32 straight_count = 0;
	pathfinder_pathpoint_pool::pathpoint_chunk* straight_path = 0;

	EPathResult er = find_path(query, filter, path, start_ref, end_ref, &straight_path, straight_count);
	if (er == EPR_NOPATH)
	{
		path->set_path_result(EPR_NOPATH);
	}
	else
	{
		path->set_path_data(straight_path, straight_count);
		path->set_path_result(er);
	}
}

__END_NAMESPACE
