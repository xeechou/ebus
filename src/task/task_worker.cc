#include "ebus/task_worker.hh"
#include <atomic>

namespace sp
{

void
task_worker::shutdown()
{
    m_live = false;
    // special function to trigger the worker to wake.
    m_sem.release();
}

void
task_worker::operator()()
{
    while (m_live)
    {
        // coming from add_task() or shutdown()
        m_sem.acquire();

        if (!m_tasks.empty())
        {
            // pop the task out
            auto task = m_tasks.front();
            m_tasks.pop_front();

            task->exec();
            task->task_done();
        }
    }
}

bool
task_worker::add_task(intrusive_ptr<task_base> task)
{
    m_tasks.push_back(task);
    m_sem.release();
    return true;
}

} // namespace sp
