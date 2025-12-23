#include "ebus/hooks.hh"
#include "test_api.hh"
#include <atomic>

namespace EBUS_NS
{

class test_hook_registry : public hook_registry<test_hook_registry>
{
public:
    void add_data(int i) { data += i; }
    int  curr_data() const { return data; }

private:
    std::atomic_int data = 0;
};
EBUS_HOOK_REGISTRY_DECLARE(TEST, test_hook_registry);

} // namespace EBUS_NS
