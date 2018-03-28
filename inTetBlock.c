/************************************************************************
  Copyright 2017 Ioannis Nompelis
  Ioannis Nompelis  <nompelis@nobelware.com>           Created: 20170801
  Ioannis Nompelis  <nompelis@nobelware.com>     Last modified: 20170808
 ************************************************************************/

/******************************************************************************
 Copyright (c) 2017, Ioannis Nompelis
 All rights reserved.

 Redistribution and use in source and binary forms, with or without any
 modification, are permitted provided that the following conditions are met:
 1. Redistribution of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistribution in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. All advertising materials mentioning features or use of this software
    must display the following acknowledgement:
    "This product includes software developed by Ioannis Nompelis."
 4. Neither the name of Ioannis Nompelis and his partners/affiliates nor the
    names of other contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.
 5. Redistribution or use of source code and binary forms for profit must
    have written permission of the copyright holder.
 
 THIS SOFTWARE IS PROVIDED BY IOANNIS NOMPELIS ''AS IS'' AND ANY
 EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL IOANNIS NOMPELIS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <inPlot3d.h>

#include "inTetBlock.h"

/*
 * The brick/hexahedron is arranged in the counter-clockwise bottom-first
 * 1,2,3,4 and 5,6,7,8 order. The following lists are counter-clockwise base
 * and peak corner order.
 *
 *  1 2 3 7    (max-x,min-y side of diagonal, bottom)
 *  1 6 2 7    (max-x,min-y side of diagonal, middle)
 *  1 6 7 5    (max-x,min-y side of diagonal, top)
 *  1 3 4 7    (min-x,max-y side of diagonal, bottom)
 *  1 8 7 4    (min-x,max-y side of diagonal, middle)
 *  7 5 8 1    (min-x,max-y side of diagonal, top)
 *
 * A brick as a p3d structured cell has k,j,i unrolling node ordering.
 * The following internal faces are created as a result of the subdivision. The
 * numbers to the right are the Left and Right adjacent cells to each face,
 * under the convention that a right-handed system points Left-to-Right cell.
 *
 *  N N N  E E
 * ------------
 *  1 5 7  3 6
 *  1 7 3  1 4
 *  1 2 7  1 2
 *  1 6 7  2 3
 *  1 7 4  4 5
 *  1 7 8  5 6
 *
 * This convention is used to form faces in the routines below. Boundary faces
 * follow the same right-hand convention with face-normals pointing outward.
 * For the construction of boundary faces numbering is intuitive.
 */

int inTetBlock_PTDBlock( struct inPTD_block *b ){   int *icon=NULL;   double *x,*y,*z;   int nno,nfa,nel;   int jzone[7][3] = { {0,0,0},                        {0,0,0},                        {0,0,0},                        {0,0,0},                        {0,0,0},                        {0,0,0},                        {0,0,0} };   int iret;   iret = inTetBlock( b->im, b->jm, b->km, &nno, &nfa, &nel, jzone, &icon );   if( iret != 0 ) {      printf(" e  There was an error creating the tetrahedral block\n");      return(1);   }   x = b->x;   y = b->y;   z = b->z;   inTetBlock_WriteFluentCase( "fluent.cas",                                nno, nfa, nel, jzone, icon, x, y, z );   free( icon );   return(0);}int inTetBlock( int imn, int jmn, int kmn,                int *nno_, int *nfa_, int *nel_, int jzone[7][3], int **icon_ ){   int *icon=NULL;   int im,jm,km;   int i,j,k,n;   int nel,nfa,nno;   printf("Building case file made of tetrahedra derived from hexahedra\n");   im = imn-1;   jm = jmn-1;   km = kmn-1;   printf("Block: im=%d, jm=%d, km=%d (%d) cells \n",im,jm,km, im*jm*km);   nno = (im+1)*(jm+1)*(km+1);   nel = im*jm*km * 6;   nfa = (im+1)*jm*km*2 +         (jm+1)*im*km*2 +         (km+1)*im*jm*2 +         im*jm*km*(2 + 2 + 2);   jzone[0][2] = (im-1)*jm*km*2 +                 (jm-1)*im*km*2 +                 (km-1)*im*jm*2 +                 im*jm*km*(2 + 2 + 2);   jzone[1][2] = jm*km*2;   jzone[2][2] = jm*km*2;   jzone[3][2] = im*km*2;   jzone[4][2] = im*km*2;   jzone[5][2] = im*jm*2;   jzone[6][2] = im*jm*2;   jzone[0][1] = jzone[0][2] - 1;   for(n=1;n<7;++n) {      jzone[n][0] = jzone[n-1][1] + 1;      jzone[n][1] = jzone[n][0] + jzone[n][2] - 1;   }   printf("Faces per face-group (expected)\n");   for(n=0;n<7;++n) printf("  group: %d, is=%d, ie=%d, icnt=%d \n", n,                              jzone[n][0], jzone[n][1], jzone[n][2] );   icon = (int *) malloc(((size_t) nfa)*5*sizeof(int));   if( icon == NULL ) {      printf(" e  Could not allocate memory for face-connectivity\n");      return(-1);   } else {   }   for(n=0;n<7;++n) jzone[n][2] = 0;   for(k=0;k<km;++k) {   for(j=0;j<jm;++j) {   for(i=0;i<im;++i) {      int n1 = (k  )*(jm+1)*(im+1) + (j  )*(im+1) + (i  );      int n2 = (k  )*(jm+1)*(im+1) + (j  )*(im+1) + (i+1);      int n3 = (k  )*(jm+1)*(im+1) + (j+1)*(im+1) + (i+1);      int n4 = (k  )*(jm+1)*(im+1) + (j+1)*(im+1) + (i  );      int n5 = (k+1)*(jm+1)*(im+1) + (j  )*(im+1) + (i  );      int n6 = (k+1)*(jm+1)*(im+1) + (j  )*(im+1) + (i+1);      int n7 = (k+1)*(jm+1)*(im+1) + (j+1)*(im+1) + (i+1);      int n8 = (k+1)*(jm+1)*(im+1) + (j+1)*(im+1) + (i  );      int io0 = ( (k  )*jm*im + (j  )*im + (i  ) )*6;      int io1 = ( (k  )*jm*im + (j  )*im + (i-1) )*6;      int io3 = ( (k  )*jm*im + (j-1)*im + (i  ) )*6;      int io5 = ( (k-1)*jm*im + (j  )*im + (i  ) )*6;      if( i == 0 ) {         n = jzone[1][0] + jzone[1][2];         ++( jzone[1][2] );         icon[n*5 + 0] = n1;         icon[n*5 + 1] = n8;         icon[n*5 + 2] = n4;         icon[n*5 + 3] = io0 + 4;         icon[n*5 + 4] = -1;         n = jzone[1][0] + jzone[1][2];         ++( jzone[1][2] );         icon[n*5 + 0] = n1;         icon[n*5 + 1] = n5;         icon[n*5 + 2] = n8;         icon[n*5 + 3] = io0 + 5;         icon[n*5 + 4] = -1;      } else {         n = jzone[0][0] + jzone[0][2];         ++( jzone[0][2] );         icon[n*5 + 0] = n1;         icon[n*5 + 1] = n4;         icon[n*5 + 2] = n8;         icon[n*5 + 3] = io1 + 0;         icon[n*5 + 4] = io0 + 4;         n = jzone[0][0] + jzone[0][2];         ++( jzone[0][2] );         icon[n*5 + 0] = n1;         icon[n*5 + 1] = n8;         icon[n*5 + 2] = n5;         icon[n*5 + 3] = io1 + 1;         icon[n*5 + 4] = io0 + 5;      }      if( i == im-1 ) {         n = jzone[2][0] + jzone[2][2];         ++( jzone[2][2] );         icon[n*5 + 0] = n2;         icon[n*5 + 1] = n3;         icon[n*5 + 2] = n7;         icon[n*5 + 3] = io0 + 0;         icon[n*5 + 4] = -1;         n = jzone[2][0] + jzone[2][2];         ++( jzone[2][2] );         icon[n*5 + 0] = n2;         icon[n*5 + 1] = n7;         icon[n*5 + 2] = n6;         icon[n*5 + 3] = io0 + 1;         icon[n*5 + 4] = -1;      }      if( j == 0 ) {         n = jzone[3][0] + jzone[3][2];         ++( jzone[3][2] );         icon[n*5 + 0] = n1;         icon[n*5 + 1] = n2;         icon[n*5 + 2] = n6;         icon[n*5 + 3] = io0 + 1;         icon[n*5 + 4] = -1;         n = jzone[3][0] + jzone[3][2];         ++( jzone[3][2] );         icon[n*5 + 0] = n1;         icon[n*5 + 1] = n6;         icon[n*5 + 2] = n5;         icon[n*5 + 3] = io0 + 2;         icon[n*5 + 4] = -1;      } else {         n = jzone[0][0] + jzone[0][2];         ++( jzone[0][2] );         icon[n*5 + 0] = n1;         icon[n*5 + 1] = n6;         icon[n*5 + 2] = n2;         icon[n*5 + 3] = io3 + 3;         icon[n*5 + 4] = io0 + 1;            n = jzone[0][0] + jzone[0][2];         ++( jzone[0][2] );         icon[n*5 + 0] = n1;         icon[n*5 + 1] = n5;          icon[n*5 + 2] = n6;         icon[n*5 + 3] = io3 + 4;         icon[n*5 + 4] = io0 + 2;      }      if( j == jm-1 ) {         n = jzone[4][0] + jzone[4][2];         ++( jzone[4][2] );         icon[n*5 + 0] = n4;         icon[n*5 + 1] = n7;         icon[n*5 + 2] = n3;         icon[n*5 + 3] = io0 + 3;         icon[n*5 + 4] = -1;         n = jzone[4][0] + jzone[4][2];         ++( jzone[4][2] );         icon[n*5 + 0] = n4;         icon[n*5 + 1] = n8;         icon[n*5 + 2] = n7;         icon[n*5 + 3] = io0 + 4;         icon[n*5 + 4] = -1;      }      if( k == 0 ) {         n = jzone[5][0] + jzone[5][2];         ++( jzone[5][2] );         icon[n*5 + 0] = n1;         icon[n*5 + 1] = n3;         icon[n*5 + 2] = n2;         icon[n*5 + 3] = io0 + 0;         icon[n*5 + 4] = -1;         n = jzone[5][0] + jzone[5][2];         ++( jzone[5][2] );         icon[n*5 + 0] = n1;         icon[n*5 + 1] = n4;         icon[n*5 + 2] = n3;         icon[n*5 + 3] = io0 + 3;         icon[n*5 + 4] = -1;      } else {         n = jzone[0][0] + jzone[0][2];         ++( jzone[0][2] );         icon[n*5 + 0] = n1;         icon[n*5 + 1] = n2;         icon[n*5 + 2] = n3;         icon[n*5 + 3] = io5 + 2;         icon[n*5 + 4] = io0 + 0;         n = jzone[0][0] + jzone[0][2];         ++( jzone[0][2] );         icon[n*5 + 0] = n1;         icon[n*5 + 1] = n3;         icon[n*5 + 2] = n4;         icon[n*5 + 3] = io5 + 5;         icon[n*5 + 4] = io0 + 3;      }      if( k == km-1 ) {         n = jzone[6][0] + jzone[6][2];         ++( jzone[6][2] );         icon[n*5 + 0] = n5;         icon[n*5 + 1] = n6;         icon[n*5 + 2] = n7;         icon[n*5 + 3] = io0 + 2;         icon[n*5 + 4] = -1;         n = jzone[6][0] + jzone[6][2];         ++( jzone[6][2] );         icon[n*5 + 0] = n5;         icon[n*5 + 1] = n7;         icon[n*5 + 2] = n8;         icon[n*5 + 3] = io0 + 5;         icon[n*5 + 4] = -1;      }      n = jzone[0][0] + jzone[0][2];      ++( jzone[0][2] );      icon[n*5 + 0] = n1;      icon[n*5 + 1] = n5;      icon[n*5 + 2] = n7;      icon[n*5 + 3] = io0 + 2;      icon[n*5 + 4] = io0 + 5;      n = jzone[0][0] + jzone[0][2];      ++( jzone[0][2] );       icon[n*5 + 0] = n1;      icon[n*5 + 1] = n7;      icon[n*5 + 2] = n3;      icon[n*5 + 3] = io0 + 0;       icon[n*5 + 4] = io0 + 3;       n = jzone[0][0] + jzone[0][2];      ++( jzone[0][2] );      icon[n*5 + 0] = n1;      icon[n*5 + 1] = n2;      icon[n*5 + 2] = n7;      icon[n*5 + 3] = io0 + 0;      icon[n*5 + 4] = io0 + 1;      n = jzone[0][0] + jzone[0][2];      ++( jzone[0][2] );      icon[n*5 + 0] = n1;      icon[n*5 + 1] = n6;      icon[n*5 + 2] = n7;      icon[n*5 + 3] = io0 + 1;      icon[n*5 + 4] = io0 + 2;      n = jzone[0][0] + jzone[0][2];      ++( jzone[0][2] );      icon[n*5 + 0] = n1;      icon[n*5 + 1] = n7;      icon[n*5 + 2] = n4;      icon[n*5 + 3] = io0 + 3;      icon[n*5 + 4] = io0 + 4;      n = jzone[0][0] + jzone[0][2];      ++( jzone[0][2] );      icon[n*5 + 0] = n1;      icon[n*5 + 1] = n7;      icon[n*5 + 2] = n8;      icon[n*5 + 3] = io0 + 4;      icon[n*5 + 4] = io0 + 5;   }}}   printf("Faces per face-group (final)\n");   for(n=0;n<7;++n) printf("  group: %d, is=%d, ie=%d, icnt=%d \n", n,                              jzone[n][0], jzone[n][1], jzone[n][2] );   *nno_ = nno;   *nel_ = nel;   *nfa_ = nfa;   *icon_ = icon;   return(0);}int inTetBlock_WriteFluentCase( char *filename,                                int nno, int nfa, int nel,                                int jzone[7][3], int *icon,                                double *x, double *y, double *z ){   FILE *fp;   int nzone,n,k;   fp = fopen( filename, "w" );   if( fp == NULL ) {      printf(" e  Could not open Fluent case file (\"%s\") for writing\n",filename);      return(1);   } else {      printf(" Writing Fluent case file: \"%s\" \n",filename);   }   fprintf(fp,"(0 \"File automatically generated by inTetBlock.      \") \n");   fprintf(fp,"(0 \"Do not edit this file unless you know what you   \") \n");   fprintf(fp,"(0 \"are doing.                                       \") \n");   fprintf(fp,"(0 \"This file wants to be FLUENT-compliant           \") \n");   fprintf(fp,"\n\n");   fprintf(fp,"(0 \"Automatically generated files are always 3D.     \") \n");   fprintf(fp,"(2 3) \n");   fprintf(fp,"\n\n");   nzone = 1;   fprintf(fp,"(0 \"Nodes section                                    \") \n");   fprintf(fp,"(0 \" Total nodes: %d\") \n",nno);   fprintf(fp,"(10 (0 1 %x 1 3)) \n",nno);   fprintf(fp,"(10 (%x 1 %x 1 3) ( \n",nzone,nno);   for(n=0;n<nno;++n) {      fprintf(fp," %23.16e %23.16e %23.16e \n",x[n],y[n],z[n]);   }   fprintf(fp,")) \n");   fprintf(fp,"\n\n");   nzone += 1;   fprintf(fp,"(0 \"Cells section                                    \") \n");   fprintf(fp,"(0 \" Total cells: %d\") \n",nel);   fprintf(fp,"(12 (0 1 %x 0) ) \n",nel);   fprintf(fp,"\n\n");   fprintf(fp,"\n\n");   fprintf(fp,"(0 \"Faces section                                    \") \n");   fprintf(fp,"(0 \" Total faces: %d\") \n",nfa);   fprintf(fp,"(13 (0 1 %x 0)) \n",nfa);   fprintf(fp,"\n\n");   nzone += 1;   fprintf(fp,"(0 \"Interior faces                                   \") \n");   fprintf(fp,"(0  Start: %d End: %d ) \n",jzone[0][0] +1,jzone[0][1] +1);   fprintf(fp,"(13 (%x %x %x 2 3) ( \n",nzone, jzone[0][0] +1,jzone[0][1] +1);   for(n=jzone[0][0];n<=jzone[0][1];++n) {      int n1 = icon[n*5+0] +1;      int n2 = icon[n*5+1] +1;      int n3 = icon[n*5+2] +1;      int i1 = icon[n*5+3] +1;      int i2 = icon[n*5+4] +1;      fprintf(fp," %x %x %x  %x %x \n",n1,n2,n3, i2,i1);   }   fprintf(fp,")) \n");   fprintf(fp,"\n\n");   fprintf(fp,"(0 \"boundary faces                                   \") \n");   for(k=1;k<7;++k) {      nzone += 1;      fprintf(fp,"(0  Start: %d End: %d ) \n",jzone[k][0] +1,jzone[k][1] +1);      fprintf(fp,"(13 (%x %x %x %x 3) ( \n",              nzone,jzone[k][0] +1,jzone[k][1] +1, 70+k);      for(n=jzone[k][0];n<=jzone[k][1];++n) {         int n1 = icon[n*5+0] +1;         int n2 = icon[n*5+1] +1;         int n3 = icon[n*5+2] +1;         int i1 = icon[n*5+3] +1;         int i2 = icon[n*5+4] +1;         fprintf(fp," %x %x %x  %x %x \n",n1,n2,n3, i2,i1);      }      fprintf(fp,")) \n");      fprintf(fp,"\n\n");   }   fclose( fp );   return(0);}
