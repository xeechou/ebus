#include <catch2/catch_test_macros.hpp>
#include "export_lib.hh"

TEST_CASE("test ebus export dll [EBUS]")
{
    int value = 0;

    // without the extern template, the handler will be registered only in dll.
    setup_handler_and_set_int(101);

    EBUS_NS::ebus<sample_interface>::invoke(value, &sample_interface::get_int);

    REQUIRE(value == 101);
}
