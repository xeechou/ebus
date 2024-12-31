#include "util.hh"
#include <ebus/task_scheduler.hh>
#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <sstream>
#include <thread>

namespace EBUS_NS
{

// simple task
bool
task_execute(int task_id, int task_step, int final_step = -1)
{
    std::stringstream ss;
    ss << "executing task " << task_id;

    if (task_step == final_step)
    {
        ss << " finished";
    }
    else
    {
        ss << " step " << task_step << "...";
    }

    ss << std::endl;
    test::sync_output(ss);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    return true;
}

bool
test_reschedule()
{
    {
        default_task_scheduler scheduler;
        std::cout << "task scheduler started" << std::endl;
        for (int i = 0; i < 10; i++)
        {
            task_scheduler_iface::add_rescheduable_task([i]() -> bool
                                                        { return task_execute(i, 0); })
                ->reschedule([i]() -> bool { return task_execute(i, 1); })
                ->finish([i]() { task_execute(i, 2, 2); });
        }

        // std::this_thread::sleep_for(std::chrono::seconds(10));
        // once this goes out of scope, the default_task_scheduler will
        // un-listen immediately
    }
    std::cout << "task scheduler ended" << std::endl;

    return true;
}

} // namespace EBUS_NS

TEST_CASE("test task scheduler [TASK]") { REQUIRE(EBUS_NS::test_reschedule() == true); }
