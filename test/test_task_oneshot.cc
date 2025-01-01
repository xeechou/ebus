#include "util.hh"
#include <ebus/task_scheduler.hh>
#include <catch2/catch_test_macros.hpp>

#include <string>
#include <thread>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////////////
// oneshot task
//////////////////////////////////////////////////////////////////////////////////////
namespace EBUS_NS
{

class oneshot_task final : public task_base
{
public:
    oneshot_task(int i) :
        task_base(
            [i]()
            {
                std::stringstream output;
                output << "oneshot_task " << i << " executing..." << std::endl;
                test::sync_output(output);
                std::this_thread::sleep_for(std::chrono::seconds(1));
                return true;
            }),
        m_i(i)
    {
    }

    virtual ~oneshot_task() {}

    /// overrides
    virtual void task_done() override
    {
        std::stringstream output;
        output << "oneshot_task " << m_i << " done" << std::endl;
        test::sync_output(output);
    }

    virtual void add_ref() override { ++m_refcount; }

    virtual void release() override
    {
        if (--m_refcount <= 0)
            delete this;
    }

private:
    const int m_i;
    int       m_refcount = 0;
};

bool
test_oneshot()
{
    // should be hooked to it.
    {
        default_task_scheduler scheduler;
        std::cout << "task scheduler started" << std::endl;
        for (unsigned i = 0; i < 10; i++)
        {
            task_base::ptr oneshot(new oneshot_task(i));
            task_scheduler_iface::add_task(oneshot);
        }
    }
    std::cout << "task scheduler ended" << std::endl;

    return true;
}

} // namespace EBUS_NS

//////////////////////////////////////////////////////////////////////////////////////
// main
//////////////////////////////////////////////////////////////////////////////////////
TEST_CASE("test task scheduler [TASK]") { REQUIRE(EBUS_NS::test_oneshot() == true); }
