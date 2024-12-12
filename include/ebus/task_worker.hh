#pragma once

#include "task.hh"
#include "ebus/memory/intrusive_ptr.hh"

#include <atomic>
#include <semaphore>

namespace EBUS_NS
{

/**
 * @class task_worker
 */
class task_worker
{
public:
    bool add_task(task_base::ptr task);
    void operator()();

    void shutdown();

protected:
    std::deque<task_base::ptr> m_tasks;
    std::binary_semaphore      m_sem{0};
    std::atomic_bool           m_live = true;
};

} // namespace EBUS_NS
