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
#include <vector>
#include <list>
#include <set>

#include <cassert>
#include <string>
#include <map>          // Tests
#include <algorithm>    // std::find
#include <functional>   // std::fucntion

#include <iostream>
#include <iomanip>

// forward declarations:

//------------------------------------------------------------------------------

namespace limo { 



    class TestContext;
    

    typedef std::string Name;
    typedef std::function<TestContext*(void)>           TestContextGetter;
    typedef std::function<void (TestContextGetter&)>    TestFunction;
    typedef std::function<void(void)>                   PrepareFunction;
    typedef std::map<Name, TestFunction>                Tests;

    
    struct TestSettings
    {
        TestSettings(Name name, TestContext* suite): m_name(name), m_context(suite) {}
        
        TestSettings& operator<<(TestFunction test) {
            m_test = test;
            return *this;
        }

        Name            m_name;
        TestContext*    m_context;
        TestFunction    m_test;
    };


    struct Run
    {
        const char* expected;
        const char* recieved;
        const char* file;
        const int line;
        const bool ok;
    };

    struct Statistics
    {
        size_t total, passed, failured, crashed;
        void reset() { total = passed = failured = crashed = 0; }

        bool is_valid() const { return total == passed + failured + crashed; }

        Statistics& operator+=(const Statistics& rhs)
        {
            total       += rhs.total;
            passed      += rhs.passed;
            failured    += rhs.failured;
            crashed     += rhs.crashed;
            return *this;
        };
    };

    inline std::ostream& operator<<(std::ostream& o, const Run& run)
    {
        return o    << run.file << ":" << run.line << ":: " 
                    << (run.ok ? "ok" : "failed") << ":    "
                    << run.expected << std::endl;
    };

    inline std::ostream& operator<<(std::ostream& o, const Statistics& stats)
    {
        return o    << " crashed " << stats.crashed
                    << ", failured " << stats.failured
                    << ", passed " << stats.passed
                    << ", total " << stats.total
                    << ".";
        assert(stats.is_valid());
    };

    class TestContext {
    public:
        static const bool verbose = true;
        
        PrepareFunction m_before;
        PrepareFunction m_after;

        Name  m_name;
        Tests m_tests;

        bool m_global;

        Statistics stats;

    public:
        TestContext(Name name, bool global=false)
        : m_name(name)
        , m_global(global)
        {
            stats.reset();

            m_before = [](){};
            m_after  = [](){};

            if(!m_global && verbose) std::cout << m_name << std::endl;
        }

        virtual bool run() { return false; }


        virtual void test(Name name, TestFunction test)
        {
            run_test(name, test, m_name+".");
        }


        void run_test(Name name, TestFunction test, Name basename)
        {

            m_before();
            
            TestContext context(basename + name);
            TestContextGetter getter = [&context]() { return &context; };
            test(getter);
            // results[fullname] = info;
            m_after();

            stats += context.stats;            
        }

    public: // outcomes
        void expect_true(const char* file, int line, const char* expected, bool ok)
        {
            stats.total++;

            Run run = {expected, ok ? "true" : "false", file, line, ok};
            // db.push_back(run);
            if(ok)
            {
                stats.passed++;
            }
            else
            {
                stats.failured++;
                std::cout << run;
            }
        }

    private:
       
    };


    class GlobalTestContext : public TestContext {
    public:
        GlobalTestContext(): TestContext("root", true) {}

        bool run()
        {
            for(const auto& test : m_tests)
            {
                run_test(test.first, test.second, "");
            }

            std::cout << stats << std::endl;
            return true;
        }

        virtual void test(Name name, TestFunction test) {
            m_tests[name] = test;
        }
    };

    struct Registrator {
        Registrator(const TestSettings& w) {
            w.m_context->test(w.m_name, w.m_test);
        }
    };

    struct To { template <class T> To(const T&) {} };

    // #define LTEST(test_name, ...) \
    //     To doreg##test_name = \
    //         get_ltest_context()->add(#test_name)
    //         limo::TestSettings(#test_name,  get_ltest_context()) << \
    //         [__VA_ARGS__](limo::TestContextGetter& get_ltest_context) mutable -> void

    

    #define LTEST(test_name, ...) \
        limo::Registrator ltest_ ## test_name = \
            limo::TestSettings(#test_name,  get_ltest_context()) << \
            [__VA_ARGS__](limo::TestContextGetter& get_ltest_context) mutable -> void

    

    #define LBEFORE limo_context__.m_before = [&]()
    #define LAFTER  limo_context__.m_after  = [&]()


    // Unary
    #define EXPECT_TRUE(expr)       get_ltest_context()->expect_true(__FILE__, __LINE__, #expr, expr)
    #define EXPECT_FALSE(expr)      EXPECT_TRUE(!(expr))

    // Binary
    #define EXPECT_EQ(expr1, expr2) EXPECT_TRUE((expr1) == (expr2))
    #define EXPECT_NE(expr1, expr2) EXPECT_TRUE((expr1) != (expr2))
    #define EXPECT_LT(expr1, expr2) EXPECT_TRUE((expr1) < (expr2))
    #define EXPECT_LE(expr1, expr2) EXPECT_TRUE((expr1) <= (expr2))
    #define EXPECT_GT(expr1, expr2) EXPECT_TRUE((expr1) > (expr2))
    #define EXPECT_GE(expr1, expr2) EXPECT_TRUE((expr1) >= (expr2))

    // Complex
    template <class T1, class T2>
    bool in(const std::initializer_list<T1>& s, const T2& x) {
        return std::find(s.begin(), s.end(), x) != s.end();
    }
    ////////////////////////////////////////////////////////////////////////////////////////


} // namespace limo

//------------------------------------------------------------------------------
// globals

inline limo::TestContext* get_ltest_context() {
    static limo::GlobalTestContext context;
    return &context;
}

//------------------------------------------------------------------------------
