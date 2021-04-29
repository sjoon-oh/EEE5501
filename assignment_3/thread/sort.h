#ifndef __SORT_H__
#define __SORT_H__

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <new>
#include <thread>

// Added libraries
#include <iostream>
#include <vector>

// For sync
#include <chrono>
#include <mutex>
#include <condition_variable>

template <typename T>
void sort(T *array, const size_t num_data, const unsigned num_threads) {
    /* Assignment */

    // The array is integer type.
    // The number of data is 4000000
    T* t_arr = new T[num_data]();

    // Thread related
    std::vector<std::thread> worker_list(num_threads);
    
    // Reduction Helper
    std::vector<bool> is_done(num_threads, false);
    std::vector<int> red_step(num_threads, num_threads / 2);
    
    std::mutex mtx;
    std::condition_variable cv;
    
    // temp
    std::mutex out;

    worker_list.reserve(num_threads); // Reserve size

    // Function definition
    std::function<void(const unsigned, const unsigned)> 
        work_fn = [&](
                const unsigned tidx, // Thread index
                const unsigned num_data // Total # of elements
            ) mutable {
            






            
            
            
            while(1) {






                // Put reduction here.
                
                mtx.lock();
                if (int(signed(tidx) - red_step[tidx]) >= 0) { // if you are upper half,
                    is_done[tidx] = true;
                    mtx.unlock(); // Unlock!
                    
                    cv.notify_all(); // Wake all up!
                    break;
                }
                else { // Sleep. It's not your turn.
                    mtx.unlock(); // First unlock

                    std::unique_lock<std::mutex> lk_cv(mtx);
                    cv.wait(lk_cv, [&]{ return is_done[tidx + red_step[tidx]]; });

                    red_step[tidx] /= 2;
                }
            }
        };




    // Generate threads
    for (unsigned index = 0; index < num_threads; index++) {

        worker_list.emplace_back(
            std::thread(work_fn, index, num_data)
        );
    }
    
    // Wait until all are finished.
    for (auto& thread : worker_list) 
        if (thread.joinable()) thread.join(); 

    delete[] t_arr; // Delete
}

#endif

