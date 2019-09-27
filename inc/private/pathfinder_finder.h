/*------------------------------------------------------------------------------
|
| COPYRIGHT (C) 2018 - 2026 All Right Reserved
|
| FILE NAME  : \pathfinder\inc\private\pathfinder_finder.h
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


#ifndef _PATHFINDER_FINDER_H_
#define _PATHFINDER_FINDER_H_


__BEGIN_NAMESPACE


class pathfinder_finder : public core_interface
{
public:
	virtual ~pathfinder_finder(void) {}
	virtual void path_finding(pathfinder_path* path) = 0;

protected:
	dtPolyRef	find_nearest_poly(dtNavMeshQuery* query, 
		dtQueryFilter* filter, 
		const float32* center, 
		float32 extent, 
		float32 height, 
		int32 extent_times = 6, 
		int32 height_times = 6);

	EPathResult find_path(dtNavMeshQuery* query, 
		dtQueryFilter* filter, 
		pathfinder_path* path, 
		dtPolyRef start_ref, 
		dtPolyRef end_ref, 
		pathfinder_pathpoint_pool::pathpoint_chunk** data, 
		uint32& data_len);
};


__END_NAMESPACE


#endif //_PATHFINDER_FINDER_H_