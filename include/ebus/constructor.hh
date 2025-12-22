#pragma once

#include "export.hh"

#include <stdarg.h>

#ifndef EBUS_NS
#    define EBUS_NS _ebus_
#endif

#define EBUS_CONCAT_IMPL(x, y) x ## y
#define EBUS_CONCAT(x, y) EBUS_CONCAT_IMPL(x, y)

#if defined(__cplusplus)

// the most portable way of defining the constructor, there is guarantee when
// this will run before the main.
#    define EBUS_CONSTRUCTOR(_name)   \
        static void _name();          \
        struct _name##_t_             \
        {                             \
            _name##_t_() { _name(); } \
        };                            \
        static _name##_t_ _name##_;   \
        static void       _name()

// #elif defined(__GNUC__) || defined(__clang__)

// #    define EBUS_CONSTRUCTOR(_name) \
//         __attribute__((used, constructor)) static void _name()

// #elif defined(_MSC_VER)

// // declare special read-only sections
// #    pragma section(".ebusctor", read)
// #    pragma section(".ebusdtor", read)

// // see:
// // https://stackoverflow.com/questions/1113409/attribute-constructor-equivalent-in-vc
// // this special trick
// #    define EBUS_CONSTRUCTOR2_(f, p)                              \
//         static void f(void);                                      \
//         __declspec(allocate(".ebusctor")) void (*f##_)(void) = f; \
//         __pragma(comment(linker, "/include:" p #f "_")) static void _name()

// #    define EBUS_DES

// #    ifdef _WIN64
// #        define EBUS_CONSTRUCTOR(_name) EBUS_CONSTRUCTOR2_(_name, "")
// #    else // win32
// #        define EBUS_CONSTRUCTOR(_name) EBUS_CONSTRUCTOR2_(_name, "_")
// #    endif

#else

#    error "unsupported compiler or architecture"

#endif
