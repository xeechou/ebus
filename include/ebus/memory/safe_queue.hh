#pragma once

#include <vector>
#include <functional>

namespace sp
{

///@brief the safe_queue allows you to visit the
template <typename T>
class safe_queue
{
public:
    safe_queue()  = default;
    ~safe_queue() = default;
    void emplace_back(T&& t) { m_pendings.emplace_back(t); }
    void execute(std::function<void(T&)> visitor)
    {
        while (!m_executions.empty() || !m_pendings.empty())
        {
            m_executions.swap(m_pendings);
            for (T& t : m_executions)
            {
                visitor(t);
            }
            m_executions.clear();
        }
    }

private:
    std::vector<T> m_pendings;
    std::vector<T> m_executions;
};

} // namespace sp
