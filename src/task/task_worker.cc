#include "ebus/task_worker.hh"
#include <atomic>

namespace EBUS_NS
{

void
task_worker::shutdown()
{
    m_live = false;
    // special function to trigger the worker to wake.
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
    m_tasks.push(task);
    return true;
}

} // namespace EBUS_NS
