#include "stdafx.h"
#include "pathfinder_path_pool.h"


__BEGIN_NAMESPACE


pathfinder_path_pool::pathfinder_path_pool(void)
{
	m_pool.set_tag("path_pool");
}

pathfinder_path_pool::~pathfinder_path_pool(void)
{

}

pathfinder_path* pathfinder_path_pool::get(void)
{
	return m_pool.get();
}

void pathfinder_path_pool::cycle(pathfinder_path* p)
{
	p->destroy();
	m_pool.cycle(p);
}

__END_NAMESPACE
