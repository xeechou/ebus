#pragma once

#include <ebus/ebus.hh>

#include "task.hh"

#include <memory>
#include <thread>

namespace EBUS_NS
{

struct rescheduable_task : task_base
{
    using ptr     = INTRUSIVE_NS::intrusive_ptr<rescheduable_task>;
    using fini_fn = std::function<void(void)>;

    /// API to support task rescheduling
    virtual ptr reschedule(task_base::exec_fn&& exec) = 0;
    // API to mark the task has no following task need to execute

    // TODO How do I get the output from previous task? Or we don't. The users simply
    // rely on the capture context for data sharing. I don't need type casting
    // in that case
    virtual void finish(fini_fn&& fini) = 0;

    fini_fn m_fini_task;
};

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
    void                   add_task(task_base::ptr task) override;
    rescheduable_task::ptr add_rescheduable_task(task_base::exec_fn&&) override;

    default_task_scheduler();
    ~default_task_scheduler();

private:
    std::vector<std::unique_ptr<task_worker>> m_workers;
    std::vector<std::thread>                  m_worker_threads;
};

} // namespace EBUS_NS
