#pragma once


namespace limo
{
    void sleep(float seconds);
    {
        std::this_thread::sleep_for (std::chrono::seconds(sec));
    }
} // limo
