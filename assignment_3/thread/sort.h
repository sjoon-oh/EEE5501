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
#include <iomanip>
#include <vector>

// For sync
#include <mutex>
#include <condition_variable>

template <typename T>
void sort(T *array, const size_t num_data, const unsigned num_threads) {
    // The array is integer type (T).
    // The number of data is 4000000
    T* sorted_arr = new T[num_data];

    // Thread related values.
    // worker_list stores the thread objects,
    // mtx and cv are used for synchronizing multiple threads, 
    // especially when merging operation.
    std::vector<std::thread> worker_list(num_threads);
    worker_list.reserve(num_threads); // Reserve size

    // Locks for thread control variable (is_active, steps) 
    // and element arrays (array, merged_array)
    std::mutex t_mtx;
    std::condition_variable cv; // For wait

#define T_LOCK      t_mtx.lock();
#define T_UNLOCK    t_mtx.unlock();
    
    // Reduction Helper(Merge Operations)
    // Variable is_active and steps syncs the reduction operations.
    bool is_active[num_threads];
    unsigned steps[num_threads];
    
    std::fill_n(is_active, num_threads, true); // Initialize with all trues
    for (unsigned t_idx = 0; t_idx < num_threads; t_idx++)
        steps[t_idx] = !bool(t_idx % 2); // Steps are what to wait.
    // For instance, the even index waits for the very next tidx.
    // tidx 0 waits for 1, tidx 4 waits for 5.
    // Even tidx waits to operate gather_fn, while the other just kills itself.
    
    // temp
    std::mutex out;
#define LOCK out.lock();
#define UNLOCK out.unlock();

    // First function variable: gather_fn
    // - gathers separated array
    std::function<void(const int, const int, const int)>
        gather_fn = [&](
                const int start_idx,
                const int mid_idx,
                const int end_idx
            ) mutable {

                int left_idx = start_idx;
                int right_idx = mid_idx + 1;
                int sort_idx = start_idx;

                while (left_idx <= mid_idx && right_idx <= end_idx) {
                    if (array[left_idx] <= array[right_idx]) 
                        sorted_arr[sort_idx++] = array[left_idx++];
                    else sorted_arr[sort_idx++] = array[right_idx++];
                }

                if (left_idx > mid_idx)
                    for(int idx = right_idx; idx <= end_idx; idx++)
                        sorted_arr[sort_idx++] = array[idx];

                else
                    for(int idx = left_idx; idx <= mid_idx; idx++)
                        sorted_arr[sort_idx++] = array[idx];

                // std::copy(sorted_arr + start_idx, sorted_arr + end_idx, array + start_idx);
                for (int idx = start_idx; idx <= end_idx; idx++)
                    array[idx] = sorted_arr[idx];

                // LOCK
                // std::cout 
                //     << "\tgather_fn: (" << std::setw(8) << start_idx 
                //     << ", " << std::setw(8) << mid_idx
                //     << ", " << std::setw(8) << end_idx 
                //     << ")" << std::endl;

                // for(int idx = start_idx; idx < end_idx; idx++)
                //     std::cout << "\t" << *(sorted_arr + idx) << " \n";
                // std::cout << std::endl;
                // UNLOCK
        };

    // Second function variable: msort_fn 
    // - sorts the array in a given range.
    std::function<void(const int, const int)> 
        msort_fn = [&](
                const int start_idx,
                const int end_idx
            ) mutable {
                
                const int mid_idx = (start_idx + end_idx) / 2;

                if (start_idx < end_idx) {
                    msort_fn(start_idx, mid_idx);
                    msort_fn(mid_idx + 1, end_idx);

                    gather_fn(start_idx, mid_idx, end_idx);
                }                
        };

    // Third function variable: work_fn 
    // - wrapper executable function thrown to each thread.
    std::function<void(const unsigned, const unsigned)> 
        work_fn = [&](
                const unsigned tidx, // Thread index
                const unsigned num_data // Total # of elements
            ) mutable {

                const int idx_step = num_data / num_threads;
                const int idx_start = tidx * idx_step;
                int idx_end;

                if (num_data % num_threads) idx_end = num_data - 1;
                else idx_end = (tidx + 1) * idx_step - 1;
                
                msort_fn(idx_start, idx_end);
                // First, each thread sorts partial array of allocated range to itself.
                // The merge sort algorithm is used in this phase.
            
            while(1) {

                T_LOCK // Lock the variables first.

                if (steps[tidx] == 0 || 
                    steps[tidx] == num_threads) {
                    // Two cases:
                    // Case when the thread has no jump steps: 
                    //  in other words, the thread does not have to wait for another thread
                    // Case when the thread has the step of num_threads.
                    //  This happens when the last thread (specifically thread tidx = 0)
                    //  holds the jump step of num_threads, which does not exist.
                    //  Thus, the second condition is just for thread tidx = 0.

                    is_active[tidx] = false;
                    steps[tidx] = 0;
                    T_UNLOCK // Remove the lock
                    
                    cv.notify_all(); // Wake all up!
                    // Each should wake every threads up when they are killed.

                    break;

                } else { // Sleep. It's not your turn.
                    T_UNLOCK // First unlock

                    { // Scoped. For automatic lock release.
                        std::unique_lock<std::mutex> lk_cv(t_mtx);
                        cv.wait(lk_cv, [&]{ return !is_active[tidx + steps[tidx]]; });
                    }
                    
                    idx_end = (tidx + steps[tidx] * 2) * idx_step - 1;
                    // idx_end = unsigned(idx_end) + 1 == num_data ? 
                    //     idx_end : (tidx + steps[tidx] * 2) * idx_step - 1;

                    T_LOCK {
                        if (tidx % (steps[tidx] * 4)) steps[tidx] = 0;
                        else steps[tidx] *= 2;
                    } T_UNLOCK
                    
                    // Lock is not needed. Only index of its own thread id are accessed. 
                    // No one else writes it.
                    // Call the merge!
                    gather_fn(idx_start, (idx_start + idx_end) / 2, idx_end);
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

    delete[] sorted_arr;
}

#endif