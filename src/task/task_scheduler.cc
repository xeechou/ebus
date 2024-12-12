#include <ebus/task_scheduler.hh>
#include <ebus/task_worker.hh>

#include <algorithm>
#include <memory>
#include <thread>

namespace EBUS_NS
{
class default_task_scheduler : public ebus_handler<task_scheduler_iface>
{
    using handler_t = ebus_handler<task_scheduler_iface>;

public:
    task_base::ptr add_task(task_base::exec_fn&& exec_func) override;

    default_task_scheduler();
    ~default_task_scheduler();

private:
    std::vector<std::unique_ptr<task_worker>> m_workers;
};

default_task_scheduler::default_task_scheduler()
{
    handler_t::connect();
    // get number of workers, minimum is 2, or we have enough
    size_t nworkers = std::max((unsigned)2, std::thread::hardware_concurrency());
    // task_worker is not movable...
    m_workers.resize(nworkers);
}

default_task_scheduler::~default_task_scheduler() { handler_t::disconnect(); }

task_base::ptr
default_task_scheduler::add_task(task_base::exec_fn&& exec_func)
{
}

} // namespace EBUS_NS
