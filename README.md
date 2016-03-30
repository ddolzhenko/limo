limo
====

light immutable objects for c++. Immutable strings, Char classes Ranges and other stl extensions


# Test framework requirements

* Super easy learning barrier!
* Minimal start
    * Minimal setup:
        * Include-only (no test framework compilation required)
        * as small number of files as possible
        * no non standard dependencies
        * support older compilers?
    * Minimal time to start:
        * just include testt_framework.hpp and start writing test
        * not reuired to write main() yourself, just tests

* Set of test helpers
    * basic
        * equals, neq, less greter etc.
        * float ones (abs(x-y) < eps)
    * exceptions
        * catch: EXPECT_EXCEPTION(test, exception_type?);
    * Mock testing
    * static
        * any way to test wheter something can be compiled (test static_assert)

* Perfomance
    * possibility to measure perfomance

* Grouping
    * Suite/Fixture/Test
    * Should be opional
    * Easy to group existing tests

* features(optional)
    * report generation
    * gui?
    * planner
    * ci systems integration


# Candidates

## cppunit
   
* learning problem in bad tutorial (fixable)
* FAIL: minimal start
* FAIL: includeonly

## gtest

* FAIL: includeonly


## cxxunit