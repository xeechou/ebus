#include "hook_lib.hh"

#include <catch2/catch_test_macros.hpp>
#include <vector>

namespace EBUS_NS
{
EBUS_HOOK_REGISTRY_FUNCTION(test_hook_registry, test_hook)
{
    test_hook_registry::instance().add_data(101);
}

// defined in hook_lib2
extern void set_global_value(int i);
} // namespace EBUS_NS

TEST_CASE("test hooks [0]")
{
    // NOTE: hook_lib1 is linked because test_hook_registry is indeed defined in it.

    // NOTE: set_global_value ensure hook_lib2 does get linked here, otherwise
    // linker may decide that hook_lib2 not need to be linked at all.
    EBUS_NS::set_global_value(202);
    EBUS_NS::test_hook_registry::instance().run_hooks();
    REQUIRE(EBUS_NS::test_hook_registry::instance().curr_data() == (202 +203));
}
