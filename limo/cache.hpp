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
#include <limo/cache/LRU.hpp>
#include <limo/assert.hpp>

// include std:
#include <functional>
#include <unordered_map>

// forward declarations:


//------------------------------------------------------------------------------

namespace limo
{


template <typename TKey, typename TValue, class TCacheStrategy = LRU<TKey> >
class Cache 
{
public: 
    typedef Cache<TKey, TValue, TCacheStrategy> self_type;
    
    typedef std::size_t size_type;
    typedef TKey        key_type;
    typedef TValue      cached_type;

    typedef std::function<cached_type(const key_type&)> computor_type;

    typedef TCacheStrategy                              strategy_type;
    typedef typename strategy_type::order_info          order_info;
    
    typedef std::pair<cached_type, order_info>          cache_line;
    typedef std::unordered_map<key_type, cache_line>    cache_map;

    struct statistics_type
    {
        size_type hits;
        size_type misses;
    };

    
public: // ctors
    Cache(
        computor_type           computor, 
        size_type               capacity, 
        const strategy_type&    strategy = strategy_type()
    )
    : m_cache()
    , m_compute(computor)
    , m_capacity(capacity)
    , m_strategy(strategy)
    , m_stat()
    {
        limo_scope_invariant(is_valid());
    }

    ~Cache() = default;
    Cache(const self_type&) = default;
    Cache(self_type&&) = default;
    self_type& operator=(const self_type&) = default;
    self_type& operator=(self_type&&) = default;

    void swap(self_type& other) // nothrow swap for O(1)
    {
        std::swap(m_cache, other.m_cache);
        std::swap(m_compute, other.m_compute);
        std::swap(m_capacity, other.m_capacity);
        std::swap(m_strategy, other.m_strategy);
        std::swap(m_stat, other.m_stat);
    }

public: // state

    bool        empty()     const   { return size() == 0; }
    size_type   size()      const   { return m_cache.size(); }
    size_type   capacity()  const   { return m_capacity; }

    float       hit_ratio() const   { return float(m_stat.hits)/(m_stat.hits+m_stat.misses()); }

public: // main interface

    cached_type& operator[](const key_type& key) 
    {
        limo_scope_invariant(is_valid());

        typename cache_map::iterator x = m_cache.find(key);
        bool cache_hit = x != m_cache.end();
        if (cache_hit)
        {
            ++m_stat.hits;
            x->second.second = m_strategy.promote(x->second.second);
        }
        else 
        {
            ++m_stat.misses;

            if(size() == capacity()) {
                m_cache.erase(m_strategy.pop());
            }
            
            auto line_info = m_strategy.push(key);
            auto result = m_cache.insert(
                    std::make_pair(key, cache_line(m_compute(key), line_info)));

            limo_assert(result.second, "key should not be there, it's miss branch");
            x = result.first;
        }

        return x->second.first;
    }

private: // implementation details

    bool is_valid() const
    {
        return true;
    }


    friend std::ostream& operator<<(std::ostream& o, const self_type& cache) 
    {
        o   <<  "cache capacity=" << cache.capacity() 
            << ", size=" << cache.size() << std::endl
            << "\telements: [";
        for(const auto& x : cache.m_cache)
        {
            o << "(" << x.first << "," << x.second.value << "), ";
        }
        
        o   << std::endl << "\tstrategy: " << cache.m_order;
        return o;
    }

private:
    
    cache_map       m_cache;
    computor_type   m_compute;
    size_type       m_capacity;
    strategy_type   m_strategy;
    statistics_type m_stat;
};

    
} // namespace limo

//------------------------------------------------------------------------------
