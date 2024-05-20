#include <catch2/catch_test_macros.hpp>
#include <ebus/ebus.hh>
#include <iostream>

class sample_interface : public sp::ebus_iface<sp::ONE_TO_ONE>
{
public:
    virtual void event0(int)   = 0;
    virtual bool request0(int) = 0;
};

using sample_ebus = sp::ebus<sample_interface>;

class sample_ebus_handler : public sp::ebus_handler<sample_interface>
{
public:
    sample_ebus_handler()
    {
        std::cout << "bus connected" << std::endl;
        connect();
    }
    ~sample_ebus_handler()
    {
        disconnect();
        std::cout << "bus disconnected" << std::endl;
    }

    virtual void event0(int unused) override
    {
        std::cout << "event 0 called with " << unused << std::endl;
    }

    virtual bool request0(int unused) override
    {
        std::cout << "request called with" << unused << std::endl;
        return true;
    }
};

bool
test_compile()
{
    sample_ebus_handler handler;
    bool result = false;

    sample_ebus::broadcast(&sample_interface::event0, 0);
    sample_ebus::invoke(result, &sample_interface::request0, 0);
    return result;
}


TEST_CASE("test ebus interface [EBUS]")
{
    REQUIRE(test_compile() == true);
}
