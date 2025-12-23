#include "ebus/hooks.hh"

#include <catch2/catch_test_macros.hpp>
#include <vector>

namespace EBUS_NS
{
static int a = 0;

class test_hook_registry : public hook_registry<test_hook_registry>
{
};

// extern template class hook_registry<std::vector<int>>;

// template class hook_registry<std::vector<int>>;

EBUS_HOOK_REGISTRY_FUNCTION(test_hook_registry) { a += 100; }

EBUS_HOOK_REGISTRY_FUNCTION(test_hook_registry) { a += 100; }

} // namespace EBUS_NS

TEST_CASE("test hooks [0]")
{
    EBUS_NS::test_hook_registry::instance().run_hooks();
    REQUIRE(EBUS_NS::a == 200);
}
