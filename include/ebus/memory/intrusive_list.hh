#pragma once

#ifndef INTRUSIVE_NS
#    define INTRUSIVE_NS ITRV_NS
#endif

#include <stdint.h>
#include <stddef.h>
#include <iterator>

namespace INTRUSIVE_NS
{

class intrusive_list_node;

class intrusive_list;

template <class T>
class intrusive_list_iterator;

template <class T>
class intrusive_list_iterable;

class intrusive_list_node
{
    friend class intrusive_list;

    template <class T>
    friend class intrusive_list_iterator;

protected:
    intrusive_list_node *m_prev, *m_next;

public:
    constexpr intrusive_list_node() :
        m_prev(this),
        m_next(this) {};
    intrusive_list_node(const intrusive_list_node& another) = delete;
    intrusive_list_node(intrusive_list_node&& another)      = delete;
    ~intrusive_list_node() { earse(); }

    bool idle() const { return m_prev == this && m_next == this; }

    void insert(intrusive_list_node& other)
    {
        intrusive_list_node* nextp = this->m_next;

        this->m_next  = &other;
        other.m_prev  = this;
        other.m_next  = nextp;
        nextp->m_prev = &other;
    }
    void insert_before(intrusive_list_node& other) { this->m_prev->insert(other); }
    void earse()
    {
        intrusive_list_node *prevp = this->m_prev, *nextp = this->m_next;

        prevp->m_next = nextp;
        nextp->m_prev = prevp;

        this->m_prev = this;
        this->m_next = this;
    }
    // void splice( kernel_list &list );

    template <class T>
    inline constexpr T* container(const intrusive_list_node T::*member) const
    {
        // C++ equivalences of
        // [offsetof](http://man7.org/linux/man-pages/man3/offsetof.3.html
        // "offsetof(3) - Linux manual page") and
        // [container_of](https://stackoverflow.com/questions/15832301/understanding-container-of-macro-in-the-linux-kernel
        // "C - Understanding container_of macro in the Linux kernel - Stack
        // Overflow") macro
        return reinterpret_cast<T*>(
            reinterpret_cast<intptr_t>(this) -
            reinterpret_cast<ptrdiff_t>(&(reinterpret_cast<T*>(0)->*member)));
    }
};

class intrusive_list
{
    template <class T>
    friend class intrusive_list_iterable;

protected:
    intrusive_list_node m_head;

public:
    using size_type       = size_t;
    using difference_type = ptrdiff_t;

    intrusive_list_node* front() const
    {
        return empty() ? nullptr : this->m_head.m_next;
    }
    intrusive_list_node* back() const { return empty() ? nullptr : this->m_head.m_prev; }
    intrusive_list_node* prev(intrusive_list_node& ref) const
    {
        intrusive_list_node* prev = ref.m_prev;
        return prev == &this->m_head ? nullptr : prev;
    }
    intrusive_list_node* next(intrusive_list_node& ref) const
    {
        intrusive_list_node* next = ref.m_next;
        return next == &this->m_head ? nullptr : next;
    }
    bool      empty() const { return this->m_head.m_next == &this->m_head; }
    size_type size() const
    {
        intrusive_list::size_type s = 0;

        for (intrusive_list_node* p = this->m_head.m_next; p != &this->m_head;
             p                      = p->m_next)
        {
            s += 1;
        }

        return s;
    }
    intrusive_list_node* at(difference_type idx) const
    {
        if (empty())
        {
            return nullptr;
        }
        else
        {
            intrusive_list_node* p = const_cast<intrusive_list_node*>(&this->m_head);

            if (idx >= 0)
            {
                while (idx >= 0)
                {
                    p = p->m_next;

                    if (p != &this->m_head)
                    {
                        idx -= 1;
                    }
                }
            }
            else
            {
                while (idx < 0)
                {
                    p = p->m_prev;

                    if (p != &this->m_head)
                    {
                        idx += 1;
                    }
                }
            }

            return p;
        }
    }
    difference_type index_of(intrusive_list_node& ref) const
    {
        intrusive_list::size_type idx = 0;

        for (intrusive_list_node* p = this->m_head.m_next; p != &this->m_head;
             p                      = p->m_next)
        {
            if (p == &ref)
            {
                return idx;
            }
            else if (p)
            {
                idx += 1;
            }
        }

        return -1;
    }
    void push_front(intrusive_list_node& node) { this->m_head.insert(node); }
    void pop_front()
    {
        if (!empty())
        {
            this->m_head.m_next->earse();
        }
    }
    void push_back(intrusive_list_node& node) { this->m_head.insert_before(node); }
    void pop_back()
    {
        if (!empty())
        {
            this->m_head.m_prev->earse();
        }
    }
    void shift_forward(difference_type num = 1)
    {
        intrusive_list_node* dst = at(num);

        this->m_head.earse();
        dst->insert_before(this->m_head);
    }
    void shift_backwards(size_type num = 1)
    {
        shift_forward(-static_cast<intrusive_list::difference_type>(num));
    }
    void clear()
    {
        while (!empty())
        {
            this->m_head.m_next->earse();
        }
    }
};

template <class T>
class intrusive_list_iterator
{
private:
    intrusive_list_node*      m_node;
    const intrusive_list_node T::*m_member;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type   = ptrdiff_t;
    using value_type        = T;
    using reference         = T&;
    using pointer           = T*;

    constexpr intrusive_list_iterator(intrusive_list_node&      node,
                                      const intrusive_list_node T::*member) :
        m_node(&node),
        m_member(member)
    {
    }
    bool operator!=(const intrusive_list_iterator<T>& rhs) const
    {
        return this->m_node != rhs.m_node;
    }
    reference operator*() const { return *(this->m_node->container(this->m_member)); }
    pointer   operator->() const { return (this->m_node->container(this->m_member)); }

    intrusive_list_iterator<T>& operator++()
    {
        this->m_node = this->m_node->m_next;
        return *this;
    }

    intrusive_list_iterator<T> next() const
    {
        return intrusive_list_iterator<T>(*this->m_node->m_next, m_member);
    }

    intrusive_list_iterator<T> prev() const
    {
        return intrusive_list_iterator<T>(*this->m_node->m_prev, m_member);
    }

    intrusive_list_iterator<T>& operator--()
    {
        this->m_node = this->m_node->m_prev;
        return *this;
    }
};

template <class T>
class intrusive_list_iterable
{
private:
    intrusive_list&           m_list;
    const intrusive_list_node T::*m_member;

public:
    constexpr intrusive_list_iterable(intrusive_list&           list,
                                      const intrusive_list_node T::*member) :
        m_list(list),
        m_member(member)
    {
    }

    intrusive_list_iterator<T> begin() const
    {
        intrusive_list_node* first = m_list.front();

        if (nullptr == first)
        {
            return this->end();
        }
        else
        {
            intrusive_list_iterator<T> temp(*first, this->m_member);

            return temp;
        }
    }

    intrusive_list_iterator<T> end() const
    {
        intrusive_list_iterator<T> temp(m_list.m_head, this->m_member);
        return temp;
    }
};

// void test()
// {
//  struct int_node {
//      int data = 0;
//      intrusive_list_node m_member;
//  };

//  intrusive_list head;

//  int_node node;
//  node.data = 1;

//  int_node node1;
//  node1.data = 2;

//  head.push_back(node.m_member);
//  head.push_back(node1.m_member);

//  //construct the interable on the fly.
//  for (auto& node : intrusive_list_iterable<int_node>(head, &int_node::m_member))
//  {
//      std::cout << node.data << std::endl;
//  }
// }

// template< class T >
// inline constexpr kernel_list_node member( const T &container, const kernel_list_node
// T::*member ) {
//  // C++ equivalences of offsetof and container_of macro
//  return reinterpret_cast< T* >( reinterpret_cast< intptr_t >( &container ) +
//      reinterpret_cast< ptrdiff_t >( &( reinterpret_cast< T* >( 0 )->*member ) ) );
// }

} // namespace INTRUSIVE_NS
