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



    class LocalTestContext;
    

    typedef std::string Name;
    typedef std::function<LocalTestContext*(void)>       TestContextGetter;
    typedef std::function<void (TestContextGetter&)>    TestFunction;
    typedef std::function<void(void)>                   PrepareFunction;
    typedef std::map<Name, TestFunction>                Tests;




    struct Result
    {
        const char* expected;
        const char* recieved;
        const char* file;
        const int line;
        const bool ok;
        
        friend std::ostream& operator<<(std::ostream& o, const Result& result)
        {
            return o    << result.file << ":" << result.line << ":: " 
                        << (result.ok ? "ok" : "failed") << ":    "
                        << result.expected << std::endl;
        };
    };


    struct Statistics {
        size_t total, passed, failured, crashed;

        Statistics(): total(0), passed(0), failured(0), crashed(0) {}
        
        bool is_valid() const { return total == passed + failured + crashed; }

        void add_result(Result& result) {
            assert(is_valid());

            ++total;
            if(result.ok) {
                ++passed;
            } 
            else {
                ++failured;
                std::cout << result;
            }
            assert(is_valid());
        }

        void expect_true(const char* file, int line, const char* expected, bool ok) {
            Result result = {expected, ok ? "true" : "false", file, line, ok};
            add_result(result);
        }

        Statistics& operator+=(const Statistics& rhs) {
            assert(is_valid() && rhs.is_valid());
            total       += rhs.total;
            passed      += rhs.passed;
            failured    += rhs.failured;
            crashed     += rhs.crashed;
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& o, const Statistics& stats) {
            assert(stats.is_valid());
            return o    << " crashed " << stats.crashed
                        << ", failured " << stats.failured
                        << ", passed " << stats.passed
                        << ", total " << stats.total
                        << ".";
        }
    };


    class LocalTestContext {
    public:
        static const bool m_verbose = true;

        Name  m_name;
        PrepareFunction m_before;
        PrepareFunction m_after;

        Statistics stats;

    public:
        LocalTestContext(Name name)
        : m_name(name)
        , m_before([](){})
        , m_after([](){})
        {
            if(m_verbose) std::cout << m_name << std::endl;
        }

        virtual void test(Name name, TestFunction test) 
        {
            run_test(name, test, m_name+".");
        }

        void run_test(Name name, TestFunction test, Name basename) {
            m_before();
            LocalTestContext context(basename + name);
            TestContextGetter getter = [&context]() { return &context; };
            test(getter);
            stats += context.stats;
            m_after();
        }
    };


    class GlobalTestContext : public LocalTestContext {
    public:
        Tests m_tests;

        GlobalTestContext(): LocalTestContext("root") {}

        void test(Name name, TestFunction test) {
            m_tests[name] = test;
        }

        int run() {
            for(const auto& test : m_tests) {
                run_test(test.first, test.second, "");
            }

            std::cout << stats << std::endl;
            return 0;
        }
        
    };


    struct TestSettings {
        TestSettings(Name name, LocalTestContext* suite): m_name(name), m_context(suite) {}
        
        TestSettings& operator<<(TestFunction test) {
            m_test = test;
            return *this;
        }

        Name                m_name;
        LocalTestContext*    m_context;
        TestFunction        m_test;
    };

    struct Registrator {
        Registrator(const TestSettings& w) {
            w.m_context->test(w.m_name, w.m_test);
        }
    };

    // First arg is a test name, second(optional) arg is a lambda capture list in parentheses
    #define LTEST(...) LTEST_IMPL(__VA_ARGS__, (), )
    #define LTEST_IMPL(test_name, capture, ...) \
        limo::Registrator ltest_ ## test_name = \
            limo::TestSettings(#test_name,  get_ltest_context()) << \
            [OPEN_PARENTHESES capture](limo::TestContextGetter& get_ltest_context) mutable -> void

    // Preprocessor utils
    #define OPEN_PARENTHESES(...) __VA_ARGS__

    #define LBEFORE get_ltest_context()->m_before = [&]()
    #define LAFTER  get_ltest_context()->m_after  = [&]()

    // Unary
    #define EXPECT_TRUE(expr)       get_ltest_context()->stats.expect_true(__FILE__, __LINE__, #expr, expr)
    #define EXPECT_FALSE(expr)      EXPECT_TRUE(!(expr))
    #define EXPECT(expr)            EXPECT_TRUE(expr)

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

inline limo::GlobalTestContext* get_ltest_context() {
    static limo::GlobalTestContext context;
    return &context;
}

//------------------------------------------------------------------------------
