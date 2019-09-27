/*------------------------------------------------------------------------------
|
| COPYRIGHT (C) 2018 - 2026 All Right Reserved
|
| FILE NAME  : \pathfinder\inc\private\pathfinder_path_pool.h
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


#ifndef _PATHFINDER_PATH_POOL_H_
#define _PATHFINDER_PATH_POOL_H_


__BEGIN_NAMESPACE

class pathfinder_path_pool : public core
{
public:
	pathfinder_path_pool(void);
	~pathfinder_path_pool(void);

	pathfinder_path* get(void);
	void cycle(pathfinder_path* p);

private:
	core_pool<pathfinder_path>	m_pool;
};


__END_NAMESPACE


#endif //_PATHFINDER_PATH_POOL_H_