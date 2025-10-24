#pragma once

#ifndef EBUS_NS
#    define EBUS_NS _ebus_
#endif

#include "internal/ebus.def.hh"
#include "internal/ebus.inl.hh"

#include "export.hh"

// NOTE: declaring/defining the EBUS interface: You don't have to do this if
// you special EBUS interface is used within the a single DLL or static
// libraries. But if you plan to use the bus across DLLs, declaring directives
// becomes necessary otherwise your handler becomes dll local.

#define EBUS_HANDLER_DECLARE_CLASS(EBUSAPI, IFACE) \
    EBUSAPI##_API_TEMPLATE_CLASS(EBUS_NS::singleton<EBUS_NS::ebus_handler<IFACE>::ctx>)

#define EBUS_HANDLER_DECLARE_STRUCT(EBUSAPI, IFACE) \
    EBUSAPI##_API_TEMPLATE_STRUCT(EBUS_NS::singleton<EBUS_NS::ebus_handler<IFACE>::ctx>)

// use this in cpp file
#define EBUS_HANDLER_DEFINE_CLASS(IFACE) \
    ARCH_TEMPLATE_DEFINE(class, EBUS_NS::singleton<EBUS_NS::ebus_handler<IFACE>::ctx>)

#define EBUS_HANDLER_DEFINE_STRUCT(IFACE) \
    ARCH_TEMPLATE_DEFINE(struct, EBUS_NS::singleton<EBUS_NS::ebus_handler<IFACE>::ctx>)
