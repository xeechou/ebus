#include <catch2/catch_test_macros.hpp>
#include <ebus/ebus.hh>
#include <iostream>

template <sp::ebus_type TYPE>
class sample_interface : public sp::ebus_iface<TYPE>
{
public:
    virtual void event0(int)   = 0;
    virtual bool request0(int) = 0;
};

template <sp::ebus_type TYPE>
using sample_ebus = sp::ebus<sample_interface<TYPE>>;

using sample_id_bus = sp::ebus<sample_interface<sp::IDED>>;

using sample_id_interface = sample_interface<sp::IDED>;

static const size_t ID = 100;

class sample_id_ebus_handler : public sp::ebus_handler<sample_interface<sp::IDED>>
{
public:
    sample_id_ebus_handler()
    {
        std::cout << "ID bus connected" << std::endl;
        connect(ID);
    }
    ~sample_id_ebus_handler()
    {
        disconnect();
        std::cout << "ID bus disconnected" << std::endl;
    }

    virtual void event0(int unused) override
    {
        std::cout << "ID: event 0 called with " << unused << std::endl;
    }

    virtual bool request0(int unused) override
    {
        std::cout << "ID: request called with" << unused << std::endl;
        return true;
    }
};

bool
test_id()
{
    sample_id_ebus_handler handler;
    bool                   result = false;

    sample_id_bus::invoke(result, ID, &sample_id_interface::request0, 0);

    return result;
}

using sample_type_bus = sp::ebus<sample_interface<sp::TYPED>>;

using sample_type_interface = sample_interface<sp::TYPED>;

class sample_typed_ebus_handler : public sp::ebus_handler<sample_type_interface>
{
public:
    sample_typed_ebus_handler()
    {
        std::cout << "typed bus connected" << std::endl;
        connect();
    }
    ~sample_typed_ebus_handler()
    {
        disconnect();
        std::cout << "typed bus disconnected" << std::endl;
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
test_typed()
{
    sample_typed_ebus_handler handler;
    bool                      result = false;
    sample_type_bus::invoke(result, &sample_type_interface::request0, 0);
    return result;
}

TEST_CASE("test ebus interface [EBUS]")
{
    REQUIRE(test_id() == true);
    REQUIRE(test_typed() == true);
}
