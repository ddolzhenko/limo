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
#include <cassert>
#include <iostream>

// forward declarations:


//------------------------------------------------------------------------------
// MACRO

#if !defined(LIMO_FORCE_ASSERTIONS) && defined(NDEBUG)
    #define limo_assert(...)    (void)(0)
#else
    #define limo_assert(cond, ...) \
        limo::details::asserter(cond, #cond, "assertion", __VA_ARGS__, __FILE__, __LINE__)
#endif

#define limo_contract(cond, ...)    limo_assert(cond, __VA_ARGS__)
#define limo_scope_invariant(cond) limo_assert(cond, "scope invariant")

//------------------------------------------------------------------------------

namespace limo
{
    namespace details 
    {
        void asserter(
            bool cond, 
            const char* cond_text, 
            const char* type,
            const char* msg, 
            const char* file, 
            int line)
        {
            if(!cond) {
                std::cerr 
                    << file << ":" << line << ":: failed: " 
                    << type << " " << cond_text 
                    << "\n\t" << msg << std::endl;
                assert(0);
            }
        }
    }    

} // namespace limo

//------------------------------------------------------------------------------
