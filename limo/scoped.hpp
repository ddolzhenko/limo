#pragma once


template <class TFunc>
class scoped_type : noncopyable
{
public:
    scoped_type(TFunc f)
    : m_deferred(f)
    {
    }

    ~scoped_type()
    {
        m_deferred()
    }

private:
    m_deferred;
};


template <class TFunc>
scoped_type<TFunc> scoped(TFunc f)
{
    return scoped_type(TFunc);
}


//------------------------------------------------------------------------------

class timer
{
public:
    timer(std::clock_t time=std::clock()): m_time(time) {}

    std::clock_t m_time;

    friend ostream& operator<<(ostream& o, const timer& t)
    {
        return o << t.m_time;
    }
};


class print: noncopyable
{
public: 
    printer(ostream& stream=std::cout):m_stream(stream) {}
    
    template <class T>
    const T& operator(const T& what)
    {
        m_stream << what;
        return *this;
    }
    ostream& m_stream;
};


//------------------------------------------------------------------------------
// composition
template <class F, class G>
decltype(auto) compose(F&& f, G&& g)
{
    return [=](auto x) { return f(g(x)); };
}

template <class F1, class F2, class F3>
decltype(auto) compose(F1&& f1, F2&& f2, F3&& f3)
{
    return [=](auto x) { return F1(F2(F3(x))); };
}

/// .... more versions
 

void test()
{
    scoped(compose(timer(), print));
    scoped(profile(__func__), [](profile& p){ p.exit_scope(__func__) });
    scoped(profile(__func__));
    scoped([](){ cout << 'finished: ' << __func__ << endl; });

    auto wrapper = [](auto what){ 
        cout << "started: " << what << endl;
        return [](){ cout << "finished: " << what << endl; }
    }
    scoped(wrapper);

}