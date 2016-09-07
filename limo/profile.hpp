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
#include <typeinfo>
#include <string>

#include <sstream>

// forward declarations:
                using namespace std;

//------------------------------------------------------------------------------
// #define limo_profile_scope(id)  (void)(0)

#define limo_profile_scope(id)  \
    std::cout << "<macro checkpoint: " << id << " addr: " << (void*)(id) << std::endl; \
    static const char* limo_scope_profile_id = id; \
    static limo::profile::details::Info* limo_scope_profile_info = limo::profile::details::new_info(limo_scope_profile_id); \
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
                const char* id;
                size_t      calls;
                clock_type::duration   total_time;
                
                Info(const char* id_): id(id_), calls(0), total_time(0) {}

                bool operator<(const Info& other) const 
                { 
                    return total_time < other.total_time; 
                }

                clock_type::duration average() const 
                { 
                    return clock_type::duration(1); 
                    // return calls ? total_time/calls : clock_type::duration(0); 
                }

                double persent_of(const clock_type::duration& time) const 
                {
                    using namespace std;
                    using namespace std::chrono;
                    auto digits = 1000;
                    auto enu  = 100 * digits * total_time.count();
                    auto denom = time.count();
                    assert(denom > 0);
                    
                    // auto ratio = double(enu / denom) / digits;
                    return enu;
                }


                friend std::ostream& operator<<(std::ostream& o, const Info& info)
                {
                    using namespace std;
                    // uint64_t cnt = size_t(info.total_time.count());

                    // std::cout << cnt << std::endl;

                    ostringstream str;

                    // cout << &(info) << endl;
                    // cout << &(info) << ", " << &(info.id) << ", " << (void*)(info.id) << endl;
                    str << info.id;

                    return o
                            << "{id : " << info.id 
                            << ", calls : " << info.calls
                            << ", time : " << info.total_time.count() << "}";
                }

            };

            
            class DB 
            {

            };


            typedef std::vector<Info> ProfilerDB;
            inline  ProfilerDB* get_db()
            {
                static ProfilerDB db;
                db.reserve(1000);
                // std::cout << "profiler db status: " << db.size() << std::endl;
                return &db;
            }

            struct InfoUpdater : limo::noncopyable
            {
                Info*       m_info;
                clock_type::time_point   m_start;

                InfoUpdater(Info* info)
                : m_info(info)
                , m_start(clock_type::now())
                {
                    // cout << "+++updater created" << flush; dump();
                }

                ~InfoUpdater()
                {
                    // std::cout << ">>>---updater destroy started " << flush; dump();
                    ++(m_info->calls);
                    m_info->total_time += clock_type::now() - m_start;
                    // std::cout << "<<<---updater destroy finished " << flush; dump();
                }

                void dump()
                {
                    auto db = *get_db();
                    cout << "\nstate: ";
                    for(auto& i : db)
                    {
                        cout <<"[" << (void*)(&i) << ", ";
                        cout << (void*)(i.id) << "], ";
                    }

                    cout << endl;
                    auto info = m_info;
                    cout << info << ", addr(id):" << flush;
                    cout << &(info->id) << ", addr(str): " << flush;
                    cout << (void*)(info->id) << endl << flush;;
                    cout << "\t" << "info: " << (*info) << endl;

                    
                }
            };

            

           

            Info* new_info(const char* id)
            {
                cout << "requested info: " << id << flush;
                ProfilerDB* db = get_db();
                db->push_back(Info(id));
                cout << ", object: " << db->back() << ", addr: " << (&(db->back())) <<endl;
                return &(db->back());
            }

        } // namespace details

        inline clock_type::time_point start()
        {
            static clock_type::time_point start = clock_type::now();
            return start;
        }

        clock_type::time_point run_time = clock_type::now();

        std::ostream& results(std::ostream& o)
        {   
            using namespace limo::profile::details;
            using namespace std;
            
            clock_type::duration finish = clock_type::now() - run_time;
            std::cout <<  std::chrono::duration_cast<std::chrono::microseconds>(finish).count() << endl;
            
            const size_t id_width = 20;
            const size_t rel_width = 6;
            const size_t time_width = 10;
            const size_t calls_width = 10;
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
                    << right << setw(rel_width) << fixed << setprecision(2) << info.persent_of(finish) <<" | "
                    << right << setw(time_width) << repr(info.total_time) << " | "
                    << right << setw(time_width) << repr(info.average())  << " | "
                    << right << setw(calls_width) << info.calls  << " |\n";
                br();
                if(line++ > output_lines)
                    break;
            
            }
            return o;
        }

    } // namespace profile

        
} // namespace limo

//------------------------------------------------------------------------------

