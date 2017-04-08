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

namespace limo { namespace test {

    class   Context;
    struct  Result;
    struct  Statistics;
    
    typedef std::string Name;
    typedef std::function<Context*(void)>           ContextGetter;
    typedef std::function<void (ContextGetter&)>    TestFunction;
    typedef std::function<void(void)>               PrepareFunction;
    typedef std::function<void(const Result&)>      ResultHandler;
    typedef std::function<void(const Statistics&)>  FinalHandler;
    

    struct Result {
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
        
        bool is_valid() const { return total == passed + failured + crashed; }
        
        void add_result(const Result& result) {
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


    struct Test {
        const char*     name;
        Context*        parent;
        TestFunction    runner;
    };


    class Context {
    public:
        Name            name;
        PrepareFunction before, after;
        Statistics      stats;
    
        Context(Name name_ = "root")
        : name(name_), before([](){}), after([](){}), stats({})
        {}

        virtual void test(const Test& test) {
            run_test(test, name+".");
        }

        void run_test(const Test& test, Name basename = "") {
            // on_test_start
            before();
            Context context(basename + test.name);
            ContextGetter getter = [&context]() { return &context; };
            test.runner(getter);
            stats += context.stats;
            after();
        }
    };


    class GlobalContext : public Context {
    public:
        
        virtual void test(const Test& test) override {
            m_tests.push_back(test);
        }

        int run() {
            for(const auto& test : m_tests) {
                run_test(test);
            }

            std::cout << stats << std::endl;
            return 0;
        }
        
    private:
        std::vector<Test> m_tests;
    };


    int operator<<(Test test, TestFunction func) { 
        test.runner = func; 
        test.parent->test(test);
        return 0;
    }
 
    struct Dummy {
        template <class T> Dummy(const T&) {}
    };

    #define LTEST(test_name, ...) \
        limo::test::Dummy ltest_ ## test_name = \
            limo::test::Test{#test_name, limo_test_context()} << \
            [__VA_ARGS__](limo::test::ContextGetter& limo_test_context) -> void

    #define LBEFORE limo_test_context()->before = [&]() -> void
    #define LAFTER  limo_test_context()->after  = [&]() -> void

    // Unary
    #define EXPECT_TRUE(expr)       limo_test_context()->stats.expect_true(__FILE__, __LINE__, #expr, expr)
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

    // interface:
    // void set_printer()


} // namespace test
} // namespace limo

//------------------------------------------------------------------------------
// globals

inline limo::test::GlobalContext* limo_test_context() {
    static limo::test::GlobalContext context;
    return &context;
}

//------------------------------------------------------------------------------
