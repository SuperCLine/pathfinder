/*------------------------------------------------------------------------------
|
| COPYRIGHT (C) 2018 - 2026 All Right Reserved
|
| FILE NAME  : \pathfinder\inc\private\pathfinder_map.h
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


#ifndef _PATHFINDER_MAP_H_
#define _PATHFINDER_MAP_H_


__BEGIN_NAMESPACE


class pathfinder_map : public core
{
public:
	pathfinder_map(void);
	~pathfinder_map(void);

	void add_navmesh(pathfinder_navmesh* mesh);

	// find navigation mesh according to the radius of agent
	pathfinder_navmesh* find_navmesh(float32 radius);

	bool has_map(void);

private:
	ulist<pathfinder_navmesh*>	m_navmesh_list;
};


__END_NAMESPACE


#endif //_PATHFINDER_MAP_H_