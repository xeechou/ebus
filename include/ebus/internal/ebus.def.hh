#pragma once

#ifndef EBUS_NS
#define EBUS_NS _ebus_
#endif

#ifndef INTRUSIVE_NS
#    define INTRUSIVE_NS EBUS_NS
#endif
#include "../memory/intrusive_list.hh"
#include "../singleton.hh"

#include <cstddef>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <mutex>

// here we define a concept that type T need to has a function
template <typename T, typename function_t, typename... args_t>
concept has_function = requires(T t, function_t&& func, args_t&&... args) {
    { t.func(args...) };
};

#if defined(_MSC_VER)
typedef long long ssize_t;
#endif

namespace EBUS_NS
{

enum ebus_type
{
    GLOBAL  = 0, // the GLOBAL ebus has all listeners respond to the same event
    ONE2ONE = 1, // the ONE2ONE ebus will have a id for each listener.
    GROUP   = 2, // the GROUP ebus will have listeners grouped by id.
};

template <ebus_type iface_type>
struct ebus_iface
{
    static inline constexpr ebus_type type = iface_type;
};

template <class iface, ebus_type iface_type = iface::type>
concept EBUS_IFACE = requires { std::is_base_of_v<ebus_iface<iface_type>, iface>; };

/**
 * ebus the event bus interface
 *
 * This ebus interface is an simplified o3de's EBus implementation. We limits
 * the bus to be either ONE_TO_ONE or ONE_TO_MUL
 *
 */
template <EBUS_IFACE interface>
class ebus;

template <EBUS_IFACE interface>
class ebus_handler;

template <EBUS_IFACE interface>
class ebus
{
public:
    using handler_t = ebus_handler<interface>;

    // 1-to-1 ebus event
    template <typename function_t, typename... args_t>
    static void event(size_t id, function_t&& func, args_t&&... args);

    template <typename function_t, typename... args_t>
    static void multicast(size_t id, function_t&& func, args_t&&... args);

    // broadcast an event to multi-handlers
    template <typename function_t, typename... args_t>
    static void broadcast(function_t&& func, args_t&&... args);

    // invoke non-ided handler function
    template <typename result_t, typename function_t, typename... args_t>
        requires(interface::type == ebus_type::GLOBAL)
    static void invoke(result_t& result, function_t&& func, args_t&&... args);

    // invoke id handler function
    template <typename result_t, typename function_t, typename... args_t>
        requires(interface::type == ebus_type::ONE2ONE)
    static void invoke(result_t& result, size_t id, function_t&& func, args_t&&... args);

    // invoke group handler function
    template <typename result_t, typename function_t, typename... args_t>
        requires(interface::type == ebus_type::GROUP)
    static void invoke(result_t& result, size_t id, function_t&& func, args_t&&... args);

private:
    handler_t& find_first_handler();
};

struct ebus_priority_t
{
    const float m_priority;

    constexpr ebus_priority_t() :
        m_priority(0.0f)
    {
    }
    constexpr explicit ebus_priority_t(float p) : // explicit so no implicit conversion
        m_priority(p)
    {
    }
    constexpr inline float val() const { return m_priority; }

    constexpr ebus_priority_t operator+(float another) const
    {
        return ebus_priority_t(m_priority + another);
    }
    constexpr ebus_priority_t operator-(float another) const
    {
        return ebus_priority_t(m_priority - another);
    }
};

template <EBUS_IFACE interface>
class ebus_handler : public interface
{
    friend class ebus<interface>;

protected:
    /// connect via handler type. In this case we only allows one_to_one connection
    void connect(ebus_priority_t p = {});

    // connect via id. This is additional
    bool connect(size_t id, ebus_priority_t p = {});
    bool disconnect();

    static constexpr bool is_one2one() { return interface::type == ebus_type::ONE2ONE; }

public:
    virtual ~ebus_handler() { disconnect(); }

private:
    INTRUSIVE_NS::intrusive_list_node m_node;
    ssize_t                           m_id       = -1;
    float                             m_priority = 0.0f;

    /**
     * the context to hold all the handlers.
     */
    struct ctx
    {
        INTRUSIVE_NS::intrusive_list               m_handlers;
        std::unordered_map<size_t, ebus_handler*>  m_id_handlers;
        std::unordered_map<size_t, INTRUSIVE_NS::intrusive_list> m_group_handlers;
        using group_itr = std::unordered_map<size_t, INTRUSIVE_NS::intrusive_list>::iterator;

        std::mutex m_lock;
    };
    friend class singleton<ctx>;

    static ctx& get_context() { return singleton<ctx>::get_instance(); }
    static void insert_handler_at(intrusive_list&, ebus_handler&, const float);

    // hash_id only available for one_to_one ebus_types
    template <bool enable = interface::type == ebus_type::GLOBAL>
        requires(enable)
    static inline size_t hash_id()
    {
        return typeid(ebus_handler<interface>).hash_code();
    };
};

} // namespace EBUS_NS
