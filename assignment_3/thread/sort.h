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
// #include <iterator>

// For sync
#include <mutex>
#include <condition_variable>

template <typename T>
void sort(T *array, const size_t num_data, const unsigned num_threads) {
    // The array is integer type (T).
    // The number of data is 4000000
    T* sorted_arr = new T[num_data];

    // Thread related values.
    // wkr_list stores the thread objects,
    // mtx and cv are used for synchronizing multiple threads, 
    // especially when merging operation.
    struct t_info {
        bool is_active = true; // Is thread active?

        unsigned steps = 0; // Index value to jump to execute gather.
        unsigned start_idx = 0; // Starting index of target range
        unsigned end_idx = 0; // Ending index of target range.
    };

    std::vector<struct t_info> wkr_stat(num_threads); // Worker Status
    for (unsigned index = 0; index < num_threads; index++)
        wkr_stat[index].steps = !bool(index % 2);
    // For instance, the even index waits for the very next tidx at gather.
    // At first layer, tidx 0 waits for 1, tidx 4 waits for 5.
    // Even tidx waits to operate gather_fn, while the others just kills themselves.

    std::vector<std::thread> wkr_list(num_threads);
    wkr_list.reserve(num_threads); // Reserve size

    // Locks for thread control variable (is_active, steps) 
    // and element arrays (array, merged_array)
    std::mutex t_mtx;
    std::condition_variable cv; // For wait

#define T_LOCK      t_mtx.lock();
#define T_UNLOCK    t_mtx.unlock();

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
                } // Break it into half, and align them to the new array: sorted_arr

                if (left_idx > mid_idx) // Case when right-hand side have extras.
                    std::copy( // Defined in <algorithm>
                        sorted_arr + start_idx, // begin()
                        sorted_arr + sort_idx, // end()
                        array + start_idx); // destination begin()
                    // In this case, the rest of upper part of the sub-array has been already sorted.
                    // Thus, single copy operation is needed that targets the whole partition.

                else { // Case when left-hand side have extras.
                    std::copy( // Move within array
                        array + left_idx,
                        array + mid_idx + 1,
                        array + sort_idx);

                    std::copy( // Overwrite the rest.
                        sorted_arr + start_idx, 
                        sorted_arr + sort_idx, 
                        array + start_idx); 
                }

                // In this case, aligned part exists in the lower-half part of the sub-array.
                // Thus, two operation should be used, the first that moves lower-aligned part up,
                // and the second that copies sorted partition through while-scope done at 
                // the top of this function.
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
                    msort_fn(start_idx, mid_idx); // Sort left half
                    msort_fn(mid_idx + 1, end_idx); // Sort right half

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

                wkr_stat[tidx].start_idx = tidx * idx_step;
                wkr_stat[tidx].end_idx = tidx == num_threads - 1 ?
                    num_data - 1 : (tidx + 1) * idx_step - 1;

                int neighbor_start_idx; // Neighbor range starting index becomes the new mid_idx.

                // First, each thread sorts partial array of allocated range to itself.
                // The merge sort algorithm is used in this phase.
                msort_fn(
                    wkr_stat[tidx].start_idx, 
                    wkr_stat[tidx].end_idx);
            
            for (; ; ) {

                if (wkr_stat[tidx].steps == 0 || 
                    wkr_stat[tidx].steps == num_threads) {
                    // Two cases:
                    // Case when the thread has no jump steps: 
                    //  in other words, the thread does not have to wait for another thread
                    // Case when the thread has the step of num_threads.
                    //  This happens when the last thread (specifically thread tidx = 0)
                    //  holds the jump step of num_threads, which does not exist.
                    //  Thus, the second condition is just for thread tidx = 0.
                    wkr_stat[tidx].is_active = false;
                    wkr_stat[tidx].steps = 0;
                    
                    cv.notify_all(); // Wake all up!
                    // Each should wake every threads up when they are killed.
                    break;

                } else { // Sleep. It's not your turn.

                    { // Scoped. For automatic lock release.
                        std::unique_lock<std::mutex> lk_cv(t_mtx);
                        cv.wait(lk_cv, 
                            [&]{ return !wkr_stat[tidx + wkr_stat[tidx].steps].is_active; });
                    }

                    T_LOCK { // Lock for the use of steps[]
                        neighbor_start_idx = wkr_stat[tidx + wkr_stat[tidx].steps].start_idx;
                        wkr_stat[tidx].end_idx = wkr_stat[tidx + wkr_stat[tidx].steps].end_idx;
                    } T_UNLOCK

                    // Outside of the above scope does not require lock,
                    // since each thread only reads member 'end_idx' info of other threads.

                    // Update the status
                    if (tidx % (wkr_stat[tidx].steps * 4)) wkr_stat[tidx].steps = 0;
                    else wkr_stat[tidx].steps *= 2;
                    
                    // Gather the aligned sub-array that has been given up by neighbor thread.
                    gather_fn(
                        wkr_stat[tidx].start_idx, 
                        neighbor_start_idx - 1,
                        wkr_stat[tidx].end_idx);
                }
            }
        };

    // Hey thread slaves, get to work!
    for (unsigned index = 0; index < num_threads; index++) {
        wkr_list.emplace_back(
            std::thread(work_fn, index, num_data)
        ); // Give work_fn with each thread index and number of data.
        // These arguments are constants.
    }
    
    // Wait until all are finished.
    for (auto& thread : wkr_list) if (thread.joinable()) thread.join();
    delete[] sorted_arr; // Wrap up
}

#endif