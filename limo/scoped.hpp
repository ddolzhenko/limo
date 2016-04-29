/******************************************************************************

Copyright (c) Dmitry Dolzhenko

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

// forward declarations:


//------------------------------------------------------------------------------

namespace limo
{

    template <class TFunc> class scoped_type : noncopyable
    {
    public:
        scoped_type(TFunc f): m_deferred(f) {}
        ~scoped_type() { m_deferred(); }
    private:
        TFunc m_deferred;
    };

    template <class TFunc> scoped_type<TFunc> scoped(TFunc f)
    {
        return scoped_type(TFunc);
    }


    //------------------------------------------------------------------------------

    class timer
    {
    public:
        timer(std::clock_t time=std::clock()): m_time(time) {}
        
        std::clock_t time() const { return m_time; }

        friend ostream& operator<<(ostream& o, const timer& t)
        {
            return o << t.m_time;
        }
    
    private:
        std::clock_t m_time;
    };


    class printer : noncopyable
    {
    public: 
        printer(ostream& stream=std::cout):m_stream(stream) {}
        
        template <class T>
        const T& operator(const T& what)
        {
            m_stream << what;
            return *this;
        }
    private:
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
     

    //------------------------------------------------------------------------------
    // profiler
    
    // global variable
    typedef std::map<std::difference_type, profile_data> data_type;
    data_type __profile_data;

    decltype(auto) profiler(const char* id)
    {
        auto current = id-nullptr;
        auto t = timer();
        return [current, t]() { __profile_data[m_current].update(timer.time()); }
    }

    void print_stats()
    {
        for(const auto& x : __profile_data)
        {
            // print calls totaltime etc...
        }
    }



    /// This is test
    void test()
    {
        // this will create a timer and print it at the end of scope(function)
        scoped(compose(timer(), print));

        // this will calculate calls and total time of this function
        scoped(profiler(__func__));

        // this will print finished func name after function is finished
        scoped([](){ cout << 'finished: ' << __func__ << endl; });


        auto wrapper = [](auto what){ 
            cout << "started: " << what << endl;
            return [](){ cout << "finished: " << what << endl; }
        }

        // this will print start function now and after finis will print fincished function
        scoped(wrapper);

        /// your code code
        /// ...
    }

    //------------------------------------------------------------------------------

} // namespace limo
