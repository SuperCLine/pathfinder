#include "stdafx.h"
#include "pathfinder_map.h"


__BEGIN_NAMESPACE

struct TPathCompare
{
	bool operator()(pathfinder_navmesh* l, pathfinder_navmesh* r)
	{
		return l->get_navmesh_param().agent_radius < r->get_navmesh_param().agent_radius;
	}
};

pathfinder_map::pathfinder_map(void)
{

}

pathfinder_map::~pathfinder_map(void)
{
	for (auto itr = m_navmesh_list.begin(); itr != m_navmesh_list.end(); ++itr)
	{
		app_safe_delete(*itr);
	}
	m_navmesh_list.clear();
}

void pathfinder_map::add_navmesh(pathfinder_navmesh* mesh)
{
	ulist<pathfinder_navmesh*>::iterator itr = std::upper_bound(m_navmesh_list.begin(), m_navmesh_list.end(), mesh, TPathCompare());
	m_navmesh_list.insert(itr, mesh);
}

pathfinder_navmesh* pathfinder_map::find_navmesh(float32 radius)
{
	std::unique_ptr<pathfinder_navmesh> ptest(new pathfinder_navmesh(radius));
	ulist<pathfinder_navmesh*>::iterator itr = std::lower_bound(m_navmesh_list.begin(), m_navmesh_list.end(), ptest.get(), TPathCompare());
	if (itr == m_navmesh_list.end())
		return m_navmesh_list.back();

	return (*itr);
}

bool pathfinder_map::has_map(void)
{
	return m_navmesh_list.size() > 0;
}

__END_NAMESPACE
