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
#include <chrono>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>


// forward declarations:

//------------------------------------------------------------------------------

#if defined(LIMO_DISABLE_PROFILER)
    #define limo_profile_scope(id)  (void)(0)
#else
    #define limo_profile_scope(id)  \
        static limo::profile::details::Info& limo_scope_profile_info = limo::profile::details::DB::create_info(id); \
        limo::profile::details::InfoUpdater limo_scope_profile_updater(limo_scope_profile_info) 
#endif    

#define limo_profile_function   limo_profile_scope(__func__)

//------------------------------------------------------------------------------

namespace limo
{
    namespace profile
    {
        typedef std::chrono::high_resolution_clock  clock_type;
        
        namespace details
        {
            struct Info
            {
                const char* id;
                size_t      calls;
                clock_type::duration   total_time;
                
                Info(const char* id_): id(id_), calls(0), total_time(0) {}
                
                template <class TDuration>
                typename TDuration::rep total() const 
                { 
                    return std::chrono::duration_cast<TDuration>(total_time).count();
                }

                template <class TDuration>
                typename TDuration::rep average() const 
                { 
                    limo_assert(calls > 0, "should never happen");
                    return std::chrono::duration_cast<TDuration>(total_time/calls).count();
                }

                double relative(const clock_type::duration& time) const 
                {
                    limo_assert(time.count() > 0, "expeced nonzero time");

                    auto preserve_radix = 1000;
                    auto enu  = 100 * preserve_radix * total_time.count();
                    auto denom = time.count();
                    return  double(enu / denom) / preserve_radix;
                }
            };


            class DB
            {
            public:
                static inline DB& instance()
                {
                    static DB instance;
                    return instance;
                }

                static Info& create_info(const char* id)
                {
                    DB& db = instance();

                    limo_assert(db.m_data.size() < m_max_objects, "too many objects to profile");

                    db.m_data.emplace_back(Info(id));
                    return db.m_data.back();
                }

                static clock_type::duration time()
                {
                    return clock_type::now() - instance().m_start;
                }

                static std::vector<Info> results(size_t max_lines)
                {
                    auto greater_time = [](const Info& x, const Info& y) { 
                        return x.total_time > y.total_time; 
                    };

                    auto db = instance().m_data;
                    sort(db.begin(), db.end(), greater_time);

                    auto up = std::min(max_lines, db.size());
                    db.erase(db.begin()+up, db.end());
                    return db;
                }

            private:
                DB()
                : m_start(clock_type::now())
                {
                    m_data.reserve(m_max_objects);
                }

                static const size_t m_max_objects = 1000;
                const clock_type::time_point m_start;
                std::vector<Info> m_data;
            };

            

            struct InfoUpdater : limo::noncopyable
            {
                Info&       m_info;
                clock_type::time_point   m_start;

                InfoUpdater(Info& info)
                : m_info(info)
                , m_start(clock_type::now())
                {
                }

                ~InfoUpdater()
                {
                    ++(m_info.calls);
                    m_info.total_time += clock_type::now() - m_start;
                }
            };

            

            

        } // namespace details


        std::ostream& results(std::ostream& o)
        {   
            #if defined(LIMO_DISABLE_PROFILER)
                return o << "profiler disabled";
            #endif

            using namespace limo::profile::details;
            using namespace std;
            using namespace std::chrono;

            const size_t max_lines = 20;
            auto db = DB::results(max_lines);
            auto finish = DB::time();
            
            // output width
            const size_t w_name = 20; 
            const size_t w_rel = 6;
            const size_t w_time = 10;
            const size_t w_calls = 10;

            auto br = [&](){
                o   << left << ".-" << setfill('-')
                    << setw(w_name)     << "-" << "-.-"
                    << setw(w_rel)      << "-" << "-.-"
                    << setw(w_time)     << "-" << "-.-"
                    << setw(w_time)     << "-" << "-.-"
                    << setw(w_calls)    << "-" << "-.\n";
            };
            size_t line = 0;
            br();
            o   << setfill(' ') << left << "| "
                << setw(w_name)       << "function"   << " | "
                << setw(w_rel)      <<  "% time"    << " | "
                << setw(w_time)     <<  "time msec"      << " | "
                << setw(w_time)     <<  "average"   << " | "
                << setw(w_calls)    <<  "calls"     << " |\n";
            br();
            for(const auto& info : db)
            {
                o   << setfill(' ') << "| "
                    << left << setw(w_name)   << info.id << " | "
                    << right << setw(w_rel) << fixed << setprecision(2) 
                        << info.relative(finish) << " | "
                    << right << setw(w_time) << info.total<milliseconds>() << " | "
                    << right << setw(w_time) << info.average<milliseconds>()  << " | "
                    << right << setw(w_calls) << info.calls  << " |\n";
                br();
            }
            return o;
        }

    } // namespace profile

        
} // namespace limo

//------------------------------------------------------------------------------

