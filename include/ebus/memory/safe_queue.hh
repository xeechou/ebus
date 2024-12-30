#pragma once

#include <deque>
#include <vector>
#include <functional>

#include <shared_mutex>
#include <condition_variable>

namespace EBUS_NS
{

///@brief provides thread safe access to adding and popping queue elements
template <typename T>
class safe_queue
{
public:
    safe_queue()  = default;
    ~safe_queue() = default;

    void push(T item)
    {
        // locks the access to the queue
        std::unique_lock<std::mutex> lock(m_access);

        m_queue.push_back(item);
        // notify any thread wants to consume the queue.
        m_cv.notify_one();
    }

    T pop()
    {
        // locks the access to the queue
        std::unique_lock<std::mutex> lock(m_access);

        // wait until queue is not empty
        m_cv.wait(lock, [this]() { return !m_queue.empty(); });

        // retrieving the item
        T item = m_queue.front();
        m_queue.pop_front();

        return item;
    }

    size_t size()
    {
        std::lock_guard<std::mutex> lock(m_access);
        return m_queue.size();
    }

protected:
    std::deque<T>           m_queue;
    std::mutex              m_access;
    std::condition_variable m_cv; // provides notifying mechanism as well.
};

} // namespace EBUS_NS
