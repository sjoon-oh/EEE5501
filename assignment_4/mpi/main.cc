#include <complex>
#include <iostream>
#include <mpi.h>
#include "abort.h"
#include "data.h"
#include "dft.h"
#include "stopwatch.h"

int main(int argc, char **argv) {
    if(argc != 2) {                     // Run command message
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        exit(1);
    }

    const char *data_file = argv[1];    // Input file
    std::complex<float> *data = 0;      // Data array
    unsigned width = 0, height = 0;     // Data dimension
    int num_ranks = 0;                  // Communicator size
    int rank_id = -1;                   // Rank ID

    // Read data file.
    read(data_file, data, width, height);
    // Initialize MPI.
    abort(MPI_Init(&argc, &argv));
    // Get the communicator size and rank ID.
    abort(MPI_Comm_size(MPI_COMM_WORLD, &num_ranks));
    abort(MPI_Comm_rank(MPI_COMM_WORLD, &rank_id));

    stopwatch_t stopwatch;
    stopwatch.start();
    // Two-dimensional discrete Fourier transform
    dft2d(data, width, height, num_ranks, rank_id);
    stopwatch.stop();
    // Rank 0 displays the runtime and stores the final result to a file.
    if(!rank_id) { stopwatch.display(); write("result", data, width, height); } 

    // Finalize MPI.
    abort(MPI_Finalize());
    // Close data.
    fin(data);

    return 0;
}

