/*------------------------------------------------------------------------------
|
| COPYRIGHT (C) 2018 - 2026 All Right Reserved
|
| FILE NAME  : \pathfinder\inc\private\pathfinder_navmesh_finder.h
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


#ifndef _PATHFINDER_NAVMESH_FINDER_H_
#define _PATHFINDER_NAVMESH_FINDER_H_


__BEGIN_NAMESPACE


class pathfinder_navmesh_finder : public pathfinder_finder
{
public:
	pathfinder_navmesh_finder(void);
	virtual ~pathfinder_navmesh_finder(void);

	virtual void path_finding(pathfinder_path* path);

private:
	dtQueryFilter	m_filter;
};


__END_NAMESPACE


#endif //_PATHFINDER_NAVMESH_FINDER_H_