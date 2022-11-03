#ifndef _UTILS_H
#define _UTILS_H
#include <mpi.h>
#include <stdio.h>

#define CHKERR(ierr) do {                                        \
    if (ierr) {                                                  \
      fprintf(stderr, "MPI failed with return code %d\n", ierr); \
      return MPI_Abort(MPI_COMM_WORLD, ierr);                    \
    }                                                            \
  } while (0)

typedef enum { CHECK, BENCH } Mode;

typedef struct {
  Mode mode;
  int count;
} UserOptions;

int SyncPrintf(MPI_Comm, const char *, ...);

#endif
