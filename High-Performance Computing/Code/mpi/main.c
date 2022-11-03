#include <mpi.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include "reduce.h"
#include "utils.h"


static void usage(const char *progname)
{
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "%s -N N [-t MODE] [-h]\n", progname);
  fprintf(stderr, "Run benchmarking or checking of allreduce.\n\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, " -N N\n");
  fprintf(stderr, "    Set the message count (required).\n\n");
  fprintf(stderr, " -t BENCH | CHECK\n");
  fprintf(stderr, "    Select execution mode (default CHECK).\n");
  fprintf(stderr, "    CHECK: non-exhaustive check of correctness.\n");
  fprintf(stderr, "    BENCH: print timing data.\n");
  fprintf(stderr, " -h\n");
  fprintf(stderr, "    Print this help.\n");
}

static int ProcessOptions(MPI_Comm comm, int argc, char **argv, UserOptions *options)
{
  int ch;
  int rank;
  int ierr;
  ierr = MPI_Comm_rank(comm, &rank);CHKERR(ierr);
  while ((ch = getopt(argc, argv, "t:N:h")) != -1) {
    switch (ch) {
    case 't':
      if (strncmp(optarg, "CHECK", 5) == 0) {
        options->mode = CHECK;
      } else if (strncmp(optarg, "BENCH", 5) == 0) {
        options->mode = BENCH;
      } else {
        if (!rank) {
          fprintf(stderr, "Unrecognised mode type '%s'.\n\n", optarg);
          usage(argv[0]);
        }
        return 1;
      }
      break;
    case 'N':
      errno = 0;
      options->count = (int)strtol(optarg, NULL, 10);
      if (options->count <= 0 || errno == ERANGE) {
        if (!rank) {
          fprintf(stderr, "Could not interpret message size '%s' as positive integer.\n\n", optarg);
          usage(argv[0]);
        }
        return 1;
      }
      break;
    case 'h':
    default:
      if (!rank) usage(argv[0]);
      return 1;
    }
  }
  return 0;
}

/* Run some tests of the implementations by comparing to the results
   of MPI_Allreduce */
static int CheckAllreduce(MPI_Comm comm)
{
  int ierr;
  int rank;
  int size;
  int testno;
  int i;
  int c;
  int o;
  int pass, gpass;
  const MPI_Op ops[] = {MPI_SUM, MPI_PROD, MPI_MIN, MPI_MAX};
  const char *names[] = {"MPI_SUM", "MPI_PROD", "MPI_MIN", "MPI_MAX"};
  const int counts[] = {1, 3, 12, 1024*1024 + 1};
  int nop = sizeof(ops) / sizeof(*ops);
  int ncount = sizeof(counts) / sizeof(*counts);
  ierr = MPI_Comm_rank(comm, &rank);CHKERR(ierr);
  ierr = MPI_Comm_size(comm, &size);CHKERR(ierr);
  /* Output confirms to Test Anything Protocol (see
     https://en.wikipedia.org/wiki/Test_Anything_Protocol) */
  if (!rank) {
    printf("1..%d\n", nop * ncount);
    fflush(stdout);
  }
  testno = 1;
  /* For all the diffeernt counts */
  for (c = 0; c < ncount; c++) {
    int count = counts[c];
    int *sendbuf = malloc(count*sizeof(*sendbuf));
    int *recvbuf = malloc(count*sizeof(*recvbuf));
    int *expect = malloc(count*sizeof(*expect));
    if (!sendbuf || !recvbuf || !expect) {
      fprintf(stderr, "Unable to allocate memory for checking\n");
      free(sendbuf);
      free(recvbuf);
      free(expect);
      return 1;
    }
    for (i = 0; i < count; i++) {
      sendbuf[i] = (rank + 5) * (1 - 2 * (i % 2));
    }

    /* For all the different operations we care about */
    for (o = 0; o < nop; o++) {
      MPI_Op op = ops[o];
      const char *name = names[o];
      ierr = MPI_Allreduce(sendbuf, expect, count, MPI_INT, op, comm);CHKERR(ierr);
      ierr = tree_allreduce(sendbuf, recvbuf, count, op, comm);CHKERR(ierr);
      pass = 1;
      for (i = 0; i < count; i++) {
        if (expect[i] != recvbuf[i]) {
          pass = 0;
          break;
        }
      }
      ierr = MPI_Allreduce(&pass, &gpass, 1, MPI_INT, MPI_LAND, comm);CHKERR(ierr);
      if (!gpass) {
        if (!rank) {
          printf("not ok %d - %s count %d: Not all processes have correct result\n", testno, name, count);
          fflush(stdout);
        }
        if (pass) {
          ierr = SyncPrintf(comm, "    [rank: %d] Has correct result\n", rank);CHKERR(ierr);
        } else {
          /* Some debugging information if the result is incorrect */
          ierr = SyncPrintf(comm, "    [rank: %d] Has incorrect result, first mismatch at entry %d\n", rank, i);CHKERR(ierr);
        }
      } else {
        if (!rank) {
          printf("ok %d - %s count %d\n", testno, name, count);
          fflush(stdout);
        }
      }
      testno++;
    }
    free(sendbuf);
    free(recvbuf);
    free(expect);
  }
  return 0;
}

/* Run some benchmarking */
static int BenchAllreduce(MPI_Comm comm, UserOptions options)
{
  double start;
  double end;
  int i;
  int rank;
  int ierr;
  int nreps;
  int *sendbuf = NULL;
  int *recvbuf = NULL;

  sendbuf = malloc(options.count * sizeof(*sendbuf));
  recvbuf = malloc(options.count * sizeof(*recvbuf));
  if (!sendbuf || !recvbuf) {
    fprintf(stderr, "Unable to allocate memory for benchmarking\n");
    free(sendbuf);
    free(recvbuf);
    return 1;
  }

  ierr = MPI_Comm_rank(comm, &rank);CHKERR(ierr);
  for (i = 0; i < options.count; i++) {
    sendbuf[i] = rank;
  }

  /* Try and figure out a number of repetitions such that we run for
     around two seconds total. */
  start = MPI_Wtime();
  for (i = 0; i < 250; i++) {
    MPI_Allreduce(sendbuf, recvbuf, options.count, MPI_INT, MPI_SUM, comm);
  }
  end = MPI_Wtime();
  nreps = (int)(500 / (end - start));
  if (nreps <= 0) {
    nreps = 4;
  }
  /* Collectively pick the number of repetitions. */
  ierr = MPI_Allreduce(MPI_IN_PLACE, &nreps, 1, MPI_INT, MPI_MAX, comm);CHKERR(ierr);

  /* Start the benchmarking */
  start = MPI_Wtime();
  for (i = 0; i < nreps; i++) {
    ring_allreduce(sendbuf, recvbuf, options.count, MPI_SUM, comm);
  }
  end = MPI_Wtime();
  if (!rank) {
    printf("%d %d %g", options.count, nreps, (end - start)/nreps);
  }
  start = MPI_Wtime();
  for (i = 0; i < nreps; i++) {
    tree_allreduce(sendbuf, recvbuf, options.count, MPI_SUM, comm);
  }
  end = MPI_Wtime();
  if (!rank) {
    printf(" %g", (end - start)/nreps);
  }
  start = MPI_Wtime();
  for (i = 0; i < nreps; i++) {
    MPI_Allreduce(sendbuf, recvbuf, options.count, MPI_INT, MPI_SUM, comm);
  }
  end = MPI_Wtime();
  if (!rank) {
    printf(" %g\n", (end - start)/nreps);
  }
  free(sendbuf);
  free(recvbuf);
  return 0;
}

int main(int argc, char **argv)
{
  int rank;
  int ierr;
  MPI_Comm comm;
  UserOptions options = { .count = -1, .mode = CHECK };
  ierr = MPI_Init(&argc, &argv);
  if (ierr) {
    fprintf(stderr, "MPI_Init failed with status code %d\n", ierr);
    return ierr;
  }
  comm = MPI_COMM_WORLD;
  if (ProcessOptions(comm, argc, argv, &options)) {
    ierr = MPI_Finalize();
    return ierr;
  }

  ierr = MPI_Comm_rank(comm, &rank);CHKERR(ierr);

  switch (options.mode) {
  case CHECK:
    ierr = CheckAllreduce(comm);CHKERR(ierr);
    break;
  case BENCH:
    ierr = BenchAllreduce(comm, options);CHKERR(ierr);
    break;
  }
  ierr = MPI_Finalize();
  return ierr;
}
