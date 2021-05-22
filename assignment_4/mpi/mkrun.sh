 #!/bin/bash
 
 make clean
 make
 mpiexec -n 4 ./mpi input
