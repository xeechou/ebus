#include <catch2/catch_test_macros.hpp>
#include <ebus/ebus.hh>
#include <iostream>

class sample_class {
public:
    sample_class(int val) :
        m_value(val)
    {
    }

    // purposely makes it not trivial
    sample_class(sample_class& other)  = delete;
    sample_class(sample_class&& other) = delete;

    int value() const { return m_value; }

private:
    int m_value;
};

class sample_interface : public EBUS_NS::ebus_iface<EBUS_NS::ebus_type::GLOBAL>
{
public:
    virtual void event0(const sample_class& ref) = 0;
    virtual void event1(sample_class&& ref)        = 0;
    virtual int  request0(const sample_class& ref) = 0;
};

typedef EBUS_NS::ebus<sample_interface> sample_bus;

class sample_ebus_handler : public EBUS_NS::ebus_handler<sample_interface>
{
public:
    sample_ebus_handler(int value) :
        m_value(value)
    {
        std::cout << "typed bus connected" << std::endl;
        connect(EBUS_NS::ebus_priority_t((float)m_value));
    }
    ~sample_ebus_handler()
    {
        disconnect();
        std::cout << "typed bus disconnected" << std::endl;
    }

    virtual void event0(const sample_class& ref) override
    {
        std::cout << "event 0 called with this value: " << m_value << std::endl;
    }

    virtual void event1(sample_class&& ref) override
    {
        std::cout << "event 1 called with this value: " << m_value << std::endl;
    }

    virtual int request0(const sample_class& ref) override
    {
        std::cout << "request called with this value: " << m_value << std::endl;
        return m_value;
    }

private:
    int m_value;
};

bool
test_typed()
{

    sample_class        value(10);
    sample_class        value1(11);
    sample_class        value2(9);
    sample_ebus_handler handler(value.value());
    sample_ebus_handler handler1(value1.value());
    sample_ebus_handler handler2(value2.value());
    int                 result = 0;

    sample_bus::broadcast(&sample_interface::event0, std::ref(value));
    sample_bus::invoke(result, &sample_interface::request0, std::ref(value));

    return result == 11;
}

TEST_CASE("test ebus interface [EBUS]") { REQUIRE(test_typed() == true); }
