#include "stdafx.h"
#include "pathfinder_path.h"


__BEGIN_NAMESPACE

pathfinder_path::pathfinder_path(void)
	: m_path_map(0)
	, m_path_finder(0)
	, m_path_filter(0)
	, m_pool_policy(0)
	, m_radius(0.f)
	, m_request_id(0)
	, m_response(0)
	, m_pathpoint_chunk(0)
	, m_pathpoint_count(0)
	, m_path_result(EPR_INIT)
	, m_finished(false)
{

}

pathfinder_path::~pathfinder_path(void)
{
	destroy();
}

void pathfinder_path::init(const float32* start, 
	const float32* end, 
	float32 radius, 
	pathfinder_finder* pfinder, 
	pathfinder_map* pmap,
	pathfinder_pathpoint_pool* pool_policy, 
	dtQueryFilter* filter /*= 0*/)
{
	m_radius = radius;
	m_path_map = pmap;
	m_pool_policy = pool_policy;
	m_path_finder = pfinder;
	m_pathpoint_chunk = 0;
	m_pathpoint_count = 0;
	m_finished = false;
	m_path_result = EPR_INIT;

	m_response = 0;

	m_start[0] = *(start);
	m_start[1] = *(start + 1);
	m_start[2] = *(start + 2);

	m_end[0] = *(end);
	m_end[1] = *(end + 1);
	m_end[2] = *(end + 2);

	m_path_filter = filter;
}

void pathfinder_path::destroy(void)
{
	if (m_pathpoint_chunk)
	{
		m_pool_policy->cycle(m_pathpoint_chunk);
		m_pathpoint_chunk = 0;
		m_pathpoint_count = 0;
	}

	m_path_map = 0;
	m_path_finder = 0;
	m_pool_policy = 0;
	m_request_id = 0;
	m_response = 0;
	m_path_filter = 0;
	m_path_result = EPR_INIT;
}

__END_NAMESPACE
