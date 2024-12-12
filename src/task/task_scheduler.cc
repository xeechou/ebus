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
    simple_task(task_base::exec_fn&& func);
    virtual ~simple_task();

    // intrusive_ptr overrides
    virtual void add_ref() override;
    virtual void release() override;

    // task API
    virtual ptr  reschedule(exec_fn&& func) override;
    virtual void task_done() override;

private:
    size_t m_refcount = 0;

    // supporting rescheduling, if next task is not none
    ptr m_next_task;
};

simple_task::simple_task(task_base::exec_fn&& func) { m_function = std::move(func); }

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

task_base::ptr
default_task_scheduler::add_task(task_base::exec_fn&& exec_func)
{

    // find the worker with least amount of work
    task_worker* idle_worker = nullptr;
    for (auto& worker : m_workers)
    {
        if (!idle_worker || worker->size() < idle_worker->size())
            idle_worker = worker.get();
    }

    // task_base::ptr new_task(new)

    // idle_worker->add_task(task_base::ptr(new));
}

} // namespace EBUS_NS
