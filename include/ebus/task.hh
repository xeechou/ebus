#pragma once

#include <deque>
#include <functional>

#include <ebus/memory/intrusive_ptr.hh>

#include <ebus/ebus.hh>

namespace EBUS_NS
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
    using ptr     = INTRUSIVE_NS::intrusive_ptr<task_base>;
    using exec_fn = std::function<bool(void)>;

    task_base(const exec_fn& fn = []() { return true; }) :
        m_function(fn)
    {
    }
    virtual ~task_base() {}

    bool exec() { return m_function(); }

    // this should defines a done event for the task, the subclass
    virtual void task_done() = 0;

    //////////////////////////////////////////////////////////////////////////
    // for intrusive_ptr support
    virtual void add_ref() = 0; // a default impl is ++ref_count
    virtual void release() = 0; // a default impl is delete if <= 0
    //////////////////////////////////////////////////////////////////////////

    exec_fn m_function; // return true if success.
};

} // namespace EBUS_NS
