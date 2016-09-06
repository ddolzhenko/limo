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

#define limo_profile_scope(id)  \
        static auto& limo_scope_profile_info = limo::profile::details::new_info(id); \
        limo::profile::details::InfoUpdater limo_scope_profile_updater(limo_scope_profile_info)

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
                Info(const char* id_): calls(0), total_time(0), id(id_) {}
                const char* id;
                size_t      calls;
                clock_type::duration   total_time;
                
                bool operator<(const Info& other) const 
                { 
                    return total_time < other.total_time; 
                }

                clock_type::duration average() const 
                { 
                    return calls ? total_time/calls : clock_type::duration(0); 
                }

                double persent_of(const clock_type::duration& time) const {
                    return std::abs(100.0* double(total_time.count()) / double(time.count()));
                }


            };

            struct InfoUpdater : limo::noncopyable
            {
                Info&       m_info;
                clock_type::time_point   m_start;

                InfoUpdater(Info& info)
                : m_info(info)
                , m_start(clock_type::now())
                {}

                ~InfoUpdater()
                {
                    ++m_info.calls;
                    m_info.total_time += clock_type::now() - m_start;
                }
            };

            typedef std::vector<Info> ProfilerDB;
            inline  ProfilerDB* get_db()
            {
                static ProfilerDB db;
                return &db;
            }

            inline Info& new_info(const char* id)
            {
                ProfilerDB* db = get_db();
                db->emplace_back(Info(id));
                return db->back();
            }
        


            


        } // namespace details


        std::ostream& results(std::ostream& o)
        {   
            using namespace limo::profile::details;
            using namespace std;

            clock_type::duration finish = clock_type::now() - clock_type::time_point();
            
            const size_t id_width = 20;
            const size_t rel_width = 6;
            const size_t time_width = 10;
            const size_t calls_width = 10;
            const size_t total_width = id_width + rel_width + 2*time_width + calls_width + 12;
            const size_t output_lines = 20;

            ProfilerDB db = (*get_db());

            std::sort(db.begin(), db.end());

            auto repr = [](const clock_type::duration& d) {
                return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
            };

            auto br = [&](){
                o   << left << "+-" << setfill('-')
                    << setw(id_width)       << "-" << "-||-"
                    << setw(rel_width)      << "-" << "-|-"
                    << setw(time_width)     << "-" << "-|-"
                    << setw(time_width)     << "-" << "-|-"
                    << setw(calls_width)    << "-" << "-|\n";
            };

            size_t line = 0;
            br();
            o   << setfill(' ') << left << "| "
                << setw(id_width)       << "function"   << " || "
                << setw(rel_width)      <<  "% time"    << " | "
                << setw(time_width)     <<  "time msec"      << " | "
                << setw(time_width)     <<  "average"   << " | "
                << setw(calls_width)    <<  "calls"     << " |\n";
            br();
            for(const auto& info : db)
            {
                o   << setfill(' ') << "| "
                    << left << setw(id_width)   << info.id << " || "
                    << right << setw(rel_width) << fixed << setprecision(2) << info.persent_of(finish) << " | "
                    << right << setw(time_width) << repr(info.total_time) << " | "
                    << right << setw(time_width) << repr(info.average())  << " | "
                    << right << setw(calls_width) << info.calls  << " |\n";
                br();
                if(line++ > output_lines)
                    break;
            
            }
        }

    } // namespace profile

        
} // namespace limo

//------------------------------------------------------------------------------

