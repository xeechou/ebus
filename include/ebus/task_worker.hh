#pragma once

#include "task.hh"
#include "ebus/memory/safe_queue.hh"

#include <atomic>

namespace EBUS_NS
{

/**
 * @class task_worker
 */
class task_worker
{
public:
    bool   live() const;
    bool   add_task(task_base::ptr task);
    size_t size() { return m_tasks.size(); }
    void   operator()();

    // method called from main thread
    void shutdown();

protected:
    safe_queue<task_base::ptr> m_tasks;
    std::atomic_bool           m_live = true;
};

} // namespace EBUS_NS
