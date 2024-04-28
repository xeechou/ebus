#pragma once

#include <memory/intrusive_list.hh>

#include <cstddef>
#include <functional>
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

/**
 * ebus the event bus interface
 *
 * This ebus interface is an simplified o3de's EBus implementation. That we do
 * not support different types of ebus(singleHandler/MultiHandler) through
 * EBusTraits. We have a simplified ebus_handler implementation.
 *
 */
template <class interface>
class ebus;

template <class interface>
class ebus_handler;

template <class interface>
class ebus
{
public:
    using handler_t = ebus_handler<interface>;

    template <typename function_t, typename... args_t>
    static void event(size_t id, function_t&& func, args_t&&... args);

    // broadcast an event
    template <typename function_t, typename... args_t>
    static void broadcast(function_t&& func, args_t&&... args);

    // invoke non-ided handler function
    template <typename result_t, typename function_t, typename... args_t>
    static void invoke(result_t& result, function_t&& func, args_t&&... args);

    // invoke id handler function
    template <typename result_t, typename function_t, typename... args_t>
    static void invoke(result_t& result, size_t id, function_t&& func, args_t&&... args);

private:
    handler_t& find_first_handler();
};

template <class interface>
class ebus_handler : public interface
{
    friend class ebus<interface>;

protected:
    // connect via the handler type
    void connect();
    // connect via id. This is additional
    bool connect(size_t id);
    bool disconnect();
    bool has_id() const { return m_id != -1; }

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

    static inline size_t hash_id()
    {
        return typeid(ebus_handler<interface>).hash_code();
    };
};

/**
 * connect the bus listeners
 */
template <class interface>
void
ebus_handler<interface>::connect()
{
    size_t   id  = hash_id();
    context& ctx = get_context();

    ctx.m_handlers.push_back(m_node);
}

template <class interface>
bool
ebus_handler<interface>::connect(size_t id)
{
    // we only allow single handler to register with the id.
    auto& id_handlers = get_context().m_id_handlers;
    if (id_handlers.find(id) != id_handlers.end())
        return false;
    id_handlers[id] = this;
    m_id            = (signed)id;
    return true;
}

template <class interface>
bool
ebus_handler<interface>::disconnect()
{
    if (has_id())
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

template <class interface>
template <typename function_t, typename... args_t>
// requires(has_function<ebus<interface>, function_t, args_t...>)
void
ebus<interface>::event(size_t id, function_t&& func, args_t&&... args)
{
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

template <class interface>
template <typename function_t, typename... args_t>
void
ebus<interface>::broadcast(function_t&& func, args_t&&... args)
{
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
    // Do I notify all the id handlers as well?
}

template <class interface>
template <typename result_t, typename function_t, typename... args_t>
void
ebus<interface>::invoke(result_t& result, function_t&& func, args_t&&... args)
{
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

template <class interface>
template <typename result_t, typename function_t, typename... args_t>
void
ebus<interface>::invoke(result_t& result, size_t id, function_t&& func, args_t&&... args)
{
    typename handler_t::context& ctx = handler_t::get_context();
    if (ctx.m_id_handlers.find(id) != ctx.m_id_handlers.end())
    {
        handler_t& handler = ctx.m_id_handlers.at(id);

        auto exec = std::bind(std::forward<function_t>(func),
                              &handler,
                              std::forward<args_t>(args)...);
        result    = exec();
    }
}

} // namespace sp
