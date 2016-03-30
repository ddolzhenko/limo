#include "limo/minimal.hpp"
#include <thread>

void limo::sleep
{
    std::this_thread::sleep_for (std::chrono::seconds(sec));
}
