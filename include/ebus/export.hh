#pragma once

// visibility controls
#if defined(_WIN32)

#    if defined(_MSC_VER)
#        define EBUS_EXPORT __declspec(dllexport)
#        define EBUS_IMPORT __declspec(dllimport)

#    elif defined(__GNUC__) || defined(__clang__)
#        define EBUS_EXPORT __attribute__((dllexport))
#        define EBUS_IMPORT __attribute__((dllimport))

#    else // unknown compiler
#        define EBUS_EXPORT
#        define EBUS_IMPORT
#    endif

#    define EBUS_HIDDEN
#    define EBUS_EXPORT_TYPE

/// NOTE on template export. By default templated class/struct are instantiated
/// IMMEDIATELY when specialized, so you could have multiple copies of the same
/// class in different DLLs. This would normally be fine but if you have a
/// singleton class it will be problematic. The 2 macros under will prevent the
/// compiler from generating the code, direct the compiler to find symbols
/// elsewhere

///  NOTE the warning: C4910, dllexport and extern template are
///  incompatible, since one direct the symbol is here the other says elsewhere.
#    define EBUS_TEMPLATE_EXPORT(type, ...)
#    define EBUS_TEMPLATE_IMPORT(type, ...) extern template type EBUS_IMPORT __VA_ARGS__

#elif defined(__GNUC__) || defined(__clang__) // linux/Mac
#    define EBUS_EXPORT __attribute__((visibility("default")))
#    define EBUS_IMPORT
#    define EBUS_HIDDEN __attribute__((visibility("hidden")))
#    define EBUS_EXPORT_TYPE __attribute__((visibility("default")))

#    define EBUS_TEMPLATE_EXPORT(type, ...)
#    define EBUS_TEMPLATE_IMPORT(type, ...) extern template type __VA_ARGS__

#endif

/// This macro explicitly instantiate the templated class
#define EBUS_TEMPLATE_DEFINE(type, ...) template type EBUS_EXPORT __VA_ARGS__
