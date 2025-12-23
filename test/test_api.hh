#pragma once

#include "ebus/export.hh"

#if defined(TEST_EXPORT)
#    define TEST_API EBUS_EXPORT
#    define TEST_API_TEMPLATE_CLASS(...) EBUS_TEMPLATE_EXPORT(class, __VA_ARGS__)
#    define TEST_API_TEMPLATE_STRUCT(...) EBUS_TEMPLATE_EXPORT(struct, __VA_ARGS__)
#else
#    define TEST_API EBUS_IMPORT
#    define TEST_API_TEMPLATE_CLASS(...) EBUS_TEMPLATE_IMPORT(class, __VA_ARGS__)
#    define TEST_API_TEMPLATE_STRUCT(...) EBUS_TEMPLATE_IMPORT(struct, __VA_ARGS__)
#endif
