#pragma once

#ifndef INTRUSIVE_NS
#    define INTRUSIVE_NS ITRV_NS
#endif

#include <concepts>
#    include <memory>
#    include <type_traits>
#    include <assert.h>

namespace INTRUSIVE_NS
{

/**
 * intrusive_ptr_count_policy
 *
 * The implementation of reference counting is on the actual objects that uses
 * the ptr.
 *
 */
template <class T>
struct intrusive_ptr_count_policy
{
    static inline void add_ref(T* p) { p->add_ref(); }
    static inline void release(T* p) { p->release(); }
};

/**
 * intrusive_ptr
 *
 * A smart pointer that uses intrusive reference counting.
 *
 * That is, the T of intrusive_ptr<T> implements the reference counting
 * itself. The intrusive_ptr is considered to be faster than std::shared_ptr
 * because std::shared_ptr usually stores the reference counting in the pointer
 * object.
 *
 * class shared_ptr<T>
 * {
 *    T*    px;
 *    uint* counter; the counter could get from last shared_ptr
 * };
 */
template <class T>
class intrusive_ptr
{
private:
    template <typename U>
    friend class intrusive_ptr;

    using count_policy = intrusive_ptr_count_policy<T>;
    using this_type    = intrusive_ptr<T>;

public:
    ~intrusive_ptr()
    {
        if (m_px != nullptr)
            count_policy::release(m_px);
    }

    ///////////////////////////////////////////////////////////////////////////
    // simple constructors
    ///////////////////////////////////////////////////////////////////////////

    intrusive_ptr() :
        m_px(0)
    {
    }

    intrusive_ptr(T* p) :
        m_px(p)
    {
        if (m_px != 0)
            count_policy::add_ref(m_px);
    }

    intrusive_ptr& operator=(T* rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    // copy constructors/operator=
    ///////////////////////////////////////////////////////////////////////////
    intrusive_ptr(intrusive_ptr const& rhs) :
        m_px(rhs.m_px)
    {
        if (m_px != nullptr)
            count_policy::add_ref(m_px);
    }

    template <class U, bool enable = std::convertible_to<U*, T*>>
        requires(enable)
    intrusive_ptr(intrusive_ptr<U> const& rhs) :
        m_px(rhs.m_px)
    {
        if (m_px != nullptr)
            count_policy::add_ref(m_px);
    }

    intrusive_ptr& operator=(intrusive_ptr const& rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    template <class U, bool enable = std::convertible_to<U*, T*>>
        requires(enable)
    intrusive_ptr& operator=(intrusive_ptr<U> const& rhs)
    {
        // firstly we create a temporary intrusive_ptr to swap our data, rhs
        // ref+1.
        this_type(rhs).swap(*this);
        return *this;
        // when this_type goes out of scope, original ptr ref-1
    }

    ///////////////////////////////////////////////////////////////////////////
    // move constructors/operator=
    ///////////////////////////////////////////////////////////////////////////
    intrusive_ptr(intrusive_ptr&& rhs) :
        m_px(rhs.m_px)
    {
        rhs.m_px = nullptr;
    }

    template <class U, bool enable = std::convertible_to<U*, T*>>
        requires(enable)
    intrusive_ptr(intrusive_ptr<U>&& rhs) :
        m_px(rhs.m_px)
    {
        // do not add ref count
        rhs.m_px = nullptr;
    }

    intrusive_ptr& operator=(intrusive_ptr&& rhs)
    {
        this_type(static_cast<intrusive_ptr&&>(rhs)).swap(*this);
        return *this;
    }

    template <class U, bool enable = std::convertible_to<U*, T*>>
        requires(enable)
    intrusive_ptr& operator=(intrusive_ptr<U>&& rhs)
    {
        this_type(std::move(rhs)).swap(*this);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    // operators
    ///////////////////////////////////////////////////////////////////////////

    T* operator->() const
    {
        assert(m_px != nullptr && "You cannot de-reference a nullptr");
        return m_px;
    }

    T& operator*() const
    {
        assert(m_px != nullptr && "You can't de-reference a nullptr");
        return *m_px;
    }

    bool operator!() const { return m_px == 0; }

    ///////////////////////////////////////////////////////////////////////////
    // member functions
    ///////////////////////////////////////////////////////////////////////////
    void reset() { this_type().swap(*this); }

    void reset(T* rhs) { this_type(rhs).swap(*this); }

    T* get() const { return m_px; }

    void swap(intrusive_ptr& rhs)
    {
        T* tmp   = m_px;
        m_px     = rhs.px;
        rhs.m_px = tmp;
    }

private:
    T* m_px = nullptr;
};

///////////////////////////////////////////////////////////////////////////
// binary operators
///////////////////////////////////////////////////////////////////////////

template <class T, class U>
inline bool
operator==(intrusive_ptr<T> const& a, intrusive_ptr<U> const& b)
{
    return a.get() == b.get();
}

template <class T, class U>
inline bool
operator!=(intrusive_ptr<T> const& a, intrusive_ptr<U> const& b)
{
    return a.get() != b.get();
}

template <class T, class U>
inline bool
operator==(intrusive_ptr<T> const& a, U* b)
{
    return a.get() == b;
}

template <class T, class U>
inline bool
operator!=(intrusive_ptr<T> const& a, U* b)
{
    return a.get() != b;
}

template <class T, class U>
inline bool
operator==(T* a, intrusive_ptr<U> const& b)
{
    return a == b.get();
}

template <class T, class U>
inline bool
operator!=(T* a, intrusive_ptr<U> const& b)
{
    return a != b.get();
}

template <class T>
inline bool
operator<(intrusive_ptr<T> const& a, intrusive_ptr<T> const& b)
{
    return (a.get() < b.get());
}

template <class T>
void
swap(intrusive_ptr<T>& lhs, intrusive_ptr<T>& rhs)
{
    lhs.swap(rhs);
}

///////////////////////////////////////////////////////////////////////////
// pointer cast
///////////////////////////////////////////////////////////////////////////

template <class T, class U>
intrusive_ptr<T>
static_pointer_cast(intrusive_ptr<U> const& p)
{
    return static_cast<T*>(p.get());
}

template <class T, class U>
intrusive_ptr<T>
const_pointer_cast(intrusive_ptr<U> const& p)
{
    return const_cast<T*>(p.get());
}

template <class T, class U>
intrusive_ptr<T>
dynamic_pointer_cast(intrusive_ptr<U> const& p)
{
    return dynamic_cast<T*>(p.get());
}

} // namespace INTRUSIVE_NS

///////////////////////////////////////////////////////////////////////////
// hash support
///////////////////////////////////////////////////////////////////////////

namespace std
{
// hashing support for STL containers
template <typename T>
struct hash<INTRUSIVE_NS::intrusive_ptr<T>>
{
    inline size_t operator()(const INTRUSIVE_NS::intrusive_ptr<T>& value) const
    {
        return hash<T*>()(value.get());
    }
};

} // namespace std
