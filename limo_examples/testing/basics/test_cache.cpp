#include <limo/test.hpp>

#include <unordered_map>
#include <functional>

using namespace std;

template <typename TKey>
class LRU {
public:
    static const bool verbose = true;

    typedef TKey                            key_type;
    typedef std::list<key_type>             order_impl;
    typedef typename order_impl::iterator   order_info;
    typedef LRU<TKey> self_type;
    
    order_info promote(order_info x) {
        m_order.push_front(*x);
        m_order.erase(x);
        return m_order.begin();
    }

    order_info push(const key_type& key) {
        m_order.push_front(key);
        return m_order.begin();
    }
    
    TKey pop() {
        assert(!m_order.empty());
        if(verbose) {
            cout << "\tpopping: " << m_order.back() << endl;
        }
        auto key = m_order.back();
        m_order.pop_back();
        return key;
    }

    friend ostream& operator<<(ostream& o, const self_type& order) {
        o << "[";
        for(auto x : order.m_order) {
            o << x << ", ";
        }
        return o << "]";
    }

private:
    order_impl m_order;
};

template <typename TKey>
class LFU {
public:


    typedef TKey                    key_type;
    typedef std::list<key_type>     bucket_type;
    
    std::list<bucket_type>          m_buckets;
    
    struct order_info {
        std::list<bucket_type>  bucket;
        typename bucket_type::iterator   order;
    };
    
    order_info promote(order_info x) {
        auto next_bucket = x.bucket;
        ++next_bucket;
        if (next_bucket == m_buckets.end()) {
            buckets.push_back(Bucket());
            next_bucket = buckets.end();
            -- next_bucket;
        }

        next_bucket->push_front(*x.order);
        x.bucket->erase(x.order);

        return order_info(next_bucket, next_bucket->begin());
    }

    order_info push(const key_type& key) {
        m_order.push_front(key);
        return m_order.begin();
    }
    
    TKey pop() {
        if(!m_buckets.empty() && !m_buckets.front.empty()) {
            m_buckets.front().pop_back();
        }
    }

};



template <typename TKey, typename TValue, 
    template <class> class TCacheStrategy = LRU>
class Cache {
public:
    typedef std::function<TValue(const TKey&)> computor_type;
    typedef std::size_t size_type;
    typedef TKey key_type;

    typedef Cache<TKey, TValue, TCacheStrategy> self_type;
    
public:
    Cache(computor_type computor, size_type capacity=1000)
    : m_capacity(capacity)
    , m_compute(computor)
    {

    }

public: // state

    size_type size() const          { return m_cache.size(); }
    size_type capacity() const      { return m_capacity; }

    bool empty() const              { return size() == 0; }


    TValue& get(const key_type& key) {
        
        auto x = m_cache.find(key);
        const bool hit = m_cache.end() != x;
        if(hit) {
            std::cout << "<hit: " << key << ">: ";

            x->second.info = m_order.promote(x->second.info);
        }
        else {
            std::cout << "<miss: " << key << ">: ";
            assert(size() <= capacity());

            if(size() == capacity()) {
                m_cache.erase(m_order.pop());
            }
            
            x = m_cache.insert(std::make_pair(key, Data(m_compute(key), m_order.push(key)))).first;
        }

        return x->second.value;
    }

    friend ostream& operator<<(ostream& o, const self_type& cache) {
        o <<  "cache capacity=" << cache.capacity() << ", size=" << cache.size() << endl;
        o << "\telements: [";
        for(const auto& x : cache.m_cache) {
            o << "(" << x.first << "," << x.second.value << "), ";
        }
        o   << endl 
            << "\tstrategy: " << cache.m_order << endl;
        return o;
    }

private:

    typedef typename TCacheStrategy<TKey>::order_info order_info;

    struct Data {
        Data(const TValue& v, const order_info& inf): value(v), info(inf) {}
        TValue      value;
        order_info  info;
    };

    typedef std::unordered_map<TKey, Data>  cache_map;

    cache_map m_cache;
    TCacheStrategy<TKey> m_order;
    size_type m_capacity;
    computor_type m_compute;
};



LTEST (caches) {
    using namespace std;

    EXPECT(2*2 == 5);

    auto creator = [](int key) { 
        return float(key)/10;
    };

    Cache<int, float, LRU> cache(creator, 10);

    for(auto i = 1; i < 11; ++i) {
        cout << cache.get(i) << endl;
    }
    
    cout << cache;

    cout << cache.get(24) << endl;

    cout << cache;
    cout << cache.get(1) << endl;
    cout << cache.get(2) << endl;
    cout << cache.get(4) << endl;
    cout << cache;
};