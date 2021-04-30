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

    // The array is integer type.
    // The number of data is 4000000
    T* merged_arr = new T[num_data]();

    // Thread related values.
    // worker_list stores the thread objects,
    // mtx and cv are used for synchronizing multiple threads, 
    // especially when merging operation.
    std::vector<std::thread> worker_list(num_threads);
    worker_list.reserve(num_threads); // Reserve size

    // Locks for thread control variable (is_active, steps) 
    // and element arrays (array, merged_array)
    std::mutex t_mtx, a_mtx; 
    std::condition_variable cv; // For wait    
    
    // Reduction Helper(Merge Operations)
    // Variable is_active and steps syncs the reduction operations.
    bool is_active[num_threads];
    unsigned steps[num_threads];
    
    std::fill_n(is_active, num_threads, true); // Initialize with all trues
    for (unsigned t_idx = 0; t_idx < num_threads; t_idx++)
        steps[t_idx] = !bool(t_idx % 2); // Steps are what to wait.
    
    // temp
    std::mutex out;

    // First function variable: sort_fn 
    // - sorts the array in a given range.
    std::function<void(const int, const int, const int)> 
        sort_fn = [&](
                const int t_idx,
                const int start_idx,
                const int end_idx
            ) {
                out.lock();
                std::cout << "Sorted[" << t_idx << "]: (" << start_idx 
                    << ", "<< end_idx << ")" << std::endl;
                out.unlock();
        };

    // Second function variable: reduction_fn
    // - merges separated array
    std::function<void(const int, const int, const int)>
        merge_fn = [&](
                const int t_idx,
                const int start_idx,
                const int end_idx
            ) {
                out.lock();
                std::cout << "\tMerged[" << t_idx << "]: (" << start_idx << ", "
                    << end_idx << ")" << std::endl;
                out.unlock();
        };

    // Third function variable: work_fn 
    // - wrapper function for each thread.
    std::function<void(const unsigned, const unsigned)> 
        work_fn = [&](
                const unsigned tidx, // Thread index
                const unsigned num_data // Total # of elements
            ) mutable {

                int range_step = num_data / num_threads;
                int start = tidx * range_step;
                int end = (tidx + 1) * range_step - 1;

                sort_fn(tidx, start, end);

                

            
            
            while(1) {
                
                // Partial-merge
                

                // Put reduction here.
                // Each thread merges 
                
                t_mtx.lock();
                if (steps[tidx] == 0 || 
                    steps[tidx] == num_threads) { // if you are upper half,

                    is_active[tidx] = false;
                    steps[tidx] = 0;
                    t_mtx.unlock(); // Remove the lock
                    
                    cv.notify_all(); // Wake all up!
                    // Each should wake every threads up when they are killed.
                    // 

                    break;

                } else { // Sleep. It's not your turn.
                    t_mtx.unlock(); // First unlock

                    {
                        std::unique_lock<std::mutex> lk_cv(t_mtx);
                        cv.wait(lk_cv, [&]{ return !is_active[tidx + steps[tidx]]; });
                    }

                    t_mtx.lock();
                    if (steps[tidx])
                    merge_fn(
                        tidx, start,
                        (tidx + steps[tidx] + 1) * range_step - 1
                    );

                    // Update steps
                    if ( !(tidx % (steps[tidx] * 4))) steps[tidx] *= 2;
                    else steps[tidx] = 0;

                    t_mtx.unlock();

                }
            }
        };




    // Hey thread slaves, get to work!
    for (unsigned index = 0; index < num_threads; index++) {
        worker_list.emplace_back(
            std::thread(work_fn, index, num_data)
        ); // Give work_fn with each thread index and number of data.
        // These arguments are constants.
    }
    
    // Wait until all are finished.
    for (auto& thread : worker_list) if (thread.joinable()) thread.join();

    delete[] merged_arr; // Delete
}

#endif

