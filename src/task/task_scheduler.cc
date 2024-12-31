#include <ebus/task_scheduler.hh>
#include <ebus/task_worker.hh>

#include <algorithm>
#include <memory>
#include <thread>

namespace EBUS_NS
{
class simple_task : public rescheduable_task
{
public:
    simple_task(task_base::exec_fn&& func, simple_task* prev_task = nullptr);
    virtual ~simple_task();

    // intrusive_ptr overrides
    virtual void add_ref() override;
    virtual void release() override;

    // task API
    virtual ptr  reschedule(exec_fn&& func) override;
    virtual void finish(fini_fn&& func) override;
    virtual void task_done() override;

private:
    size_t m_refcount = 0;
    // next task is used to schedule
    ptr m_next_task = nullptr;
    // prev task. Used to trace back to the first added task.
    simple_task* m_prev_task = nullptr;
};

simple_task::simple_task(task_base::exec_fn&& func, simple_task* prev_task) :
    m_prev_task(prev_task)
{
    m_function = std::move(func);
}

simple_task::~simple_task() {}

void
simple_task::add_ref()
{
    ++m_refcount;
}

void
simple_task::release()
{
    if (--m_refcount <= 0)
        delete this;
}

// reschedule a task, this is not easy
rescheduable_task::ptr
simple_task::reschedule(task_base::exec_fn&& exec)
{
    if (m_next_task)
    {
        return nullptr;
    }
    m_next_task = rescheduable_task::ptr(new simple_task(std::move(exec), this));
    return m_next_task;
}

void
simple_task::finish(fini_fn&& fn)
{
    m_fini_task = fn;

    // It is the point to schedule task now
    simple_task* task = this;
    while (task->m_prev_task)
    {
        task = task->m_prev_task;
    }
    {
        task_base::ptr first_task(task); //+1
        task_scheduler_bus::broadcast(&task_scheduler_iface::m_add_task,
                                      std::ref(first_task));
        //-1
    }
}

void
simple_task::task_done()
{
    if (m_next_task)
    {
        task_base::ptr task(m_next_task); //+1
        // now it is a good to schedule the next task if available.  We are
        // safe to pass in rescheduable_task::ptr here since the
        // default_task_scheduler is implementing the task_schedule_bus (which
        // gives us the simple_task).
        task_scheduler_bus::broadcast(&task_scheduler_iface::m_add_task, std::ref(task));
        //-1
    }
    else if (m_fini_task)
    {
        m_fini_task();
    }
    // else, we need to reschedule
}

} // namespace EBUS_NS

namespace EBUS_NS
{

void
task_scheduler_iface::add_task(task_base::ptr task)
{
    task_scheduler_bus::broadcast(&task_scheduler_iface::m_add_task, std::ref(task));
}

rescheduable_task::ptr
task_scheduler_iface::add_rescheduable_task(const task_base::exec_fn& fn)
{
    rescheduable_task::ptr result;
    task_scheduler_bus::invoke(result,
                               &task_scheduler_iface::m_add_rescheduable_task,
                               std::ref(fn));
    return result;
}

} // namespace EBUS_NS

namespace EBUS_NS
{

default_task_scheduler::default_task_scheduler()
{
    handler_t::connect();
    // get number of workers, minimum is 2, or we have enough
    size_t nworkers = std::max((unsigned)2, std::thread::hardware_concurrency());
    // creating the number of threads to schedule for tasks
    for (size_t i = 0; i < nworkers; i++)
    {
        m_workers.emplace_back(std::unique_ptr<task_worker>(new task_worker));
        m_worker_threads.emplace_back(std::thread([this, i] { (*m_workers[i])(); }));
    }
}

default_task_scheduler::~default_task_scheduler()
{
    // exhaust the workers queues.
    for (size_t i = 0; i < m_workers.size(); i++)
    {
        m_workers[i]->shutdown();
    }

    // now it is good time to disconnect and join all the threads.
    handler_t::disconnect();
    for (size_t i = 0; i < m_worker_threads.size(); i++)
    {
        m_worker_threads[i].join();
    }
}

void
default_task_scheduler::m_add_task(task_base::ptr task)
{
    // find the worker with least amount of work
    task_worker* idle_worker = nullptr;
    for (auto& worker : m_workers)
    {
        // skip if not live.
        if (!worker->live())
            continue;
        if (!idle_worker || (worker->size() < idle_worker->size()))
            idle_worker = worker.get();
    }

    idle_worker->add_task(task);
}

rescheduable_task::ptr
default_task_scheduler::m_add_rescheduable_task(const task_base::exec_fn& fn)
{

    // this is not easy, because if your task can reschedule, it could be that
    // you are running into raise conditions, I suggest you call task_worker in
    // reschedule or done event.
    rescheduable_task::ptr new_task(new simple_task(fn));
    return new_task;
}

} // namespace EBUS_NS
