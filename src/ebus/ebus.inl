#pragma once

#include "ebus.def.hh"
#include "memory/intrusive_list.hh"

#include <functional>
#include <utility>

namespace sp
{

///////////////////////////////////////////////////////////////////////////////
// ebus_handler
///////////////////////////////////////////////////////////////////////////////

/**
 * connect the bus listeners
 */
template <EBUS_IFACE interface>
void
ebus_handler<interface>::connect()
{
    static_assert(interface::type == ebus_type::GLOBAL,
                  "non-id connect() are reserved for type based ebus handlers");
    size_t id  = hash_id();
    ctx&   ctx = get_context();

    ctx.m_handlers.push_back(m_node);
}

/**
 * connect
 */
template <EBUS_IFACE interface>
bool
ebus_handler<interface>::connect(size_t id)
{
    static_assert(interface::type == ebus_type::ONE2ONE ||
                      interface::type == ebus_type::GROUP,
                  "id connect(id) are reserved for ONE2ONE or GROUP ebus handlers");

    if (interface::type == ebus_type::ONE2ONE)
    {
        auto& id_handlers = get_context().m_id_handlers;
        if (id_handlers.find(id) != id_handlers.end())
            return false;

        id_handlers[id] = this;
        m_id            = (signed)id;
    }
    else // group case
    {
        get_context().m_group_handlers[id].push_back(m_node);
    }

    return true;
}

template <EBUS_IFACE interface>
bool
ebus_handler<interface>::disconnect()
{
    if (is_one2one())
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

///////////////////////////////////////////////////////////////////////////////
// ebus
///////////////////////////////////////////////////////////////////////////////

template <EBUS_IFACE interface>
template <typename function_t, typename... args_t>
void
ebus<interface>::event(size_t id, function_t&& func, args_t&&... args)
{
    static_assert(interface::type == ebus_type::ONE2ONE,
                  "event(id) is reserved only for id based ebus");

    typename handler_t::ctx& ctx = handler_t::get_context();
    if (ctx.m_id_handlers.find(id) != ctx.m_id_handlers.end())
    {
        handler_t* handler = ctx.m_id_handlers.at(id);

        auto exec = std::bind(std::forward<function_t>(func),
                              handler,
                              std::forward<args_t>(args)...);
        exec();
    }
}

template <EBUS_IFACE interface>
template <typename function_t, typename... args_t>
void
ebus<interface>::multicast(size_t id, function_t&& func, args_t&&... args)
{
    static_assert(interface::type == ebus_type::GROUP,
                  "multicast(id) is reserved only for group type ebus");
    typename handler_t::ctx&           ctx = handler_t::get_context();
    typename handler_t::ctx::group_itr itr = ctx.m_group_handlers.find(id);
    // find the group
    if (itr != ctx.m_group_handlers.end())
    {
        intrusive_list& head = itr->second;
        for (handler_t& handler :
             intrusive_list_iterable<handler_t>(head, &handler_t::m_node))
        {
            auto functor = std::bind(std::forward<function_t>(func),
                                     &handler,
                                     std::forward<args_t>(args)...);
            functor();
        }
    }
}

template <EBUS_IFACE interface>
template <typename function_t, typename... args_t>
void
ebus<interface>::broadcast(function_t&& func, args_t&&... args)
{
    static_assert(interface::type == ebus_type::GLOBAL,
                  "broadcast() is reserved only for global type ebus");

    typename handler_t::ctx& ctx = handler_t::get_context();
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
    requires(interface::type == ebus_type::GLOBAL)
void
ebus<interface>::invoke(result_t& result, function_t&& func, args_t&&... args)
{
    static_assert(interface::type == ebus_type::GLOBAL,
                  "invoke() without id is only reserved for type based ebus");

    // we only gets the result of the first listener
    typename handler_t::ctx& ctx = handler_t::get_context();
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
    requires(interface::type == ebus_type::ONE2ONE)
void
ebus<interface>::invoke(result_t& result, size_t id, function_t&& func, args_t&&... args)
{
    static_assert(interface::type == ebus_type::ONE2ONE,
                  "invoke(id) is reserved only for id based ebus");

    typename handler_t::ctx& ctx = handler_t::get_context();
    if (ctx.m_id_handlers.find(id) != ctx.m_id_handlers.end())
    {
        handler_t* handler = ctx.m_id_handlers.at(id);

        auto exec = std::bind(std::forward<function_t>(func),
                              handler,
                              std::forward<args_t>(args)...);
        result    = exec();
    }
}

template <EBUS_IFACE interface>
template <typename result_t, typename function_t, typename... args_t>
    requires(interface::type == ebus_type::GROUP)
void
ebus<interface>::invoke(result_t& result, size_t id, function_t&& func, args_t&&... args)
{
    static_assert(interface::type == ebus_type::GROUP,
                  "invoke() without id is only reserved for group based ebus");

    typename handler_t::ctx&           ctx = handler_t::get_context();
    typename handler_t::ctx::group_itr itr = ctx.m_group_handlers.find(id);
    // find the group
    if (itr != ctx.m_group_handlers.end())
    {
        intrusive_list& head = itr->second;
        // we only get result from first listener
        for (handler_t& handler :
             intrusive_list_iterable<handler_t>(head, &handler_t::m_node))
        {
            auto exec = std::bind(std::forward<function_t>(func),
                                  &handler,
                                  std::forward<args_t>(args)...);
            result    = exec();
            break;
        }
    }
}

} // namespace sp
