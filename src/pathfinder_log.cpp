#include "stdafx.h"
#include "pathfinder_log.h"


__BEGIN_NAMESPACE

static interface_logmgr* g_pathfinder_logger = 0;
void set_pathfinder_logger(interface_logmgr* log)
{
	g_pathfinder_logger = log;
}

interface_logmgr* get_pathfinder_logger(void)
{
	return g_pathfinder_logger;
}

void pathfinder_log(ELogType type, const char* tag, const char* log)
{
	if (get_pathfinder_logger())
	{
		get_pathfinder_logger()->log(type, tag, log);
	}
}

__END_NAMESPACE
