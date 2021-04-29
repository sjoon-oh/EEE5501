#ifndef __STOPWATCH_H__
#define __STOPWATCH_H__

#include <sys/time.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

// Time units: seconds, milliseconds, and microseconds
enum stopwatch_unit { none = 0, sec, msec, usec };

class stopwatch_t {
public:
    stopwatch_t() : elapsed_time(0.0) { }
    // Start the timer.
    void start() { gettimeofday(&start_time, 0); }
    // Stop the timer.
    void stop()  { gettimeofday(&end_time,   0); }
    // Reset the stopwatch.
    void reset() {
        elapsed_time = 0.0;
        memset(&start_time, 0, sizeof(timeval));
        memset(&end_time,   0, sizeof(timeval));
    }
    // Display the elapsed time.
    void display(stopwatch_unit m_stopwatch_unit = none) {
        elapsed_time += ((end_time.tv_sec  - start_time.tv_sec ) * 1e3 +
                         (end_time.tv_usec - start_time.tv_usec) / 1e3);
        if(m_stopwatch_unit == none) {
                 if(elapsed_time > 1000.0) { m_stopwatch_unit = sec;  }
            else if(elapsed_time < 0.0001) { m_stopwatch_unit = usec; }
            else                           { m_stopwatch_unit = msec; }
        }
        std::cout << "Elapsed time = ";
        switch(m_stopwatch_unit) {
            case sec:  { std::cout << elapsed_time / 1e3 << " sec"  << std::endl; break; }
            case usec: { std::cout << elapsed_time * 1e3 << " usec" << std::endl; break; }
            default:   { std::cout << elapsed_time       << " msec" << std::endl; break; }
        }
    }

private:
    double  elapsed_time;
    timeval start_time, end_time;
};

#endif

