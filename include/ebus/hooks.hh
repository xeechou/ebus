#pragma once

#include "constructor.hh"

#include <vector>

namespace EBUS_NS
{

template <class TOKEN>
class hook_registry
{
public:
    using hook_t = void (*)();

    static hook_registry<TOKEN>& instance()
    {
        static hook_registry<TOKEN> s_instance = {};
        return s_instance;
    }

    void add_hook(hook_t hook) { m_hooks.push_back(hook); }

    void run_hooks()
    {
        for (auto hook : m_hooks)
        {
            hook();
        }
    }

private:
    std::vector<hook_t> m_hooks;
};

#define EBUS_HOOK_REGISTRY_DEF(TOKEN, NAME)                   \
    static void EBUS_CONCAT(_hooks_registry_, NAME)(void*);   \
    EBUS_CONSTRUCTOR(EBUS_CONCAT(_hooks_registry_add_, NAME)) \
    {                                                         \
        hook_registry<TOKEN>::instance().add_hook(            \
            (void (*)())EBUS_CONCAT(_hooks_registry_, NAME)); \
    }                                                         \
    static void EBUS_CONCAT(_hooks_registry_, NAME)(void*)

#define EBUS_HOOK_REGISTRY_FUNCTION(TOKEN) EBUS_HOOK_REGISTRY_DEF(TOKEN, __COUNTER__)

#define EBUS_HOOK_REGISTRY_DECLARE(EBUSAPI, IFACE) \
    EBUSAPI##_API_TEMPLATE_CLASS(hook_registry<IFACE>)

#define EBUS_HOOK_REGISTRY_DEFINE(IFACE) \
    EBUS_TEMPLATE_DEFINE(class, hook_registry<IFACE>)

} // namespace EBUS_NS
