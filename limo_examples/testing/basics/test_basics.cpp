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

// #include "test_basics.hpp"

//------------------------------------------------------------------------------

// include local:
#include "limo/test_main.hpp"

// include std:

// forward declarations:

// limo plan

// - ltest
// - str + strlib
// - cc
// - cc and regex iterators
// - stream adaptors

// - ranges
// - range.algo().algo()
// - range concepts

// - algorithms

// - scopeds

// - functional programming support

// - ds
// - ds.flat_map

// - advanced ds:
// - cache + strategies
// - trie

// - multithreading:
// - universal deamon





//------------------------------------------------------------------------------

// + all macroses
// + __file__ ___line__
// + setup/teardown  anyway
// - print calculated values (if possible)
// + statistics
// + includeonly
// - context:     CONTEXT(sort, {selection_sort, naive_sort})
// - select tests to run test_context__.run("search.*")
// - separate printers and config
// - assert marcroses

// 0.3
// - base test example
// - test example with 2 algoritms

// 0.5: 



int my_sqare(int x)
{
    if (x == 3)
        return 3;
    return x*x;
}


LTEST(lambda_mutabelnost) {
    using namespace std;

    int x = 42;

    auto foo = [x]() mutable {
        x = 13;
    };

    EXPECT_EQ(x, 42);

    x = 128;
    EXPECT_EQ(x, 128);

    foo();
    EXPECT_EQ(x, 128);
};

LTEST(my_sqare) {

    using namespace limo::test;

    EXPECT_TRUE(2==2);
    EXPECT_FALSE(2==3);

    EXPECT_TRUE(2==2);
    
    EXPECT_TRUE(in({1, 2} , 2));

    EXPECT_EQ(1, 1);

    // auto my_sqare = [](int x) { return x * x; };
    
    LTEST(degenarated) {
        EXPECT_EQ(0, my_sqare(0));
    };
    LTEST(trivial) {
        EXPECT_EQ(1, my_sqare(1));
    };
        
    LTEST(even) {
        LTEST(positive) {
            EXPECT_EQ(5, my_sqare(2));  
            EXPECT_EQ(16, my_sqare(4));  
        };
        LTEST(negative) {
            EXPECT_EQ(4, my_sqare(-2));  
            EXPECT_EQ(16, my_sqare(-4));  
        };
    };


    LTEST(odd) {
        LTEST(positive) {
            // EXPECT_EQ(9, my_sqare(3));  
            EXPECT_EQ(25, my_sqare(5));  
        };
        LTEST(negative) {
            EXPECT_EQ(9, my_sqare(-3));  
            EXPECT_EQ(25, my_sqare(-5));  
        };
    };
};