#pragma once

#include <memory/intrusive_list.hh>

#include <cstddef>
#include <functional>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <unordered_map>

// here we define a concept that type T need to has a function
template <typename T, typename function_t, typename... args_t>
concept has_function = requires(T t, function_t&& func, args_t&&... args) {
    {
        t.func(args...)
    };
};

namespace sp
{

enum ebus_type
{
    TYPED = 0,
    IDED  = 1,
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
    static void event(function_t&& func, args_t&&... args);

    // send event to specific id
    template <typename function_t, typename... args_t>
    static void event(size_t id, function_t&& func, args_t&&... args);

    // broadcast an event to multi-handlers
    template <typename function_t, typename... args_t>
    static void broadcast(function_t&& func, args_t&&... args);

    // invoke non-ided handler function
    template <typename result_t, typename function_t, typename... args_t>
        requires(interface::type == ebus_type::TYPED)
    static void invoke(result_t& result, function_t&& func, args_t&&... args);

    // invoke id handler function
    template <typename result_t, typename function_t, typename... args_t>
        requires(interface::type == ebus_type::IDED)
    static void invoke(result_t& result, size_t id, function_t&& func, args_t&&... args);

private:
    handler_t& find_first_handler();
};

template <EBUS_IFACE interface>
class ebus_handler : public interface
{
    friend class ebus<interface>;

protected:
    /// connect via handler type. In this case we only allows one_to_one connection
    void connect();

    // connect via id. This is additional
    bool connect(size_t id);
    bool disconnect();

    static constexpr bool is_ided() { return interface::type == ebus_type::IDED; }

private:
    intrusive_list_node m_node;
    ssize_t             m_id = -1;

    /**
     * the context to hold all the handlers.
     */
    struct context
    {
        intrusive_list                            m_handlers;
        std::unordered_map<size_t, ebus_handler*> m_id_handlers;
    };

    static context& get_context()
    {
        // I am using static variable here for simple implementation.
        static context s_ctx;
        return s_ctx;
    }

    // hash_id only available for one_to_one ebus_types
    template <bool enable = interface::type == ebus_type::TYPED>
        requires(enable)
    static inline size_t hash_id()
    {
        return typeid(ebus_handler<interface>).hash_code();
    };
};

/**
 * connect the bus listeners
 */
template <EBUS_IFACE interface>
void
ebus_handler<interface>::connect()
{
    static_assert(interface::type == ebus_type::TYPED,
                  "non-id connect() are reserved for type based ebus handlers");
    size_t   id  = hash_id();
    context& ctx = get_context();

    ctx.m_handlers.push_back(m_node);
}

/**
 * connect
 */
template <EBUS_IFACE interface>
bool
ebus_handler<interface>::connect(size_t id)
{
    static_assert(interface::type == ebus_type::IDED,
                  "id connect(id) are reserved for id based ebus handlers");

    auto& id_handlers = get_context().m_id_handlers;
    if (id_handlers.find(id) != id_handlers.end())
        return false;
    id_handlers[id] = this;
    m_id            = (signed)id;
    return true;
}

template <EBUS_IFACE interface>
bool
ebus_handler<interface>::disconnect()
{
    if (is_ided())
    {
        auto& id_handlers = get_context().m_id_handlers;
        if (id_handlers.find(m_id) == id_handlers.end())
            return false;
        if (id_handlers.at(m_id) != this)
            return false;

        id_handlers.erase(m_id);
        m_id = -1;
    }
    else
    {
        m_node.earse();
    }
    return true;
}

template <EBUS_IFACE interface>
template <typename function_t, typename... args_t>
void
ebus<interface>::event(size_t id, function_t&& func, args_t&&... args)
{
    static_assert(interface::type == ebus_type::IDED,
                  "event(id) is reserved only for id based ebus");

    typename handler_t::context& ctx = handler_t::get_context();
    if (ctx.m_id_handlers.find(id) != ctx.m_id_handlers.end())
    {
        handler_t& handler = ctx.m_id.handlers.at(id);

        auto exec = std::bind(std::forward<function_t>(func),
                              &handler,
                              std::forward<args_t>(args)...);
        exec();
    }
}

template <EBUS_IFACE interface>
template <typename function_t, typename... args_t>
void
ebus<interface>::broadcast(function_t&& func, args_t&&... args)
{
    static_assert(interface::type == ebus_type::TYPED,
                  "broadcast() is reserved only for type based ebus");

    typename handler_t::context& ctx = handler_t::get_context();
    for (handler_t& handler :
         intrusive_list_iterable<handler_t>(ctx.m_handlers, &handler_t::m_node))
    {
        // we are using the feature of bind to a pointer to member function: "
        auto functor = std::bind(std::forward<function_t>(func),
                                 &handler,
                                 std::forward<args_t>(args)...);
        functor();
    }
}

template <EBUS_IFACE interface>
template <typename result_t, typename function_t, typename... args_t>
    requires(interface::type == ebus_type::TYPED)
void
ebus<interface>::invoke(result_t& result, function_t&& func, args_t&&... args)
{
    static_assert(interface::type == ebus_type::TYPED,
                  "invoke() without id is only reserved for type based ebus");

    // we only gets the result of the first listener
    typename handler_t::context& ctx = handler_t::get_context();
    for (handler_t& handler :
         intrusive_list_iterable<handler_t>(ctx.m_handlers, &handler_t::m_node))
    {
        auto exec = std::bind(std::forward<function_t>(func),
                              &handler,
                              std::forward<args_t>(args)...);
        result    = exec();
        break;
    }
}

template <EBUS_IFACE interface>
template <typename result_t, typename function_t, typename... args_t>
    requires(interface::type == ebus_type::IDED)
void
ebus<interface>::invoke(result_t& result, size_t id, function_t&& func, args_t&&... args)
{
    static_assert(interface::type == ebus_type::IDED,
                  "invoke(id) is reserved only for id based ebus");

    typename handler_t::context& ctx = handler_t::get_context();
    if (ctx.m_id_handlers.find(id) != ctx.m_id_handlers.end())
    {
        handler_t* handler = ctx.m_id_handlers.at(id);

        auto exec = std::bind(std::forward<function_t>(func),
                              handler,
                              std::forward<args_t>(args)...);
        result    = exec();
    }
}

} // namespace sp
