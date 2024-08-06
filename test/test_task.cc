#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <task/task_worker.hh>
#include <iostream>
#include <thread>

class test_task : public sp::task<sp::GLOBAL>
{
public:
    test_task(int id) :
        m_id(id)
    {
        init_function();
    }

    test_task(const test_task& another) :
        m_id(another.m_id),
        m_refcount(another.m_refcount) // this is really a hack
    {
        init_function();
    };

    // non memory management
    virtual void add_ref() override
    {
        ++m_refcount;
        // std::cout << "adding ref for " << m_id << ", the ref is: " << m_refcount
        //           << std::endl;
    };

    virtual void release() override
    {
        // std::cout << "decreasing ref for " << m_id << ", the ref is: " << m_refcount
        //           << std::endl;
        if (--m_refcount <= 0)
        {
            std::cout << "at end of life for " << m_id << std::endl;
        }
    };

    virtual void task_done() override
    {
        std::cout << "the task " << m_id << " is done" << std::endl;
    }

private:
    void init_function()
    {
        m_function = [this]() -> bool
        {
            std::cout << "processing task: " << m_id << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return true;
        };
    }
    size_t       m_refcount = 0;
    const size_t m_id;
};

bool
test_task_worker()
{
    sp::task_worker worker;

    std::thread worker_thread([&worker]() { worker(); });
    worker_thread.detach();

    std::cout << "thread is " << (worker_thread.joinable() ? "" : " not") << " joinable"
              << std::endl;

    for (int i = 0; i < 10; i++)
    {
        worker.add_task(sp::intrusive_ptr<test_task>(new test_task(i)));
        std::cout << "adding task " << i << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));
    worker.shutdown();

    // you need to either detach() or join(). Cannot do both
    // worker_thread.join();
    return true;
}

TEST_CASE("test task interface [TASK_WORKER]") { REQUIRE(test_task_worker() == true); }
