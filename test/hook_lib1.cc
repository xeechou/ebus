#include "hook_lib.hh"

namespace EBUS_NS
{

EBUS_HOOK_REGISTRY_FUNCTION(test_hook_registry)
{
    global_value = 0;
    test_hook_registry::instance().add_data(101);
}

EBUS_HOOK_REGISTRY_DEFINE(test_hook_registry);
} // namespace EBUS_NS
