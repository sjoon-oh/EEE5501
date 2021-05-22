#ifndef __DFT_H__
#define __DFT_H__

#include <complex>
#include <cstdlib>
#include <cstring>
#include <mpi.h>
#include "abort.h"
#include "data.h"


// Author: SukJoon Oh
// 2018142216

// ** Before running! **
// Do not test this example on Arch linux.
// The AUR package MPICH automatically searches for CUDA support, 
// thus running this on Arch will keep you warning that no libcuda.so library is seen.
// This project was run and tested on the Docker ubuntu:latest container.
// Every necessary installation is written in the Dockerfile.
// This project was not tested on other Linux distributions.

// Three additional functions:
// reorder_idx_map: Generates reordered indices so that the original 
// initial x values can be reordered.
// reorder_data: Using the array generated above, it reorders the 
// original data precalculated from reorder_idx_map.
// dft1d: Discrete Fourier Transformation for a single row.

// Added.
// These are necessary
#include <cmath>
#include <utility> // For swap function.

//
// Reorder indices
void reorder_idx_map(
   unsigned* map, // Record index map
   unsigned st_idx, // Starting index of an array.
   unsigned size // Total range of reordering.
   ) {

   unsigned pmap[size]; // Temporary indices storage

   if (size == 1) ;
   else {
      unsigned idx_e = 0; // index of evens
      unsigned idx_o = size / 2; // index of odds

      for (unsigned idx = st_idx; idx < st_idx + size; idx++) {
         if (idx % 2) { pmap[idx_o] = map[idx]; idx_o++; }
         else { pmap[idx_e] = map[idx]; idx_e++; }
      }

      std::copy(pmap, pmap + size, map + st_idx);

      // Calls the rest.
      reorder_idx_map(map, st_idx, size / 2);
      reorder_idx_map(map, st_idx + size / 2, size / 2);
   }
}

//
// reorder_data
template <class T>
void reorder_data(
   T* data, // Yes, that original data.
   unsigned st_row, // Starting row index
   unsigned end_row, // Ending row index
   unsigned width, // 1024, fixed.
   unsigned* map) {

   T* init_data = new T[width]();
   for (unsigned i = st_row; i <= end_row; i++) {

      for (unsigned idx = 0; idx < width; idx++) 
         init_data[idx] = data[width * i + map[idx]]; // Perform initial value reordering.

      std::copy(init_data, init_data + width, data + width * i);
      // Overwrite
   }

   delete[] init_data; // Done reordering
}

//
// dft1d
template <typename T>
void dft1d(
   T* data, // The original data.
   T* weight, // Pre-calculated weights.
   unsigned st_idx, // Starting index 
   unsigned length, // Remaining range (length)
   unsigned width) { // 1024, fixed.

   if (length < 2) {} // Nothing to do.
   else {
      dft1d(data, weight, st_idx, length / 2, width);
      dft1d(data, weight, st_idx + length / 2, length / 2, width);

      for (unsigned k = 0; k < length / 2; k++) {

#define EVEN   data[k + st_idx]
#define ODD    data[k + st_idx + length / 2]
#define WEIGHT weight[k * width / length] // Fetch precalculated ones.

         T even = EVEN; // Temporary stores
         T odd = ODD; // Temporary stores

         EVEN = even + WEIGHT * odd;
         ODD = even - WEIGHT * odd;

#undef EVEN
#undef ODD
#undef WEIGHT
      }
   }
}

//
// Perform 2-D discrete Fourier transform (DFT).
template <typename T>
void dft2d(
   T *data, 
   const unsigned width, 
   const unsigned height,
   const int num_ranks, 
   const int rank_id) {

    /* Assignment:
       a. Perform row-wise one-dimensional DFT.
       b. Transpose the data matrix for column-wise DFT.
       c. Perform row-wise one-dimensional DFT on the transposed matrix.
       d. Transpose the data matrix back to the original orientation.
    */

   //
   // Setting row idx
   // Every procs are aware of others' workloads.
   struct Workload {
      unsigned st_idx; // starting index of row
      unsigned end_idx; // ending index of row (including)
   };

   Workload workloads[num_ranks];

   // Every proc but the last one has the same workloads(same working rows)
   // Last line might have the identical value or not.
   // Up to number of ranks.
   for (int rank = 0; rank < num_ranks; rank++) {
      workloads[rank].st_idx = rank * (height / num_ranks);
      workloads[rank].end_idx = (rank + 1) * (height / num_ranks);
   }
   workloads[num_ranks - 1].end_idx = height - 1;

   // Synchronize between procs.
   // Register receiving actions
   MPI_Request mpi_recv_request[num_ranks];

   for (int rank = 0; rank < num_ranks; rank++) {
      if (rank != rank_id)
         MPI_Irecv(
            &data[width * workloads[rank].st_idx], // Recv buffer starting point
            width * (workloads[rank].end_idx - workloads[rank].st_idx + 1), // How many?
            MPI_COMPLEX, // What type?
            rank, // From where?
            0, // Tag? Unnecessary for this project.
            MPI_COMM_WORLD, //  Communicator
            &mpi_recv_request[rank] // Req
         );
   }

   // Note that the template T is std::complex<float>.
   // Set weights first.
   T* weight = new T[width]();
   for (unsigned n = 0; n < width; n++)
      weight[n] = std::exp(T(0, -2 * M_PI * n / (width)));
      // Pre-calculation.

   //
   // Reorder the mapped indices. 
   unsigned* index_map = new unsigned[width];
   for (unsigned idx = 0; idx < width; idx++) index_map[idx] = idx;

   reorder_idx_map(index_map, 0, width);
   reorder_data(
      data, 
      workloads[rank_id].st_idx, 
      workloads[rank_id].end_idx, 
      width, 
      index_map);

   // Mission a. Perform row-wise one-dimensional DFT.
   // Run dft for only allocated rows.
   for (unsigned row = workloads[rank_id].st_idx; 
      row <= workloads[rank_id].end_idx; 
      row++)
      dft1d(data, weight, row * width, width, width);

   // Synchronize between procs.
   // 1. Send (Blocking)
   for (int rank = 0; rank < num_ranks; rank++) {
      if (rank != rank_id) // if not self,
         MPI_Send(
            &data[width * workloads[rank_id].st_idx], // Starting point
            width * (workloads[rank_id].end_idx - workloads[rank_id].st_idx + 1), // How many?
            MPI_COMPLEX, // What type?
            rank, // To where?
            0, // Tag?
            MPI_COMM_WORLD
         );
   }

   // 2. And wait for others.
   MPI_Status mpi_status[num_ranks];
   for (int rank = 0; rank < num_ranks; rank++)
      if (rank != rank_id) // if not self,
         MPI_Wait(&mpi_recv_request[rank], &mpi_status[rank]);
   //
   // 3. Frees the request. - Unnecessary, frees itself automatically.
   // Otherwise, it will warn us that the pointer is null.

   // for (int rank = 0; rank < num_ranks; rank++)
   //    if (rank != rank_id) // if not self,
   //       MPI_Request_free(&mpi_recv_request[rank]);

   // Mission b. Transpose the data matrix for column-wise DFT.
   // The saved data form is single dimensional. Thus, the 2D transpose must be:
   // data[x + width * y] <-> data[y + x * width] exchange.
   for (unsigned y = 0; y < height; y++) 
      for (unsigned x = y + 1; x < width; x++) 
         std::swap(data[x + width * y], data[y + x * width]);

   // 4. Again, register the recv request again for the second round.
   for (int rank = 0; rank < num_ranks; rank++) {
      if (rank != rank_id)
         MPI_Irecv(
            &data[width * workloads[rank].st_idx], // Recv buffer starting point
            width * (workloads[rank].end_idx - workloads[rank].st_idx + 1), // How many?
            MPI_COMPLEX, // What type?
            rank, // From where?
            0, // Tag?
            MPI_COMM_WORLD, //  Communicator
            &mpi_recv_request[rank] // Req
         );
   }

   // Rearrange the initial values again.
   // The reordered indices are the exact same ones in previous steps.
   reorder_data(
      data, 
      workloads[rank_id].st_idx, 
      workloads[rank_id].end_idx, 
      width, 
      index_map);

   // Mission c. Perform row-wise one-dimensional DFT on the transposed matrix.
   // Run dft for only allocated rows.
   for (unsigned row = workloads[rank_id].st_idx; 
      row <= workloads[rank_id].end_idx; 
      row++)
      dft1d(data, weight, row * width, width, width);
 
   // Synchronize between procs.
   // 1. Send (Blocking)
   for (int rank = 0; rank < num_ranks; rank++) {
      if (rank != rank_id) // if not self,
         MPI_Send(
            &data[width * workloads[rank_id].st_idx], // Starting point
            width * (workloads[rank_id].end_idx - workloads[rank_id].st_idx + 1), // How many?
            MPI_COMPLEX, // What type?
            rank, // To where?
            0, // Tag?
            MPI_COMM_WORLD
         );
   }

   // 2. And wait for others.
   for (int rank = 0; rank < num_ranks; rank++)
      if (rank != rank_id) // if not self,
         MPI_Wait(&mpi_recv_request[rank], &mpi_status[rank]);
   
   // Freeing request not necessary, 
   // MPI_Finalize will do it instead. Thx!

   // Mission d. Transpose the data matrix back to the original orientation.
   for (unsigned y = 0; y < height; y++) 
      for (unsigned x = y + 1; x < width; x++) 
         std::swap(data[x + width * y], data[y + x * width]);
   
   delete[] weight;
   delete[] index_map;

   // Thanks a lot!
}

#endif

