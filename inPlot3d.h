
#ifndef _INPLOT3D_H_
#define _INPLOT3D_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct inPTD_block {
   int im,jm,km;
   int s[6];
   double *x, *y, *z;
};

#include "inPlot3d_proto.h"

#ifdef __cplusplus
}
#endif

#endif

