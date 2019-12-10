#include "stdafx.h"
#include "pathfinder_mgr.h"


__BEGIN_NAMESPACE

PATHFINDER_API interface_path_mgr* load_module_pathfinder(void)
{
	return new pathfinder_mgr();
}

PATHFINDER_API void unload_module_pathfinder(interface_path_mgr* p)
{
	app_safe_delete(p);
}

struct CrowdImpl
{
	bool				use_thread;
	dtCrowd*			crowd;
	pathfinder_navmesh*	nav_mesh;
	core_thread*		proc_thread;
	volatile bool		has_exit;

	CrowdImpl(bool t, pathfinder_navmesh* mesh) : use_thread(t), crowd(0), nav_mesh(mesh), proc_thread(0), has_exit(false)
	{
		crowd = dtAllocCrowd();

		if (use_thread)
		{
			proc_thread = new core_thread(std::bind(&CrowdImpl::process, this));
		}
	}

	~CrowdImpl(void)
	{
		has_exit = true;
		app_safe_delete(proc_thread);

		if (crowd)
		{
			dtFreeCrowd(crowd);
			crowd = 0;
		}
	}

	void update(float32 delta_tick)
	{
		if (use_thread) return;

		crowd->update(delta_tick, 0);
	}

	void process(void)
	{
		int64 cur_tick = core_time::get_tick();
		int64 last_tick = cur_tick;
		while (!has_exit)
		{
			cur_tick = core_time::get_tick();
			int64 delta_tick = cur_tick - last_tick;
			last_tick = cur_tick;

			if (delta_tick > 0)
			{
				crowd->update(delta_tick * 0.001f, 0);
			}

			core_thread::sleep(5);
		}
	}
};


pathfinder_mgr::pathfinder_mgr(void)
	: m_work_queue(0)
	, m_work_channel(0)
	, m_crowd_impl(0)
{
	m_pathfinder_map[EPFP_NAVMESH] = new pathfinder_navmesh_finder();
}

pathfinder_mgr::~pathfinder_mgr(void)
{
	destroy();
	set_pathfinder_logger(0);
}

bool pathfinder_mgr::init(interface_logmgr* log, int32 thread_num/* = 4*/)
{
	set_pathfinder_logger(log);

	m_work_queue = new core_default_workqueue("path_woker");
	m_work_queue->set_response_processing_timelimit(10);

	m_work_channel = m_work_queue->get_channel("pathfinder_channel");
	m_work_queue->add_request_handler(m_work_channel, this);
	m_work_queue->add_response_handler(m_work_channel, this);

	m_work_queue->startup(thread_num);

	return true;
}

void pathfinder_mgr::destroy(void)
{
	if (m_work_queue)
	{
		m_work_queue->remove_request_handler(m_work_channel, this);
		m_work_queue->remove_response_handler(m_work_channel, this);
		m_work_queue->shutdown();
	}

	for (path_itr_type itr = m_path_list.begin(); itr != m_path_list.end(); ++itr)
	{
		app_safe_delete(*itr);
	}
	m_path_list.clear();

	for (path_itr_type itr = m_path_cycle_list.begin(); itr != m_path_cycle_list.end(); ++itr)
	{
		app_safe_delete(*itr);
	}
	m_path_cycle_list.clear();

	for (finder_itr_type itr = m_pathfinder_map.begin(); itr != m_pathfinder_map.end(); ++itr)
	{
		app_safe_delete(itr->second);
	}
	m_pathfinder_map.clear();

	app_safe_delete(m_work_queue);
	app_safe_delete(m_crowd_impl);
}

void pathfinder_mgr::update(float32 delta_tick)
{
	m_work_queue->process_responses();
	m_pathpoint_pool.update(delta_tick);

	if (m_crowd_impl)
	{
		m_crowd_impl->update(delta_tick);
	}

	update_internal();
}

path_id_type pathfinder_mgr::create_path(const float32* start, 
	const float32* end, 
	float32 radius /*= 0.6f*/, 
	bool use_filter /*= false*/, 
	EPathFinderPolicy efinder /*= EPFP_NAVMESH*/, 
	bool use_thread /*= true*/)
{
	if (dtVdistSqr(start, end) <= 0.25f || !m_path_map.has_map()) 
		return path_invalid_id;

	pathfinder_path* path = m_path_pool.get();
	pathfinder_finder* finder = m_pathfinder_map[efinder];
	path->init(start, end, radius, finder, &m_path_map, &m_pathpoint_pool, (use_filter ? &m_path_filter : 0));

	core_workqueue::request_id_type rid = m_work_queue->add_request(m_work_channel, ERT_PATHFIND, path, 0, !use_thread);
	path->set_request_id(rid);

	ulocker scoped_locker(m_path_list_mutex);
	m_path_list.push_back(path);

	return app_make_value(path);
}

void pathfinder_mgr::destroy_path(path_id_type id)
{
	if (!is_valid(id)) 
		return;

	umutex list_scoped_lock(m_path_list_mutex);
	for (path_itr_type itr = m_path_list.begin(); itr != m_path_list.end(); ++itr)
	{
		pathfinder_path* path = (*itr);
		if (app_make_value(path) == id)
		{
			{
				umutex cycle_list_scoped_lock(m_path_cycle_list_mutex);
				m_path_cycle_list.push_back(path);
			}

			m_path_list.erase(itr);

			break;
		}
	}
}

EPathState pathfinder_mgr::get_path(path_id_type id, float32** path, uint32& count)
{
	if (!is_valid(id) || !path)
		return EPS_FAILEDNODATA;

	pathfinder_path* path_obj = (pathfinder_path*)app_make_pointer(id);
	if (!path_obj->get_finished()) 
		return EPS_FINDING;

	EPathResult e = path_obj->get_path_reslut();
	if (EPR_NOMAP == e || EPR_NOSTART == e || EPR_NOEND == e || EPR_NOPATH == e)
		return EPS_FAILEDNODATA;

	*path = path_obj->get_pathpoint_chunk()->get_chunk();
	count = path_obj->get_pathpoint_count();

	return (EPR_SUCCEED == e ? EPS_SUCCEED : EPS_FAILEDNOCONNECT);
}

bool pathfinder_mgr::set_filter(uint32 area_id, float32 area_cost)
{
	if (area_id > 31)
		return false;

	m_path_filter.setAreaCost(area_id, area_cost);

	return true;
}

bool pathfinder_mgr::load_navmesh(const char* file)
{
	if (!file || !app_strlen(file))
		return false;

	pathfinder_navmesh* mesh = new pathfinder_navmesh();
	if (!mesh->init(file))
	{
		app_safe_delete(mesh);
		return false;
	}

	m_path_map.add_navmesh(mesh);

	return true;
}

bool pathfinder_mgr::load_navmesh(void* data, int32 size)
{
	if (!data)
		return false;

	pathfinder_navmesh* mesh = new pathfinder_navmesh();
	if (!mesh->init(data, size))
	{
		app_safe_delete(mesh);
		return false;
	}

	m_path_map.add_navmesh(mesh);

	return true;
}

int32 pathfinder_mgr::get_random_point(float32 agent_radius, 
	const float32* center, 
	float32 search_radius, 
	uint32 max_points, 
	float32* points)
{
	pathfinder_navmesh* mesh = m_path_map.find_navmesh(agent_radius);
	if (!mesh)
		return 0;

	dtNavMeshQuery* query = mesh->get_navquary();
	dtQueryFilter filter;

	if (agent_radius <= 0.0001f)
	{
		int32 num = 0;
		for (uint32 i = 0; i < max_points; ++i)
		{
			float32 pt[3] = { 0 };
			dtPolyRef ref = 0;
			dtStatus status = query->findRandomPoint(&filter, core_util::random_float32, &ref, pt);
			if (dtStatusSucceed(status))
			{
				dtVcopy(&points[num * 3], pt);
				num++;
			}
		}

		return num;
	}

	const float32 extents[3] = { 2.f, 4.f, 2.f };
	dtPolyRef startRef = 0;

	query->findNearestPoly(center, extents, &filter, &startRef, 0);
	if (!startRef)
		return 0;

	int32 num = 0;
	for (uint32 i = 0; i < max_points; ++i)
	{
		float32 pt[3] = { 0 };
		dtPolyRef ref = 0;
		dtStatus status = query->findRandomPointAroundCircle(startRef, center, search_radius, &filter, core_util::random_float32, &ref, pt);
		if (dtStatusSucceed(status))
		{
			dtVcopy(&points[num * 3], pt);
			num++;
		}
	}

	return num;
}

bool pathfinder_mgr::raycast(float32 agent_radius, const float32* start, const float32* end, float32* hit_pos)
{
	pathfinder_navmesh* mesh = m_path_map.find_navmesh(agent_radius);
	if (!mesh)
		return false;

	dtNavMeshQuery* query = mesh->get_navquary();
	dtQueryFilter filter;

	const float32 extents[3] = { 2.f, 4.f, 2.f };
	dtPolyRef startRef = 0;

	query->findNearestPoly(start, extents, &filter, &startRef, 0);
	if (!startRef)
		return false;

	float32 t = 0;
	int32 npolys = 0;
	dtPolyRef polys[512] = { 0 };
	float32 hit_normal[3] = { 0 };

	app_memset(hit_normal, 0, app_array_len(hit_normal));

	query->raycast(startRef, start, end, &filter, &t, hit_normal, polys, &npolys, 512);

	if (t > 1)
	{
		// no hit
		return false;
	}
	else
	{
		// hit
		dtVlerp(hit_pos, start, end, t);
		if (npolys > 0)
		{
			float32 h = 0;
			query->getPolyHeight(polys[npolys - 1], hit_pos, &h);
			hit_pos[1] = h;
		}
		return true;
	}
}

bool pathfinder_mgr::init_crowd(bool use_thread /*= false*/, float32 radius /*= 0.6f*/, int32 max_agent /*= 512*/)
{
	if (m_crowd_impl)
		return true;

	pathfinder_navmesh* mesh = m_path_map.find_navmesh(radius);
	if (!mesh)
		return false;

	m_crowd_impl = new CrowdImpl(use_thread, mesh);
	if (!m_crowd_impl->crowd->init(max_agent, radius, mesh->get_navmesh()))
	{
		app_safe_delete(m_crowd_impl);
		return false;
	}

	// setup local avoidance params to different qualities.
	dtObstacleAvoidanceParams params;
	// use mostly default settings, copy from dtCrowd.
	memcpy(&params, m_crowd_impl->crowd->getObstacleAvoidanceParams(0), sizeof(dtObstacleAvoidanceParams));

	// Low (11)
	params.velBias = 0.5f;
	params.adaptiveDivs = 5;
	params.adaptiveRings = 2;
	params.adaptiveDepth = 1;
	m_crowd_impl->crowd->setObstacleAvoidanceParams(0, &params);

	// Medium (22)
	params.velBias = 0.5f;
	params.adaptiveDivs = 5;
	params.adaptiveRings = 2;
	params.adaptiveDepth = 2;
	m_crowd_impl->crowd->setObstacleAvoidanceParams(1, &params);

	// Good (45)
	params.velBias = 0.5f;
	params.adaptiveDivs = 7;
	params.adaptiveRings = 2;
	params.adaptiveDepth = 3;
	m_crowd_impl->crowd->setObstacleAvoidanceParams(2, &params);

	// High (66)
	params.velBias = 0.5f;
	params.adaptiveDivs = 7;
	params.adaptiveRings = 3;
	params.adaptiveDepth = 3;

	m_crowd_impl->crowd->setObstacleAvoidanceParams(3, &params);

	if (m_crowd_impl->proc_thread)
		m_crowd_impl->proc_thread->start();

	return true;
}

int32 pathfinder_mgr::get_agent(const float32* pos, 
	float32 agent_speed, 
	float32 agent_radius /*= 0.6f*/, 
	float32 agent_height /*= 2.0f*/, 
	float32 agent_acceleration /*= 8.0f*/)
{
	if (!m_crowd_impl)
		return -1;

	dtCrowdAgentParams params;
	memset(&params, 0, sizeof(params));
	params.radius = agent_radius;
	params.height = agent_height;
	params.maxAcceleration = agent_acceleration;
	params.maxSpeed = agent_speed;
	params.collisionQueryRange = params.radius * 12.0f;
	params.pathOptimizationRange = params.radius * 30.0f;
	params.updateFlags = DT_CROWD_ANTICIPATE_TURNS | DT_CROWD_OPTIMIZE_VIS | DT_CROWD_OPTIMIZE_TOPO | DT_CROWD_OBSTACLE_AVOIDANCE;
	params.obstacleAvoidanceType = (unsigned char)3;
	params.separationWeight = 2;

	return m_crowd_impl->crowd->addAgent(pos, &params);
}

void pathfinder_mgr::cycle_agent(int32 id)
{
	if (!m_crowd_impl)
		return;

	m_crowd_impl->crowd->removeAgent(id);
}

bool pathfinder_mgr::move_to_target(int32 id, const float32* target_pos)
{
	if (!m_crowd_impl)
		return false;

	const dtQueryFilter* filter = m_crowd_impl->crowd->getFilter(0);
	const float32* ext = m_crowd_impl->crowd->getQueryExtents();
	dtNavMeshQuery* query = m_crowd_impl->nav_mesh->get_navquary();

	dtPolyRef targetRef;
	float32 targetPos[3] = { 0 };
	if (dtStatusFailed(query->findNearestPoly(target_pos, ext, filter, &targetRef, targetPos)))
	{
		dtVcopy(targetPos, target_pos);
	}

	const dtCrowdAgent* agent = m_crowd_impl->crowd->getAgent(id);
	if (agent && agent->active)
	{
		return m_crowd_impl->crowd->requestMoveTarget(id, targetRef, targetPos);
	}

	return false;
}

void pathfinder_mgr::change_speed(int32 id, float32 speed)
{
	if (!m_crowd_impl)
		return;

	const dtCrowdAgent* agent = m_crowd_impl->crowd->getAgent(id);
	if (agent && agent->active)
	{
		dtCrowdAgentParams params;
		memcpy(&params, &agent->params, sizeof(dtCrowdAgentParams));
		params.maxSpeed = speed;
		m_crowd_impl->crowd->updateAgentParameters(id, &params);
	}
}

const float32* pathfinder_mgr::get_agent_position(int32 id)
{
	if (!m_crowd_impl)
		return 0;

	const dtCrowdAgent* agent = m_crowd_impl->crowd->getAgent(id);
	return ((agent != 0) ? agent->npos : 0);
}

core_workqueue::response* pathfinder_mgr::handle_request(const core_workqueue::request* req, const core_workqueue* srcQ)
{
	app_unreferenced(srcQ);
	pathfinder_path* path = app_cast_static(pathfinder_path*, req->get_data());

	polyref_chunk* chunk = m_tile_polyref_pool.get();

	core_workqueue::response* res = new core_workqueue::response(req, true, (void*)chunk, false);
	app_assert(res);
	path->set_response(res);

	path->get_path_finder()->path_finding(path);

	return res;
}

void pathfinder_mgr::handle_response(const core_workqueue::response* res, const core_workqueue* srcQ)
{
	app_unreferenced(srcQ);
	pathfinder_path* path = app_cast_static(pathfinder_path*, res->get_request()->get_data());
	path->set_response(0);
	path->set_finished(true);

	polyref_chunk* chunk = app_cast_static(polyref_chunk*, res->get_data());
	
	app_assert(chunk);
	m_tile_polyref_pool.cycle(chunk);
}

bool pathfinder_mgr::is_valid(path_id_type id)
{
	if (path_invalid_id == id)
		return false;

	ulocker scoped_locker(m_path_list_mutex);
	return m_path_list.end() != std::find(m_path_list.begin(), m_path_list.end(), app_cast_static(pathfinder_path*, app_make_pointer(id)));
}

void pathfinder_mgr::update_internal(void)
{
	umutex scoped_locker(m_path_cycle_list_mutex);
	for (path_itr_type itr = m_path_cycle_list.begin(); itr != m_path_cycle_list.end();)
	{
		pathfinder_path* path = (*itr);
		if (path->get_finished())
		{
			itr = m_path_cycle_list.erase(itr);
			m_path_pool.cycle(path);
		}
		else
		{
			++itr;
		}
	}
}

__END_NAMESPACE
