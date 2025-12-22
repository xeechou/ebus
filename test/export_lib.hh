#include "ebus/ebus.hh"

#if defined(USE_DLL)
#    define EXP_API EBUS_EXPORT
#    define EXP_API_TEMPLATE_CLASS(...) EBUS_TEMPLATE_EXPORT(class, __VA_ARGS__)
#    define EXP_API_TEMPLATE_STRUCT(...) EBUS_TEMPLATE_EXPORT(struct, __VA_ARGS__)
#else
#    define EXP_API EBUS_IMPORT
#    define EXP_API_TEMPLATE_CLASS(...) EBUS_TEMPLATE_IMPORT(class, __VA_ARGS__)
#    define EXP_API_TEMPLATE_STRUCT(...) EBUS_TEMPLATE_IMPORT(struct, __VA_ARGS__)
#endif

#include <iostream>

class sample_interface : public EBUS_NS::ebus_iface<EBUS_NS::GLOBAL>
{
public:
    virtual void set_int(int) = 0;
    virtual int  get_int()    = 0;

protected:
    int data = {};
};

class sample_interface_handler : public EBUS_NS::ebus_handler<sample_interface>
{
    void set_int(int i) override
    {
        std::cout << "Global event: " << i << std::endl;
        data = i;
    }

    int get_int() override { return data; }

public:
    sample_interface_handler() { EBUS_NS::ebus_handler<sample_interface>::connect(); }
    ~sample_interface_handler()
    {
        EBUS_NS::ebus_handler<sample_interface>::disconnect();
    }
};

// this class will instantiate a local handler inside the dll and set the data.
EXP_API void setup_handler_and_set_int(int value);
// without the extern template below, the static interface should be
EBUS_HANDLER_DECLARE_CLASS(EXP, sample_interface);
