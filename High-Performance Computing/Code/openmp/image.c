#include <stdlib.h>
#include "image.h"

void CreateImage(Image *image)
{
  Image im;
  im = calloc(1, sizeof(*im));
  *image = im;
}

void SetSizes(Image image, int NX, int NY)
{
  image->NX = NX;
  image->NY = NY;
  image->data = calloc(NX * NY, sizeof(*image->data));
}

void SetThreshold(Image image, int threshold)
{
  image->threshold = threshold;
}

void CopyImage(Image in, Image *out)
{
  CreateImage(out);
  SetSizes(*out, in->NX, in->NY);
  SetThreshold(*out, in->threshold);
  for (int i = 0; i < in->NX * in->NY; i++) {
    (*out)->data[i] = in->data[i];
  }
}

void DestroyImage(Image *image)
{
  if (!*image) return;
  free((*image)->data);
  free(*image);
  *image = NULL;
}
