#include "export_lib.hh"

EBUS_HANDLER_DEFINE_CLASS(sample_interface);

static sample_interface_handler global_handler;

// without the explicit template instantiation. the ebus interface will be
// locked to dll only.
void
setup_handler_and_set_int(int value)
{
    EBUS_NS::ebus<sample_interface>::broadcast(&sample_interface::set_int, value);
}

std::type_index
get_type_index_sample_interface()
{
    return std::type_index(typeid(sample_interface));
}

std::type_info const&
get_type_info_sample_interface()
{
    return typeid(sample_interface);
}
