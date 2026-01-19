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

TEST_CASE("test type id")
{
    auto const& iface_type          = typeid(sample_interface);
    auto const& iface_type_exported = get_type_info_sample_interface();

    REQUIRE(iface_type == iface_type_exported);
}

TEST_CASE("test type index")
{
    auto iface_type_idx          = std::type_index(typeid(sample_interface));
    auto iface_type_idx_exported = get_type_index_sample_interface();
    REQUIRE(iface_type_idx == iface_type_idx);
}
