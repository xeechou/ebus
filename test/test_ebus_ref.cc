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

class sample_interface : public sp::ebus_iface<sp::ebus_type::GLOBAL>
{
public:
    virtual void event0(const sample_class& ref) = 0;
    virtual void event1(sample_class&& ref)        = 0;
    virtual bool request0(const sample_class& ref) = 0;
};

typedef sp::ebus<sample_interface> sample_bus;

class sample_ebus_handler : public sp::ebus_handler<sample_interface>
{
public:
    sample_ebus_handler(int value) :
        m_value(value)
    {
        std::cout << "typed bus connected" << std::endl;
        connect();
    }
    ~sample_ebus_handler()
    {
        disconnect();
        std::cout << "typed bus disconnected" << std::endl;
    }

    virtual void event0(const sample_class& ref) override
    {
        std::cout << "event 0 called with " << ref.value() << std::endl;
    }

    virtual void event1(sample_class&& ref) override
    {
        std::cout << "event 1 called with " << ref.value() << std::endl;
    }

    virtual bool request0(const sample_class& ref) override
    {
        std::cout << "request called with" << ref.value() << std::endl;
        return m_value == ref.value();
    }

private:
    int m_value;
};

bool
test_typed()
{

    sample_class        value(10);
    sample_class        value1(11);
    sample_ebus_handler handler(value.value());
    sample_ebus_handler handler1(value1.value());
    bool                result = false;

    sample_bus::broadcast(&sample_interface::event0, std::ref(value));
    sample_bus::invoke(result, &sample_interface::request0, std::ref(value));

    return result == true;
}

TEST_CASE("test ebus interface [EBUS]") { REQUIRE(test_typed() == true); }
