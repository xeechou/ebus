#pragma once

#include <ebus/ebus.hh>

#include "task.hh"

#include <memory>
#include <thread>

namespace EBUS_NS
{

class task_worker;

/**
 * @class task_scheduler
 *
 *
 * A global interface for scheduling or rescheduling tasks
 */
struct task_scheduler_iface : public ebus_iface<ebus_type::GLOBAL>
{
    /// @brief adding a single task
    ///
    /// user is responsible to prepare the task's implementation. The task is
    /// expected to be scheduled immediately
    virtual void add_task(task_base::ptr task) = 0;

    /// @brief Adding a reschedule-able task.
    ///
    /// The implementation should wait for rescheduable_task::done() to
    /// actually adding the task to the system, for the thread safety.
    virtual rescheduable_task::ptr add_rescheduable_task(task_base::exec_fn&&) = 0;
};

using task_scheduler_bus = ebus<task_scheduler_iface>;

/**
 * @class default_task_scheduler
 *
 * default task_scheduler implementation
 */
class default_task_scheduler : public ebus_handler<task_scheduler_iface>
{
    using handler_t = ebus_handler<task_scheduler_iface>;

public:
    task_base::ptr add_task(task_base::exec_fn&& exec_func) override;

    default_task_scheduler();
    ~default_task_scheduler();

private:
    std::vector<std::unique_ptr<task_worker>> m_workers;
    std::vector<std::thread>                  m_worker_threads;
};

} // namespace EBUS_NS
