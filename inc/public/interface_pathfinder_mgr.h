/*------------------------------------------------------------------------------
|
| COPYRIGHT (C) 2018 - 2026 All Right Reserved
|
| FILE NAME  : \pathfinder\inc\public\interface_pathfinder_mgr.h
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


#ifndef _INTERFACE_PATHFINDER_MGR_H_
#define _INTERFACE_PATHFINDER_MGR_H_


__BEGIN_NAMESPACE


typedef uint32 path_id_type;
#define path_invalid_id path_id_type(-1)

enum EPathFinderPolicy
{
	EPFP_NAVMESH,
	EPFP_ASTAR,
};

enum EPathState
{
	EPS_IDLE = 0,
	EPS_FINDING,
	EPS_SUCCEED,
	EPS_FAILEDNOCONNECT,//have path data but no connection
	EPS_FAILEDNODATA,//no path data
};

class interface_path_mgr
{
public:
	virtual ~interface_path_mgr(void) {}

	virtual bool init(interface_logmgr* log, int32 thread_num = 4) = 0;
	virtual void destroy(void) = 0;
	virtual void update(float32 delta_tick) = 0;

	//path
	virtual path_id_type create_path(const float32* start, 
		const float32* end, 
		float32 radius = 0.6f, 
		bool use_filter = false, 
		EPathFinderPolicy efinder = EPFP_NAVMESH, 
		bool use_thread = true) = 0;
	virtual void destroy_path(path_id_type id) = 0;
	virtual EPathState get_path(path_id_type id, float32** path, uint32& count) = 0;

	//filter[0, 31]
	virtual bool set_filter(uint32 area_id, float32 area_cost) = 0;

	//navigation mesh
	virtual bool load_navmesh(const char* file) = 0;
	virtual bool load_navmesh(void* data, int32 size) = 0;

	//get random point on whole navigation mesh which radius is more small or on the specified polygon
	virtual int32 get_random_point(float32 agent_radius, 
		const float32* center, 
		float32 search_radius, 
		uint32 max_points, 
		float32* points) = 0;
	virtual bool raycast(float32 agent_radius, const float32* start, const float32* end, float32* hit_pos) = 0;

	//crowd
	virtual bool init_crowd(bool use_thread = false, float32 radius = 0.6f, int32 max_agent = 512) = 0;
	virtual int32 get_agent(const float32* pos, 
		float32 agent_speed, 
		float32 agent_radius = 0.6f, 
		float32 agent_height = 2.0f, 
		float32 agent_acceleration = 8.0f) = 0;
	virtual void cycle_agent(int32 id) = 0;
	virtual bool move_to_target(int32 id, const float32* target_pos) = 0;
	virtual void change_speed(int32 id, float32 speed) = 0;
	virtual const float32* get_agent_position(int32 id) = 0;
};

extern "C"
{
	PATHFINDER_API interface_path_mgr*	load_module_pathfinder(void);
	PATHFINDER_API void					unload_module_pathfinder(interface_path_mgr* p);
};

__END_NAMESPACE


#endif //_INTERFACE_PATHFINDER_MGR_H_