// Simple timer class to keep track of simulation ticks
#pragma once

#include <iostream>
#include <chrono>

class Timer
{
public:

    Timer() : m_beg(Clock::now()) {}

    void reset() { m_beg = Clock::now(); }

    // Return elapsed time in milliseconds with high precision or rounded
    double elapsed() const { return std::chrono::duration_cast<Milliseconds>(Clock::now() - m_beg).count(); }
    int roundElapsed() const { return static_cast<int>( std::chrono::duration_cast<Milliseconds>(Clock::now() - m_beg).count() ); }

    void display() const { std::cout << "Time taken: " << roundElapsed() << " [ms]\n"; }

private:
    using Clock = std::chrono::steady_clock;
    using Milliseconds = std::chrono::duration<double, std::milli>;

    std::chrono::time_point<Clock> m_beg;
};