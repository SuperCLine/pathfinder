/*------------------------------------------------------------------------------
|
| COPYRIGHT (C) 2018 - 2026 All Right Reserved
|
| FILE NAME  : \pathfinder\inc\private\pathfinder_mgr.h
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


#ifndef _PATHFINDER_MGR_H_
#define _PATHFINDER_MGR_H_


__BEGIN_NAMESPACE

class pathfinder_mgr : public core_workqueue::request_handler
	, public core_workqueue::response_handler
	, public core_interface
	, public interface_path_mgr
{
public:
	pathfinder_mgr(void);
	virtual ~pathfinder_mgr(void);

	virtual bool init(interface_logmgr* log, int32 thread_num = 4);
	virtual void destroy(void);
	virtual void update(float32 delta_tick);

	//path
	virtual path_id_type create_path(const float32* start, 
		const float32* end, 
		float32 radius = 0.6f, 
		bool use_filter = false, 
		EPathFinderPolicy efinder = EPFP_NAVMESH, 
		bool use_thread = true);
	virtual void destroy_path(path_id_type id);
	virtual EPathState get_path(path_id_type id, float32** path, uint32& count);

	//filter[0, 31]
	virtual bool set_filter(uint32 area_id, float32 area_cost);

	//navigation mesh
	virtual bool load_navmesh(const char* file);
	virtual bool load_navmesh(void* data, int32 size);

	//get random point on whole navigation mesh which radius is more small or on the specified polygon
	virtual int32 get_random_point(float32 agent_radius, 
		const float32* center, 
		float32 search_radius, 
		uint32 max_points, 
		float32* points);
	virtual bool raycast(float32 agent_radius, const float32* start, const float32* end, float32* hit_pos);

	//crowd
	virtual bool init_crowd(bool use_thread = false, float32 radius = 0.6f, int32 max_agent = 512);
	virtual int32 get_agent(const float32* pos, 
		float32 agent_speed, 
		float32 agent_radius = 0.6f, 
		float32 agent_height = 2.0f, 
		float32 agent_acceleration = 8.0f);
	virtual void cycle_agent(int32 id);
	virtual bool move_to_target(int32 id, const float32* target_pos);
	virtual void change_speed(int32 id, float32 speed);
	virtual const float32* get_agent_position(int32 id);

	//handler
	virtual core_workqueue::response* handle_request(const core_workqueue::request* req, const core_workqueue* srcQ);
	virtual void handle_response(const core_workqueue::response* res, const core_workqueue* srcQ);

private:
	bool is_valid(path_id_type id);
	void update_internal(void);

private:
	typedef umap<EPathFinderPolicy, pathfinder_finder*>				finder_map_type;
	typedef umap<EPathFinderPolicy, pathfinder_finder*>::iterator	finder_itr_type;
	typedef ulist<pathfinder_path*>									path_list_type;
	typedef ulist<pathfinder_path*>::iterator						path_itr_type;

	pathfinder_path_pool		m_path_pool;
	pathfinder_map				m_path_map;
	finder_map_type				m_pathfinder_map;
	path_list_type				m_path_list;
	path_list_type				m_path_cycle_list;
	umutex						m_path_list_mutex;
	umutex						m_path_cycle_list_mutex;
	pathfinder_pathpoint_pool	m_pathpoint_pool;
	dtQueryFilter				m_path_filter;
	uint16						m_work_channel;
	core_default_workqueue*		m_work_queue;
	
	struct CrowdImpl*			m_crowd_impl;


	enum ERequestType
	{
		ERT_PATHFIND,
		ERT_RESERVED = 0xffff,
	};

	struct polyref_chunk
	{
		enum
		{
			CHUNKSIZE = 2048,
		};
		uint8		chunk_flag[CHUNKSIZE];
		dtPolyRef	chunk_data[CHUNKSIZE];
		dtPolyRef	chunk_temp[CHUNKSIZE];
	};
	core_pool<polyref_chunk>		m_tile_polyref_pool;

	friend class pathfinder_finder;
};

__END_NAMESPACE


#endif //_PATHFINDER_MGR_H_