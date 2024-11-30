#include <catch2/catch_test_macros.hpp>
#include "ebus/event.hh"

typedef EBUS_NS::event<>   null_event;
static int                 counter = 0;
static null_event          null_event0;
static null_event::handler handler([]() { counter++; }, &null_event0);

bool
test_compile()
{
    null_event0.dispatch();
    return (counter == 1);
}

//////////////////////////////////////////////////////////////////////////////////////
// main
//////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("test event interface [EBUS]") { REQUIRE(test_compile() == true); }
