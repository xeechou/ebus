#include "hook_lib.hh"

namespace EBUS_NS
{
EBUS_HOOK_REGISTRY_FUNCTION(test_hook_registry)
{
    test_hook_registry::instance().add_data(200);
}

} // namespace EBUS_NS
