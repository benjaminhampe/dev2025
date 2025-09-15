#include <de/ThreadPoolWithTasks.h>

namespace de {

ThreadPoolWithTasks::ThreadPoolWithTasks( uint_fast32_t maxThreads )
    : m_threadCount( maxThreads ? maxThreads : std::max(std::thread::hardware_concurrency() - 1, 1u) )
	, m_threadPool( new std::thread[ m_threadCount ] )
{
	createThreads();
}

ThreadPoolWithTasks::~ThreadPoolWithTasks()
{
	wait_for_tasks();
	m_isRunning = false;
	destroyThreads();
}

size_t 
ThreadPoolWithTasks::getQueuedTaskCount() const 
{ 
	LockGuard const lg(m_mutex); 
	return m_taskQueue.size(); 
}

uint_fast32_t 
ThreadPoolWithTasks::getTaskCount() const { return m_taskCount; }

uint_fast32_t 
ThreadPoolWithTasks::getRunningTaskCount() const 
{ 
	return m_taskCount - (uint_fast32_t)getQueuedTaskCount(); 
}

uint_fast32_t 
ThreadPoolWithTasks::getThreadCount() const { return m_threadCount; }

/*
template <typename Task>
void 
ThreadPoolWithTasks::push_task( Task const& task )
{
	m_taskCount++;
	{
		LockGuard const lg( m_mutex );
		m_taskQueue.push( std::function<void()>( task ) );
	}
}

template <typename Task, typename... Args>
void 
ThreadPoolWithTasks::push_task( Task const& task, Args const& ...args )
{
	push_task( [task, args...] { task(args...); } );
}
*/

void 
ThreadPoolWithTasks::reset( uint_fast32_t maxThreads )
{
	bool const wasPaused = m_isPaused;
	m_isPaused = true;
	wait_for_tasks();
	m_isRunning = false;
	destroyThreads();
    m_threadCount = maxThreads ? maxThreads : std::max(std::thread::hardware_concurrency() - 1, 1u);
	m_threadPool.reset( new std::thread[ m_threadCount ] );
	m_isPaused = wasPaused;
	createThreads();
	m_isRunning = true;
}

void 
ThreadPoolWithTasks::wait_for_tasks()
{
	while (true)
	{
		if (!m_isPaused)
		{
			if (m_taskCount < 1) { break; }
		}
		else
		{
			if (getRunningTaskCount() < 1) { break; }
		}
		sleep_or_yield();
	}
}

void 
ThreadPoolWithTasks::createThreads()
{
	for ( uint_fast32_t i = 0; i < m_threadCount; ++i )
	{
		m_threadPool[i] = std::thread( &ThreadPoolWithTasks::worker, this );
	}
}

void 
ThreadPoolWithTasks::destroyThreads()
{
	for ( uint_fast32_t i = 0; i < m_threadCount; ++i )
	{
		m_threadPool[i].join();
	}
}

bool 
ThreadPoolWithTasks::pop_task( std::function<void()> & task )
{
	LockGuard const lg( m_mutex );
	if ( m_taskQueue.empty() )
	{           
		return false;
	}
	else
	{
		task = std::move( m_taskQueue.front() );
		m_taskQueue.pop();
		return true;
	}
}

void 
ThreadPoolWithTasks::sleep_or_yield()
{
	if ( m_sleepDuration )
	{
		std::this_thread::sleep_for( std::chrono::microseconds( m_sleepDuration ) );
	}
	else
	{
		std::this_thread::yield();
	}
}

void 
ThreadPoolWithTasks::worker()
{
	while ( m_isRunning )
	{
		std::function<void()> task;
		if ( !m_isPaused && pop_task( task ) )
		{
			task();
			m_taskCount--;
		}
		else
		{
			sleep_or_yield();
		}
	}
}


} // end namespace de
