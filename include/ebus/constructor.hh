#pragma once

#include "export.hh"

#include <stdarg.h>

#ifndef EBUS_NS
#    define EBUS_NS _ebus_
#endif

// NOTE: ## prevents macro expansion, if you use x ## __LINE__ directly,
//__LINE__ will not be expanded
#define EBUS_CONCAT_IMPL(x, y) x##y
#define EBUS_CONCAT(x, y) EBUS_CONCAT_IMPL(x, y)

#if defined(__cplusplus) && \
    (defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER))

// the most portable way of defining the constructor, there is guarantee when
// this will run before the main.
#    define EBUS_CONSTRUCTOR(_name)   \
        static void _name();          \
        struct EBUS_CONCAT(_name, _t_)          \
        {                             \
            EBUS_CONCAT(_name, _t_)() { _name(); }    \
        };                            \
        static EBUS_CONCAT(_name, _t_) EBUS_CONCAT(_name, _);   \
        static void       _name()

// for non cpp usage, special compiler extensions is required see:
// https://stackoverflow.com/questions/1113409/attribute-constructor-equivalent-in-vc
#else
#    error "unsupported compiler or architecture"
#endif
