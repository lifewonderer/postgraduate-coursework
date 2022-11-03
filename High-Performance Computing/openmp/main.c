#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "image.h"
#include "pgmio.h"
#include "omp.h"

static inline int linear_index(int i, int j, int NX)
{
  return i*NX + j;
}

/* Detect edges in input image using a Laplacian filter and produce
   output "edges" image. Allocates output image. */
void EdgeDetect(Image input, Image *edges)
{
  int NX = input->NX;
  int NY = input->NY;
  CreateImage(edges);
  SetSizes(*edges, NX, NY);
  SetThreshold(*edges, input->threshold);

  for (int i = 0; i < NY; i++) { /* rows */
    for (int j = 0; j < NX; j++) { /* columns */
      int ij = linear_index(i, j, NX);
      int ijm1 = linear_index(i, j-1, NX);
      int ijp1 = linear_index(i, j+1, NX);
      int im1j = linear_index(i-1, j, NX);
      int ip1j = linear_index(i+1, j, NX);
      float vij, vijm1, vijp1, vim1j, vip1j;

      vij = input->data[ij];
      vim1j = (i == 0) ? 0 : input->data[im1j];
      vip1j = (i == NY-1) ? 0 : input->data[ip1j];
      vijm1 = (j == 0) ? 0 : input->data[ijm1];
      vijp1 = (j == NX-1) ? 0 : input->data[ijp1];
      /*
       * out[i, j] = in[i-1, j] + in[i+1, j] + in[i, j-1] + in[i, j+1] - 4*in[i, j];
       * For out of bounds accesses (edge of image), use 0.
       */
      (*edges)->data[ij] = (vijm1 + vijp1 + vim1j + vip1j - 4*vij);
    }
  }
}

/* Compute the norm of the residual for the iteration.
 * We are solving the linear equation
 * A x = b
 * Where x represents the solution, b is the right hand side (the
 * edges), and A is the stencil for the Laplace operator.
 *
 * The residual is r = b - Ax, which tends to zero as we approach the
 * solution.
 *
 * The norm of the residual is \sqrt(\sum_i r_i^2)
 */
void ResidualNorm(Image edges, Image solution, float *residual)
{
  *residual = 0;
  int NX = edges->NX;
  int NY = edges->NY;
  for (int i = 0; i < NY; i++) { /* rows */
    for (int j = 0; j < NX; j++) { /* columns */
      const int ij = linear_index(i, j, NX);
      const int ijm1 = linear_index(i, j-1, NX);
      const int ijp1 = linear_index(i, j+1, NX);
      const int im1j = linear_index(i-1, j, NX);
      const int ip1j = linear_index(i+1, j, NX);
      float vij, vijm1, vijp1, vim1j, vip1j, r;
      vij = solution->data[ij];
      vim1j = (i == 0) ? 0 : solution->data[im1j];
      vip1j = (i == NY-1) ? 0 : solution->data[ip1j];
      vijm1 = (j == 0) ? 0 : solution->data[ijm1];
      vijp1 = (j == NX-1) ? 0 : solution->data[ijp1];

      /* r = b - Ax */
      r = edges->data[ij] - (vijm1 + vijp1 + vim1j + vip1j - 4*vij);

      *residual += r*r;
    }
  }
  *residual = sqrt(*residual);
}

void ReconstructFromEdges(Image edges, int niterations, Image *output)
{
  Image new = NULL;
  int NX = edges->NX;
  int NY = edges->NY;
  float r0, r;
  /* Copy edges into old image */
  CopyImage(edges, &new);
  /* Construct the initial residual */
  ResidualNorm(edges, new, &r0);
  /* Run niterations Gauss-Seidel iterations to invert the Laplacian,
   * reconstructing an output image from its edges. */
  for (int it = 0; it < niterations; it++) {
    /* Monitor convergence to solution */
    if (it % 100 == 0) {
      ResidualNorm(edges, new, &r);
      printf("%6d ||r||/||r0|| = %g\n", it, r/r0);
    }
    /* You should try and parallelise these loops.
     * Take care to avoid data races.*/
    int i, j;
    #pragma omp parallel default(none) shared(NY,NX,edges,new) private(i,j)
    {
      /* Update all rad nodes. */
      #pragma omp for
      for (i = 0; i < NY; i++) { /* rows */
        for (j = (i % 2 == 0) ? 0 : 1; j < NX; j+=2) { /* columns */
          const int ij = linear_index(i, j, NX);
          const int ijm1 = linear_index(i, j-1, NX);
          const int ijp1 = linear_index(i, j+1, NX);
          const int im1j = linear_index(i-1, j, NX);
          const int ip1j = linear_index(i+1, j, NX);
          float vij, vijm1, vijp1, vim1j, vip1j;

          vij = edges->data[ij];
          vim1j = (i == 0) ? 0 : new->data[im1j];
          vip1j = (i == NY-1) ? 0 : new->data[ip1j];
          vijm1 = (j == 0) ? 0 : new->data[ijm1];
          vijp1 = (j == NX-1) ? 0 : new->data[ijp1];
          new->data[ij] = 0.25*(vijm1 + vijp1 + vim1j + vip1j) - 0.25*vij;
        }
      }

      /* Update all black nodes using the new red notes. */
      #pragma omp for  
      for (i = 0; i < NY; i++) { /* rows */
        for (j = (i % 2 == 0) ? 1 : 0; j < NX; j+=2) { /* columns */
          const int ij = linear_index(i, j, NX);
          const int ijm1 = linear_index(i, j-1, NX);
          const int ijp1 = linear_index(i, j+1, NX);
          const int im1j = linear_index(i-1, j, NX);
          const int ip1j = linear_index(i+1, j, NX);
          float vij, vijm1, vijp1, vim1j, vip1j;

          vij = edges->data[ij];
          vim1j = (i == 0) ? 0 : new->data[im1j];
          vip1j = (i == NY-1) ? 0 : new->data[ip1j];
          vijm1 = (j == 0) ? 0 : new->data[ijm1];
          vijp1 = (j == NX-1) ? 0 : new->data[ijp1];
          new->data[ij] = 0.25*(vijm1 + vijp1 + vim1j + vip1j) - 0.25*vij;
        }
      }
    }
  }
  /* Final residual */
  ResidualNorm(edges, new, &r);
  printf("%6d ||r||/||r0|| = %g\n", niterations, r/r0);
  *output = new;
}

int main(int argc, char **argv)
{
  Image edges = NULL;
  Image reconstructed = NULL;
  Image input = NULL;
  int niterations = 10;

  if (argc != 5) {
    fprintf(stderr, "Usage: %s INPUT EDGES RECONSTRUCTED NITERATIONS\n", argv[0]);
    return 1;
  }

  niterations = atoi(argv[4]);

  ReadImage(argv[1], &input);
  EdgeDetect(input, &edges);
  DestroyImage(&input);
  WriteImage(argv[2], edges);
  ReconstructFromEdges(edges, niterations, &reconstructed);
  DestroyImage(&edges);
  WriteImage(argv[3], reconstructed);
  DestroyImage(&reconstructed);

  return 0;
}
