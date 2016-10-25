/******************************************************************************

Copyright (c) Dmitri Dolzhenko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*******************************************************************************/

#pragma once

//------------------------------------------------------------------------------

// include local:

// include std:
#include <list>

// forward declarations:


//------------------------------------------------------------------------------

namespace limo
{

template <typename TKey>
class LRU 
{
public:
    typedef LRU<TKey>           self_type;
    typedef std::list<TKey>     order_type;

public: // contract types
    typedef typename order_type::iterator   order_info;
    typedef typename order_type::value_type key_type;
    
public: // ctors

    // all default

    void swap(self_type& other)
    {
        std::swap(m_order, other.m_order);
    }

public: // CacheStrategy contract interface

    order_info promote(order_info x) 
    {
        m_order.push_front(*x);
        m_order.erase(x);
        return m_order.begin();
    }

    order_info push(const key_type& key) 
    {
        m_order.push_front(key);
        return m_order.begin();
    }
    
    key_type pop() 
    {
        limo_contract(!m_order.empty(), "push/pop call balance broken");

        auto key = m_order.back();
        m_order.pop_back();
        return key;
    }

private: // internals

    friend std::ostream& operator<<(std::ostream& o, const self_type& order) 
    {
        o << "[";
        for(const auto& x : order.m_order) 
            o << x << ", ";
        return o << "]";
    }

private:
    order_type m_order;
};
   

} // namespace limo

//------------------------------------------------------------------------------
