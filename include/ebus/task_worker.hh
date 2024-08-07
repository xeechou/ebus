#pragma once

#include "task.hh"
#include "ebus/memory/intrusive_ptr.hh"

#include <atomic>
#include <semaphore>

namespace sp
{

/**
 * @class task_worker
 */
class task_worker
{
public:
    bool add_task(intrusive_ptr<task_base> task);
    void operator()();

    void shutdown();

protected:
    std::deque<intrusive_ptr<task_base>> m_tasks;
    std::binary_semaphore                m_sem{0};
    std::atomic_bool                     m_live = true;
};

} // namespace sp
