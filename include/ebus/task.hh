#pragma once

#include <deque>
#include <functional>

#include <ebus/ebus.hh>

namespace sp
{

/**
 * @class task
 *
 * A general task to throw onto @ref task_worker for executing. Typically user
 * will need to implement @ref m_function for the execution of the task. A @ref
 * task_done event is generated when the @ref task_worker finishes executing
 * the task.
 *
 *
 * @section memory management
 *
 * Users are required to implement it's own memory
 * management for task since a task is an @ref intrusive_ptr in @ref
 * task_worker. This can be a good thing because often you do not need to
 * implement any memory management at all.
 */
struct task_base
{
    bool exec() { return m_function(); }

    // this should defines a done event for the task, the subclass t
    virtual void task_done() = 0;

    //////////////////////////////////////////////////////////////////////////
    // for intrusive_ptr support
    virtual void add_ref() = 0; // a default impl is ++ref_count
    virtual void release() = 0; // a default impl is delete if <= 0
    //////////////////////////////////////////////////////////////////////////

    std::function<bool(void)> m_function; // return true if success.
};

template <ebus_type iface_type>
struct task : public task_base, ebus_iface<iface_type>
{
};

} // namespace sp
