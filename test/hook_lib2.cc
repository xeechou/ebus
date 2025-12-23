#include "hook_lib.hh"
//#pragma optimize("", off)

namespace EBUS_NS
{

EBUS_HOOK_REGISTRY_DEF(test_hook_registry, lib2)
{
    test_hook_registry::instance().add_data(203);
    global_value += 100;
}
void
set_global_value(int i)
{
    global_value = i;
}

} // namespace EBUS_NS

