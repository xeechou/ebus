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
        task_scheduler_bus::broadcast(&task_scheduler_iface::add_task,
                                      std::ref(first_task));
        //-1
    }
}

void
simple_task::task_done()
{
    // dispatch finish task now.
    if ((!m_next_task) && m_fini_task)
    {
        m_fini_task();
    }
    // else, we need to reschedule
    else if (m_next_task)
    {
        task_base::ptr task(m_next_task); //+1
        // now it is a good to schedule the next task if available.  We are
        // safe to pass in rescheduable_task::ptr here since the
        // default_task_scheduler is implementing the task_schedule_bus (which
        // gives us the simple_task).
        task_scheduler_bus::broadcast(&task_scheduler_iface::add_task, std::ref(task));
        //-1
    }
}

} // namespace EBUS_NS

namespace EBUS_NS
{

default_task_scheduler::default_task_scheduler()
{
    handler_t::connect();
    // get number of workers, minimum is 2, or we have enough
    size_t nworkers = std::max((unsigned)2, std::thread::hardware_concurrency());
    // task_worker is not movable...
    m_workers.resize(nworkers);

    // creating the number of threads to schedule for tasks
    for (size_t i = 0; i < nworkers; i++)
    {
        m_worker_threads.emplace_back(std::thread([this, i] { (*m_workers[i])(); }));
        m_worker_threads.back().detach();
    }
}

default_task_scheduler::~default_task_scheduler()
{
    handler_t::disconnect();
    for (auto& worker : m_workers)
    {
        worker->shutdown();
    }
}

void
default_task_scheduler::add_task(task_base::ptr task)
{
    // find the worker with least amount of work
    task_worker* idle_worker = nullptr;
    for (auto& worker : m_workers)
    {
        if (!idle_worker || worker->size() < idle_worker->size())
            idle_worker = worker.get();
    }
    // In this case we take a

    idle_worker->add_task(task);
}

rescheduable_task::ptr
default_task_scheduler::add_rescheduable_task(task_base::exec_fn&& fn)
{

    // this is not easy, because if your task can reschedule, it could be that
    // you are running into raise conditions, I suggest you call task_worker in
    // reschedule or done event.
    rescheduable_task::ptr new_task(new simple_task(std::move(fn)));
}

} // namespace EBUS_NS
