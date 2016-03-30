#pragma once

#include <ofstream>
#include <ctime>

namespace limo
{
    class logger : logger_base
    {
    public:
        logger(ostream& os)
        : m_ostream(os) {}

        static logger& get_base()
        {
            static logger(base
        }

    public:
        logger(const char local_scope[], options_type options)
        : m_parent(logger::get_base())
        , m_scope(local_scope)
        {

        }

        logger(logger& parent, options_type options)
        : m_parent(parent)
        , m_scope("")
        {

        }


        void print_preamble override



        template <typename T>
        ostream& operator<<(const T& data)
        {
            return m_stream << data;
        }


        line(const char file[], int line)
        {
            return finisher(this);
        }

    private:

        const char* m_scope;
    };


    class Line
    {
    public:
        Line(logger& logger, const char file[], int line): m_logger(logger) 
        {
            logger << ""
        }
        ~Line() { m_logger << }
    };


prolog(line(base, 123))


} // limo