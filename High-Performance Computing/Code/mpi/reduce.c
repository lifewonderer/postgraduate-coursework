#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include "reduce.h"
#include "utils.h"

/* This is the implementation that we came up with in the exercises for
   MPI_SUM. */
int ring_allreduce(const int *sendbuf, int *recvbuf, int count, MPI_Op op,
                   MPI_Comm comm)
{
  int rank, size;
  int i, c;
  int ierr;
  int source;
  int dest;

  if (op != MPI_SUM) {
    fprintf(stderr, "Only coded for op == MPI_SUM\n");
    return MPI_Abort(comm, 1);
  }
  ierr = MPI_Comm_rank(comm, &rank);CHKERR(ierr);
  ierr = MPI_Comm_size(comm, &size);CHKERR(ierr);

  source = (rank + size - 1) % size;
  dest = (rank + 1) % size;
  for (c = 0; c < count; c++) {
    recvbuf[c] = sendbuf[c];
  }
  for (i = 0; i < size - 1; i++) {
    ierr = MPI_Sendrecv_replace(recvbuf, count, MPI_INT, dest, 0, source, 0, comm, MPI_STATUS_IGNORE);CHKERR(ierr);
    /* Only correct for op == MPI_SUM */
    for (c = 0; c < count; c++) {
      recvbuf[c] += sendbuf[c];
    }
  }
  return 0;
}

int tree_allreduce(const int *sendbuf, int *recvbuf, int count, MPI_Op op,
                   MPI_Comm comm)
{
  /* You should implement the tree reduction here, you should handle
     MPI_SUM, MPI_PROD, MPI_MIN, and MPI_MAX for the MPI_Op
     argument. */
  int rank, size;
  int c;
  int ierr;
  int source;
  int dest;
  int group;
  int *tmprecv = NULL;

  tmprecv = malloc(count * sizeof(*tmprecv));
  if (!tmprecv) {
    fprintf(stderr, "Unable to allocate memory for tree_allreduce\n");
    free(tmprecv);
    return 1;
  }

  if ((op != MPI_SUM)&&(op != MPI_PROD)&&(op != MPI_MIN)&&(op != MPI_MAX)) {
    fprintf(stderr, "Only coded for op == MPI_SUM, MPI_PROD, MPI_MIN, MPI_MAX\n");
    return MPI_Abort(comm, 1);
  }
  ierr = MPI_Comm_rank(comm, &rank);CHKERR(ierr);
  ierr = MPI_Comm_size(comm, &size);CHKERR(ierr);

  for (c = 0; c < count; c++) {
    recvbuf[c] = sendbuf[c];
  }

  /* Reduce form the leaves of the tree to the root. */
  for (group = size; group > 1; group /= 2) {
    if (rank < group / 2) {
      source = rank + group / 2;
      ierr = MPI_Recv(tmprecv, count, MPI_INT, source, 0, comm, MPI_STATUS_IGNORE);CHKERR(ierr);
      for (c = 0; c < count; c++) {
        switch (op) {
        case MPI_SUM:
          recvbuf[c] += tmprecv[c];
          break;
        case MPI_PROD:
          recvbuf[c] *= tmprecv[c];
          break;
        case MPI_MIN:
          if (tmprecv[c] < recvbuf[c]) {
            recvbuf[c] = tmprecv[c];
          }
           break;
        case MPI_MAX:
          if (tmprecv[c] > recvbuf[c]) {
            recvbuf[c] = tmprecv[c];
          }
          break;
        }
      }
    }
    else if (rank >= group / 2 && rank < group) {
      dest = rank - group / 2;
      ierr = MPI_Send(recvbuf, count, MPI_INT, dest, 0, comm);CHKERR(ierr);
    }
  }

  /* Send back from the root of the tree to the leaves. */
  for (group = 1; group < size; group *= 2) {
    if (rank < group) {
      dest = rank + group;
      ierr = MPI_Send(recvbuf, count, MPI_INT, dest, 0, comm);CHKERR(ierr);
    }
    else if (rank >= group && rank < group * 2) {
      source = rank - group;
      ierr = MPI_Recv(recvbuf, count, MPI_INT, source, 0, comm, MPI_STATUS_IGNORE);CHKERR(ierr);
    }
  }

  free(tmprecv);
  return 0;
}
