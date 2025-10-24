#pragma once
#include "export.hh"

namespace EBUS_NS
{

template <typename T>
class singleton
{
public:
    static T& get_instance()
    {
        // as simple as possible, the instance itself should be protected.
        static T s_instance = {};
        return s_instance;
    }

protected:
    singleton()  = default;
    ~singleton() = default;

    singleton(const singleton&) = delete;
    singleton& operator=(const singleton&) = delete;
};

} // namespace EBUS_NS
