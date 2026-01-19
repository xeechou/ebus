#pragma once

#include "constructor.hh"
#include "singleton.hh"

#include <vector>
#include <mutex>
#include <type_traits>

namespace EBUS_NS
{

/**
 * @brief hook_registry
 *
 * Thread-safe registry for managing initialization hooks using CRTP pattern.
 *
 * The hook_registry class provides a singleton-based mechanism for registering
 * and executing hooks (callback functions) at specific points during program
 * initialization or lifecycle.  It uses the Curiously Recurring Template
 * Pattern (CRTP) to allow type-safe specialization while maintaining a single
 * instance per subclass.
 *
 * Usage Example:
 * @code
 * struct MyHookRegistry : public hook_registry<MyHookRegistry> {};
 *
 * EBUS_HOOK_REGISTRY_FUNCTION(MyHookRegistry) {
 *     // Hook code executed during initialization
 * }
 * @endcode
 *
 */
template <class SUBCLASS>
class hook_registry
{
public:
    using hook_t     = void (*)();
    using subclass_t = SUBCLASS;

    static SUBCLASS& instance()
    {

        static_assert(std::is_base_of_v<hook_registry<SUBCLASS>, SUBCLASS>,
                      "invalid hook_registry");
        return singleton<subclass_t>::get_instance();
    }

    void add_hook(hook_t hook)
    {
        std::scoped_lock<std::mutex> lock(m_lock);
        m_hooks.push_back(hook);
    }

    void run_hooks()
    {
        std::scoped_lock<std::mutex> lock(m_lock);
        for (const auto& hook : m_hooks)
        {
            hook();
        }
        m_hooks.clear();
    }

private:
    std::mutex          m_lock;
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

#define EBUS_HOOK_REGISTRY_FUNCTION(REG, TAG) \
    EBUS_HOOK_REGISTRY_DEF(REG, EBUS_CONCAT(TAG, __COUNTER__))

#define EBUS_HOOK_REGISTRY_DECLARE(EBUSAPI, IFACE) \
    EBUSAPI##_API_TEMPLATE_CLASS(singleton<hook_registry<IFACE>::subclass_t>)

#define EBUS_HOOK_REGISTRY_DEFINE(IFACE) \
    EBUS_TEMPLATE_DEFINE(class, singleton<hook_registry<IFACE>::subclass_t>)

} // namespace EBUS_NS
