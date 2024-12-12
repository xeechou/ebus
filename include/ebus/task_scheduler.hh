#pragma once

#include <ebus/ebus.hh>

#include "task.hh"


namespace EBUS_NS
{

/**
 * @class task_scheduler
 *
 *
 * A global interface for scheduling or rescheduling tasks
 */
struct task_scheduler_iface : public ebus_iface<ebus_type::GLOBAL>
{
    /// scheduling a new task. The implementation should load balance the task_worker
    /// it owns.
    virtual task_base::ptr add_task(task_base::exec_fn&&) = 0;
};

using task_scheduler_bus = ebus<task_scheduler_iface>;

} // namespace EBUS_NS
