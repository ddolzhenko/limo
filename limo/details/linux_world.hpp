#pragma once

#include <unistd.h>
#include <sys/stat.h>   // umask

namespace limo { namespace details

    class linux_world
    {
    public:
        typedef linux_home home;
        typedef linux_daemon_name daemon_name;

        home big_brother(const char* name);

    public:
        static daemon_name spawn_daemon_at(home);

    private:
        static bool fork_process(pid_t& pid);
        static bool init_log(home env);
        static bool create_sid(pid_t& sid);
        static bool chdir(home env);
        static bool close_streams();
    };


    class linux_world
    {
    public:
        typedef linux_home home;
        typedef linux_daemon_name daemon_name;

        home big_brother(const char* name) { return home().add_big_brother(name); }

    public:

        static bool fork_process(pid_t& pid)
        {
            auto pid = fork();
            if(pid < 0)
            {
                return false;
            }
            if(pid > 0)
            {
                // release caller
                std::exit(EXIT_SUCCESS);
            }
            return true;   
        }

        static bool init_log(home env)
        {
            // Change the file mode mask
            umask(0);

            // logile
            env.init_log();
            logger lerror("ERROR at: act of creation");
            // log.open("dfaemon.log");
        }

        static bool create_sid(sid)
        {
            auto sid = setsid();
            if (sid < 0) 
            {
                lerror << "sid creation failure, sid = " << sid;
                return daemon_name(pid, sid);
            }
        }

        static bool chdir(home env)
        {
            if (env.chdir() < 0) 
            {
                lerror << "can't chdir";
                exit(EXIT_FAILURE);
            }
        }

        static bool close_streams()
        {
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            return true;
        }


        static daemon_name spawn_daemon_at(home env)
        {
            pid_t pid = -1;
            pid_t sid = -1;
            bool ok = 
                fork_process(pid) &&
                init_log(env) &&
                create_sid(sid) &&
                chdir(env) &&
                close_streams();

            lerror.on(!ok) << "spawning failed for some reason";
            return daemon_name(pid, sid);
        }

    };

}

//////////////////


namespace limo
{
    class linux_home
    {
    public:
        linux_home(const char* working_dir = 0)
        : m_working_dir(working_dir)
        , m_log_file_name(nullptr)

        linux_home& operator+=(const linux_home& that)
        {
            this->m_working_dir = that.m_working_dir ? that.m_working_dir : this->m_working_dir;
            this->m_log_file_name = that.m_log_file_name ? that.m_log_file_name : this->m_log_file_name;
            return *this;
        }


        linux_home& add_big_brother(const char* name)
        {
            m_log_file_name = name;
            return *this;
        }

    private:
        friend class linux_world;

        const char* m_working_dir;
        const char* m_log_file_name;
    };

    class linux_daemon_name
    {
    public:
        typedef void life_type();
    public:

        linux_daemon_name(pid_t pid, pid_t sid): m_pid(pid), m_sid(sid) {}

        bool is_alive() const { return (m_pid > 0) && (m_sid > 0); }

        int live(life_type life)
        {
            if(this->is_alive())
            {
                for(auto i = 0; i < 10; ++i)
                {
                    life();
                    sleep(1u);
                }
                return 0;
            }

            return -1;
        }

    private:
        pid_t m_pid;
        pid_t m_sid;
    };

    
} // limo
