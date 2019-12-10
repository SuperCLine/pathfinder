#include "stdafx.h"
#include "pathfinder_pathpoint_pool.h"


__BEGIN_NAMESPACE


pathfinder_pathpoint_pool::pathfinder_pathpoint_pool(void)
	: m_pool_garbager128(new core_pool_garbager<pathpoint_chunk_128>(1024))
	, m_pool_garbager256(new core_pool_garbager<pathpoint_chunk_256>(512))
	, m_pool_garbager512(new core_pool_garbager<pathpoint_chunk_512>(256))
	, m_pool_garbager1024(new core_pool_garbager<pathpoint_chunk_1024>(64))
	, m_pool_garbager2048(new core_pool_garbager<pathpoint_chunk_2048>(16))
{
	m_pool128.set_tag("pathpoint_pool");
	m_pool256.set_tag("pathpoint_pool");
	m_pool512.set_tag("pathpoint_pool");
	m_pool1024.set_tag("pathpoint_pool");
	m_pool2048.set_tag("pathpoint_pool");
}

pathfinder_pathpoint_pool::~pathfinder_pathpoint_pool(void)
{
	util_safe_delete<core_pool_garbager<pathpoint_chunk_128>>(m_pool_garbager128);
	util_safe_delete<core_pool_garbager<pathpoint_chunk_256>>(m_pool_garbager256);
	util_safe_delete<core_pool_garbager<pathpoint_chunk_512>>(m_pool_garbager512);
	util_safe_delete<core_pool_garbager<pathpoint_chunk_1024>>(m_pool_garbager1024);
	util_safe_delete<core_pool_garbager<pathpoint_chunk_2048>>(m_pool_garbager2048);
}

pathfinder_pathpoint_pool::pathpoint_chunk* pathfinder_pathpoint_pool::get(int32 size)
{
	if (size <= EPS_128)
	{
		return m_pool128.get();
	}
	else if (size <= EPS_256)
	{
		return m_pool256.get();
	}
	else if (size <= EPS_512)
	{
		return m_pool512.get();
	}
	else if (size <= EPS_1024)
	{
		return m_pool1024.get();
	}
	else if (size <= EPS_2048)
	{
		return m_pool2048.get();
	}
	else
	{
		app_assert(!"path point is too large!!!");
		pathfinder_log(ELT_ERROR, "pathfinder", "path point is too large!!!");
		return m_pool2048.get();
	}
}

void pathfinder_pathpoint_pool::cycle(pathpoint_chunk* p)
{
	int32 size = p->get_capacity();
	switch ((EPoolSize)size)
	{
	case EPS_128:
		m_pool128.cycle(app_cast_dynamic(pathpoint_chunk_128*, p));
		break;
	case EPS_256:
		m_pool256.cycle(app_cast_dynamic(pathpoint_chunk_256*, p));
		break;
	case EPS_512:
		m_pool512.cycle(app_cast_dynamic(pathpoint_chunk_512*, p));
		break;
	case EPS_1024:
		m_pool1024.cycle(app_cast_dynamic(pathpoint_chunk_1024*, p));
		break;
	case EPS_2048:
		m_pool2048.cycle(app_cast_dynamic(pathpoint_chunk_2048*, p));
		break;
	}
}

void pathfinder_pathpoint_pool::update(float32 tick)
{
	m_pool128.update(tick, m_pool_garbager128);
	m_pool256.update(tick, m_pool_garbager256);
	m_pool512.update(tick, m_pool_garbager512);
	m_pool1024.update(tick, m_pool_garbager1024);
	m_pool2048.update(tick, m_pool_garbager2048);
}

__END_NAMESPACE
