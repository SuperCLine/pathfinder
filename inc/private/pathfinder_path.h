/*------------------------------------------------------------------------------
|
| COPYRIGHT (C) 2018 - 2026 All Right Reserved
|
| FILE NAME  : \pathfinder\inc\private\pathfinder_path.h
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


#ifndef _PATHFINDER_PATH_H_
#define _PATHFINDER_PATH_H_


__BEGIN_NAMESPACE

enum EPathResult
{
	EPR_SUCCEED = 0,

	EPR_INIT,
	EPR_NOMAP,
	EPR_NOPATH,
	EPR_NOSTART,
	EPR_NOEND,
	EPR_NOCONNECT,
};

class pathfinder_finder;
class pathfinder_path : public core
{
public:
	typedef typename core_workqueue::request_id_type request_id_type;
	typedef typename core_workqueue::response response_type;
	typedef typename core_workqueue::request  request_type;
	typedef typename pathfinder_pathpoint_pool::pathpoint_chunk pathpoint_chunk_type;

public:
	pathfinder_path(void);
	~pathfinder_path(void);

	void init(const float32* start, 
		const float32* end, 
		float32 radius, 
		pathfinder_finder* pfinder, 
		pathfinder_map* pmap,
		pathfinder_pathpoint_pool* pool_policy,
		dtQueryFilter* filter = 0);
	void destroy(void);

	inline void set_request_id(request_id_type id) { m_request_id = id; }
	inline void set_response(response_type* rsp) { m_response = rsp; }
	inline void set_path_data(pathpoint_chunk_type* chunk, uint32 count) { m_pathpoint_chunk = chunk; m_pathpoint_count = count; }
	inline void set_path_result(EPathResult result) { m_path_result = result; }
	inline void set_finished(bool finish) { m_finished = finish; }

	inline request_id_type get_request_id(void) const { return m_request_id; }
	inline response_type* get_response(void) const { return m_response; }
	inline pathpoint_chunk_type* get_pathpoint_chunk(void) const { return m_pathpoint_chunk; }
	inline uint32		get_pathpoint_count(void) const { return m_pathpoint_count; }
	inline EPathResult	get_path_reslut(void) const { return m_path_result; }
	inline bool			get_finished(void) const { return m_finished; }

	inline pathfinder_map*		get_path_map(void) const { return m_path_map; }
	inline pathfinder_finder*	get_path_finder(void) const { return m_path_finder; }
	inline dtQueryFilter*		get_path_filter(void) { return m_path_filter; }
	inline pathfinder_pathpoint_pool* get_path_pointpool_policy(void) const { return m_pool_policy; }
	inline float32*		get_start_point(void) { return m_start; }
	inline float32*		get_end_point(void) { return m_end; }
	inline float32		get_radius(void) { return m_radius; }

private:
	pathfinder_map*				m_path_map;
	pathfinder_finder*			m_path_finder;
	dtQueryFilter*				m_path_filter;
	pathfinder_pathpoint_pool*	m_pool_policy;

	float32			m_start[3];
	float32			m_end[3];
	float32			m_radius;

	request_id_type	m_request_id;
	response_type*	m_response;

	pathpoint_chunk_type* m_pathpoint_chunk;
	uint32			m_pathpoint_count;

	EPathResult		m_path_result;
	bool			m_finished;
};


__END_NAMESPACE


#endif //_PATHFINDER_PATH_H_