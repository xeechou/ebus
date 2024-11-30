#pragma once

#include "event.def.hh"
#include "ebus/memory/intrusive_list.hh"

namespace EBUS_NS
{

////////////////////////////////////////////////////////////////////////////////
// event_handler
////////////////////////////////////////////////////////////////////////////////

template <typename... args>
event_handler<args...>::event_handler() :
    m_id(++s_id_counter)
{
}

template <typename... args>
event_handler<args...>::event_handler(const handler_func& func, event<args...>* ev) :
    m_handler_func(func),
    m_id(++s_id_counter)
{
    if (ev)
    {
        ev->connect(*this);
    }
}

template <typename... args>
event_handler<args...>::event_handler(event_handler&& other) :
    m_id(other.m_id),
    m_handler_func(other.m_handler_func)
{
    other.m_handler_func = {};

    // insert us and remove the old handler
    if (!other.idle())
    {
        other.m_node.insert_before(m_node);
        other.m_node.earse();
    }
}

template <typename... args>
event_handler<args...>::~event_handler()
{
    disconnect();
}

template <typename... args>
event_handler<args...>&
event_handler<args...>::operator=(event_handler&& rhs)
{
    m_handler_func = rhs.m_handler_func;
    m_id           = rhs.m_id;

    rhs.m_handler_func = {};

    // insert us and remove the old handler
    if (!rhs.idle())
    {
        rhs.m_node.insert_before(m_node);
        rhs.m_node.earse();
    }

    return *this;
}

template <typename... args>
void
event_handler<args...>::operator()(args... params)
{
    if (m_handler_func)
    {
        m_handler_func(params...);
    }
}

template <typename... args>
bool
event_handler<args...>::operator==(const event_handler<args...> rhs)
{
    return m_id == rhs.id() && idle() && rhs.idle();
}

template <typename... args>
bool
event_handler<args...>::idle() const
{
    return m_node.idle();
}

template <typename... args>
event_handler<args...>::handler_id
event_handler<args...>::id() const
{
    return m_id;
}
template <typename... args>
void
event_handler<args...>::disconnect()
{
    m_node.earse();
}

////////////////////////////////////////////////////////////////////////////////
// event
////////////////////////////////////////////////////////////////////////////////
template <typename... args>
event<args...>::event() :
    event_base()
{
}

template <typename... args>
event<args...>::~event()
{
    iterable head(m_head, &handler::m_node);
    for (iterator pos = head.begin(), tmp = pos.next(); pos != head.end();
         pos = tmp, tmp = tmp.next())
    {
        handler& handler = *pos;
        handler.disconnect();
    }
}

template <typename... args>
void
event<args...>::dispatch(args... params)
{
    for (handler& handler : intrusive_list_iterable<handler>(m_head, &handler::m_node))
    {
        handler(params...);
    }
}

template <typename... args>
void
event<args...>::connect(handler& handler)
{
    std::lock_guard<std::mutex> lock(m_handlers_lock);
    handler.m_node.earse();
    m_head.push_back(handler.m_node);
}

} // namespace EBUS_NS
