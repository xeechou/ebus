#include "ebus/task_worker.hh"
#include <atomic>

namespace EBUS_NS
{

void
task_worker::shutdown()
{
    m_live.store(false);
    // time to exhaust the queue, because the worker is no longer in live mode,
    // it is not possible add_task anymore. We are sure we can exhaust the
    // queue this time.
    while (m_tasks.size() > 0)
    {
        auto task = m_tasks.pop();
        if (task)
        {
            task->exec();
            task->task_done();
        }
    }
    // the special code to trick the task_worker thread to quit
    // waiting. Because it is possible the worker thread was waiting the empty queue
    m_tasks.push(INTRUSIVE_NS::intrusive_ptr<task_base>{});
}

void
task_worker::operator()()
{
    while (m_live)
    {
        auto task = m_tasks.pop();

        if (task) // if we come from shutdown, the task is empty here.
        {
            task->exec();
            task->task_done();
        }
    }
}

bool
task_worker::add_task(intrusive_ptr<task_base> task)
{
    if (!this->live())
    {
        return false;
    }

    m_tasks.push(task);
    return true;
}

bool
task_worker::live() const
{
    bool value = m_live.load();
    return value;
}

} // namespace EBUS_NS
