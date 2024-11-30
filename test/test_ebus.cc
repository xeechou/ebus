#include <catch2/catch_test_macros.hpp>
#include <ebus/ebus.hh>
#include <iostream>

template <EBUS_NS::ebus_type TYPE>
class sample_interface : public EBUS_NS::ebus_iface<TYPE>
{
public:
    virtual void event0(int)   = 0;
    virtual bool request0(int)        = 0;
    virtual void event1(std::string&) = 0;
};

template <EBUS_NS::ebus_type TYPE>
using sample_ebus = EBUS_NS::ebus<sample_interface<TYPE>>;

using sample_id_bus = EBUS_NS::ebus<sample_interface<EBUS_NS::ONE2ONE>>;

using sample_id_interface = sample_interface<EBUS_NS::ONE2ONE>;

static const size_t ID = 100;
//////////////////////////////////////////////////////////////////////////////////////
// ID
//////////////////////////////////////////////////////////////////////////////////////
class sample_id_ebus_handler
    : public EBUS_NS::ebus_handler<sample_interface<EBUS_NS::ONE2ONE>>
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
    virtual void event1(std::string& unused) override
    {
        std::cout << "ID: event1 called with " << unused << std::endl;
    }
};

bool
test_id()
{
    sample_id_ebus_handler handler;
    bool                   result = false;
    std::string            param  = "test";

    sample_id_bus::invoke(result, ID, &sample_id_interface::request0, 0);
    sample_id_bus::event(ID, &sample_id_interface::event0, 100);
    sample_id_bus::event(ID, &sample_id_interface::event1, param);

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////
// global
//////////////////////////////////////////////////////////////////////////////////////

using sample_type_bus = EBUS_NS::ebus<sample_interface<EBUS_NS::GLOBAL>>;

using sample_type_interface = sample_interface<EBUS_NS::GLOBAL>;

class sample_typed_ebus_handler : public EBUS_NS::ebus_handler<sample_type_interface>
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

    virtual void event1(std::string& unused) override
    {
        std::cout << "event 1 called with " << unused << std::endl;
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
    sample_type_bus::broadcast(&sample_type_interface::event0, 0);
    return result;
}

//////////////////////////////////////////////////////////////////////////////////////
// group
//////////////////////////////////////////////////////////////////////////////////////

using sample_group_bus       = EBUS_NS::ebus<sample_interface<EBUS_NS::GROUP>>;
using sample_group_interface = sample_interface<EBUS_NS::GROUP>;

class sample_group_ebus_handler : public EBUS_NS::ebus_handler<sample_group_interface>
{
public:
    sample_group_ebus_handler(size_t id) :
        m_id(id)
    {
        std::cout << "group bus connected for " << id << std::endl;
        connect(id);
    }
    ~sample_group_ebus_handler()
    {
        disconnect();
        std::cout << "group bus disconnected for " << m_id << std::endl;
    }

    virtual void event0(int unused) override
    {
        std::cout << "event 0 called with " << unused << " for group:" << m_id
                  << std::endl;
    }

    virtual void event1(std::string& unused) override
    {
        std::cout << "event 1 called with " << unused << " for group:" << m_id
                  << std::endl;
    }

    virtual bool request0(int unused) override
    {
        std::cout << "request called with" << unused << " for group: " << m_id
                  << std::endl;
        return true;
    }

private:
    size_t m_id;
};

bool
test_grouped()
{
    sample_group_ebus_handler handler0(0); // group 0
    sample_group_ebus_handler handler1(0); // group 0
    sample_group_ebus_handler handler2(1); // group 1

    bool        result = false;
    std::string test   = "test1";
    sample_group_bus::invoke(result, 0, &sample_group_interface::request0, 0);
    sample_group_bus::multicast(0, &sample_group_interface::event0, 0);
    sample_group_bus::multicast(1, &sample_group_interface::event1, test);

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////
// main
//////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("test ebus interface [EBUS]")
{
    REQUIRE(test_id() == true);
    REQUIRE(test_typed() == true);
    REQUIRE(test_grouped() == true);
}
