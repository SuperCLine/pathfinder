/*------------------------------------------------------------------------------
|
| COPYRIGHT (C) 2018 - 2026 All Right Reserved
|
| FILE NAME  : \pathfinder\inc\private\pathfinder_private.h
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


#ifndef _PATHFINDER_PRIVATE_H_
#define _PATHFINDER_PRIVATE_H_


//recast
#include <RecastAlloc.h>
#include <RecastAssert.h>
#include <Recast.h>
//detour
#include <DetourAlloc.h>
#include <DetourAssert.h>
#include <DetourCommon.h>
#include <DetourMath.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourNavMeshQuery.h>
#include <DetourNode.h>
#include <DetourStatus.h>
//detour crowd
#include <DetourCrowd.h>
#include <DetourLocalBoundary.h>
#include <DetourObstacleAvoidance.h>
#include <DetourPathCorridor.h>
#include <DetourPathQueue.h>
#include <DetourProximityGrid.h>
//detour tile cache
#include <DetourTileCache.h>
#include <DetourTileCacheBuilder.h>
//debug util
#include <DebugDraw.h>
#include <DetourDebugDraw.h>
#include <RecastDebugDraw.h>
#include <RecastDump.h>

#include "core_public.h"
#include "pathfinder_log.h"

#include "pathfinder_public.h"
#include "pathfinder_pathpoint_pool.h"
#include "pathfinder_navmesh.h"
#include "pathfinder_map.h"
#include "pathfinder_path.h"
#include "pathfinder_path_pool.h"
#include "pathfinder_finder.h"
#include "pathfinder_navmesh_finder.h"
#include "pathfinder_mgr.h"


#endif //_PATHFINDER_PRIVATE_H_