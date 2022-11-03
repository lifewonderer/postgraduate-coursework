#ifndef _REDUCE_H
#define _REDUCE_H
#include <mpi.h>

int ring_allreduce(const int *, int *, int, MPI_Op, MPI_Comm);
int tree_allreduce(const int *, int *, int, MPI_Op, MPI_Comm);

#endif
