#pragma once

#ifndef INTRUSIVE_NS
#    define INTRUSIVE_NS EBUS_NS
#endif
#include "ebus/memory/intrusive_list.hh"

#include <functional>
#include <atomic>

namespace EBUS_NS
{

template <typename... args>
class event;

template <typename... args>
class event_handler
{
    template <typename... Args>
    friend class event;

public:
    typedef std::function<void(args...)> handler_func;
    typedef uint32_t                     handler_id;

    event_handler();
    event_handler(const handler_func& func, event<args...>* ev = nullptr);
    event_handler(const event_handler& handler) = delete;
    event_handler(event_handler&& handler);
    ~event_handler();

    event_handler& operator=(event_handler&& handler);
    void           operator()(args... params);
    bool           operator==(const event_handler<args...> handler);

    handler_id id() const;
    bool       idle() const;
    void       disconnect();

private:
    handler_id                            m_id;
    handler_func                          m_handler_func;
    static inline std::atomic<handler_id> s_id_counter = 0;

    intrusive_list_node m_node;
};

class event_base
{
protected:
    event_base() {}
};

///@brief event is an object based event system, as opposed to ebus, which are
/// type based events.
template <typename... Args>
class event : public event_base
{
public:
    typedef event_handler<Args...>         handler;
    typedef typename handler::handler_func   func;
    typedef intrusive_list_iterable<handler> iterable;
    typedef intrusive_list_iterator<handler> iterator;

public:
    event();
    ~event();

    void dispatch(Args... args);

    void connect(handler& handler);

protected:
    // problem is, event directly owns the handlers, I need Intrusive list
    intrusive_list m_head;

    // std::list<handler> m_collections;
    mutable std::mutex m_handlers_lock;
};

} // namespace EBUS_NS
