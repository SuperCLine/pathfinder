/*------------------------------------------------------------------------------
|
| COPYRIGHT (C) 2018 - 2026 All Right Reserved
|
| FILE NAME  : \pathfinder\inc\private\pathfinder_log.h
| AUTHOR     : CLine
| PURPOSE    :
|
| SPEC       :
|
| MODIFICATION HISTORY
|
| Ver      Date            By              Details
| -----    -----------    -------------   ----------------------
| 1.0      2019-9-27      CLine           Created
|
+-----------------------------------------------------------------------------*/


#ifndef _PATHFINDER_LOG_H_
#define _PATHFINDER_LOG_H_


__BEGIN_NAMESPACE


void set_pathfinder_logger(interface_logmgr* log);
interface_logmgr* get_pathfinder_logger(void);

void pathfinder_log(ELogType type, const char* tag, const char* log);

template<typename... Args>
void pathfinder_logf(ELogType type, const char* tag, const char* format, Args... args)
{
	if (get_pathfinder_logger())
	{
		get_pathfinder_logger()->logf(type, tag, format, args...);
	}
}


__END_NAMESPACE


#endif //_PATHFINDER_LOG_H_