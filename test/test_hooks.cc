#include "hook_lib.hh"

#include <catch2/catch_test_macros.hpp>
#include <vector>

namespace EBUS_NS
{
} // namespace EBUS_NS

TEST_CASE("test hooks [0]")
{
    EBUS_NS::test_hook_registry::instance().run_hooks();
    REQUIRE(EBUS_NS::test_hook_registry::instance().curr_data() == 300);
}
