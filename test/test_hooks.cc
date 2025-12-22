#include "ebus/hooks.hh"

#include <catch2/catch_test_macros.hpp>
#include <vector>

namespace EBUS_NS
{
static int a = 0;

extern template class hook_registry<std::vector<int>>;

template class hook_registry<std::vector<int>>;

EBUS_HOOK_REGISTRY_FUNCTION(std::vector<int>)
{
    a += 100;
}

EBUS_HOOK_REGISTRY_FUNCTION(std::vector<int>)
{
    a += 100;
}

} // namespace EBUS_NS

#include <iostream>

TEST_CASE("test hooks [0]")
{
    EBUS_NS::hook_registry<std::vector<int>>::instance().run_hooks();
    std::cout << std::to_string(__COUNTER__) << std::endl;
    REQUIRE(EBUS_NS::a == 200);
}
