#ifndef __MPI_ABORT_H__
#define __MPI_ABORT_H__

// MPI_Abort() on failure
inline void abort(int err) { if(err != MPI_SUCCESS) { MPI_Abort(MPI_COMM_WORLD, err); } }

#endif

