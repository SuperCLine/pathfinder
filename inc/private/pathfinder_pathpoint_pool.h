/*------------------------------------------------------------------------------
|
| COPYRIGHT (C) 2018 - 2026 All Right Reserved
|
| FILE NAME  : \pathfinder\inc\private\pathfinder_pathpoint_pool.h
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


#ifndef _PATHFINDER_PATHPOINT_POOL_H_
#define _PATHFINDER_PATHPOINT_POOL_H_


__BEGIN_NAMESPACE


class pathfinder_pathpoint_pool : public core
{
public:
	struct pathpoint_chunk
	{
		virtual float32* get_chunk(void) = 0;
		virtual int32 get_capacity(void) = 0;
	};

public:
	pathfinder_pathpoint_pool(void);
	~pathfinder_pathpoint_pool(void);

	pathpoint_chunk* get(int32 size);
	void cycle(pathpoint_chunk* p);
	void update(float32 tick);

private:
	enum EPoolSize
	{
		EPS_128 = 128,
		EPS_256 = 256,
		EPS_512 = 512,
		EPS_1024 = 1024,
		EPS_2048 = 2048,
	};
	struct pathpoint_chunk_128 : public pathpoint_chunk
	{
		float32 data[EPS_128];
		virtual float32* get_chunk(void) { return data; }
		virtual int32 get_capacity(void) { return EPS_128; }
	};
	struct pathpoint_chunk_256 : public pathpoint_chunk
	{
		float32 data[EPS_256];
		virtual float32* get_chunk(void) { return data; }
		virtual int32 get_capacity(void) { return EPS_256; }
	};
	struct pathpoint_chunk_512 : public pathpoint_chunk
	{
		float32 data[EPS_512];
		virtual float32* get_chunk(void) { return data; }
		virtual int32 get_capacity(void) { return EPS_512; }
	};
	struct pathpoint_chunk_1024 : public pathpoint_chunk
	{
		float32 data[EPS_1024];
		virtual float32* get_chunk(void) { return data; }
		virtual int32 get_capacity(void) { return EPS_1024; }
	};
	struct pathpoint_chunk_2048 : public pathpoint_chunk
	{
		float32 data[EPS_2048];
		virtual float32* get_chunk(void) { return data; }
		virtual int32 get_capacity(void) { return EPS_2048; }
	};

private:
	core_pool<pathpoint_chunk_128>		m_pool128;
	core_pool<pathpoint_chunk_256>		m_pool256;
	core_pool<pathpoint_chunk_512>		m_pool512;
	core_pool<pathpoint_chunk_1024>		m_pool1024;
	core_pool<pathpoint_chunk_2048>		m_pool2048;

	core_pool_garbager<pathpoint_chunk_128>*	m_pool_garbager128;
	core_pool_garbager<pathpoint_chunk_256>*	m_pool_garbager256;
	core_pool_garbager<pathpoint_chunk_512>*	m_pool_garbager512;
	core_pool_garbager<pathpoint_chunk_1024>*	m_pool_garbager1024;
	core_pool_garbager<pathpoint_chunk_2048>*	m_pool_garbager2048;
};


__END_NAMESPACE


#endif //_PATHFINDER_PATHPOINT_POOL_H_