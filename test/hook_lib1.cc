#include "hook_lib.hh"

namespace EBUS_NS
{
EBUS_HOOK_REGISTRY_FUNCTION(test_hook_registry)
{
    test_hook_registry::instance().add_data(100);
}
EBUS_HOOK_REGISTRY_DEFINE(test_hook_registry);
} // namespace EBUS_NS
