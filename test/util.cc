#include "util.hh"

#include <thread>
#include <mutex>
#include <iostream>


namespace test
{
void sync_output(const std::stringstream& output)
{
    static std::mutex stream_lock;

    const std::lock_guard<std::mutex> lock(stream_lock);
    std::cout << output.str() << std::endl;
}
}
