#include <mpi.h>
#include <stdio.h>
#include <stdarg.h>
#include "utils.h"

int SyncPrintf(MPI_Comm comm, const char *fmt, ...)
{
  int rank;
  int size;
  int ierr;
  int source;
  int dest;
  va_list args;
  ierr = MPI_Comm_rank(comm, &rank);CHKERR(ierr);
  ierr = MPI_Comm_size(comm, &size);CHKERR(ierr);
  source = rank ? rank - 1 : MPI_PROC_NULL;
  dest = rank == size - 1 ? MPI_PROC_NULL : rank + 1;
  ierr = MPI_Recv(&rank, 1, MPI_INT, source, 0, comm, MPI_STATUS_IGNORE);CHKERR(ierr);
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  fflush(stdout);
  ierr = MPI_Ssend(&rank, 1, MPI_INT, dest, 0, comm);CHKERR(ierr);
  return 0;
}
