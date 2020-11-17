/************************************************************************
  Copyright 2017-2020 Ioannis Nompelis
  Ioannis Nompelis  <nompelis@nobelware.com>     Last modified: 20201117
 ************************************************************************/

/******************************************************************************
 Copyright (c) 2005-2020, Ioannis Nompelis
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

#include "inPlot3d.h"
#include "inPlot3d_proto.h"


 int inPlot3d_ReadFileGrid(char *file, int *nb, struct inPTD_block **blocks) {     int n;    FILE *fp;    int size,*dis = NULL;    double *x = NULL, *y = NULL, *z = NULL;    int ierr;      if((fp=fopen(file,"r"))==NULL) {       fprintf(stdout," e  Could not open multi-block grid file\n");       return(1);    }     fscanf(fp,"%d",nb);    fprintf(stdout," i  File contains %d blocks\n", *nb);     *blocks = (struct inPTD_block *)                      malloc((size_t)(*nb)*sizeof(struct inPTD_block));    dis = (int *) malloc((size_t)(*nb)*sizeof(int));    if(*blocks == NULL || dis == NULL) {       if( *blocks != NULL) free( *blocks );       if( dis != NULL) free( dis );       fprintf(stdout," e  Could not malloc() for blocks\n");       return(-1);    }     size = 0;    for(n=0;n<*nb;++n) {       struct inPTD_block *bp = &((*blocks)[n]);       int ik;        ik = fscanf(fp,"%d %d %d", &(bp->im), &(bp->jm), &(bp->km));       if( ik != 3 ) {          fprintf(stdout," e  Failed to read 3 integer sizes for block %d\n", n);          free( *blocks );          free( dis );           fclose(fp);          return(1);       }        fprintf(stdout,"    Block[%d]: %4d %4d %4d\n",n,bp->im,bp->jm,bp->km);        dis[n] = size;       size += bp->km*bp->jm*bp->im;    }     x = (double *) malloc((size_t) size * sizeof(double));    y = (double *) malloc((size_t) size * sizeof(double));    z = (double *) malloc((size_t) size * sizeof(double));    if(x == NULL || y == NULL || z == NULL) {       fprintf(stdout," e  Could not malloc() for x,y,z\n");        if(x != NULL) free(x);       if(y != NULL) free(y);       if(z != NULL) free(z);       free(dis);       free(*blocks);        fclose(fp);        return(-2);    }     for(n=0;n<(*nb);++n) {       struct inPTD_block *bp = &((*blocks)[n]);        bp->x = (double *) &(x[((size_t) dis[n])]);       bp->y = (double *) &(y[((size_t) dis[n])]);       bp->z = (double *) &(z[((size_t) dis[n])]);    }     fprintf(stdout," r  Reading blocks\n");     for(n=0;n<*nb;++n) {       struct inPTD_block *bp = &((*blocks)[n]);        fprintf(stdout,"    Block[%d] ",n);        ierr = inPlot3d_ReadBlockGrid(fp,bp);       if(ierr != 0) {          fprintf(stdout," e  Failed while reading block data\n");          fprintf(stdout,"    AT THIS POINT I DO NOT KNOW WHY\n");          fprintf(stdout,"    Note: previously read data is returned.\n");           fclose( fp );           return(2);       }        fprintf(stdout," Done\n");    }     fprintf(stdout,"    All done\n");    fclose(fp);     return(0); }   int inPlot3d_ReadBlockGrid(FILE *fp, struct inPTD_block *bp) {    long i,j,k,m;    double d;    int ierr;      for(k=0;k<bp->km;++k) {       fprintf(stdout,".");       for(j=0;j<bp->jm;++j) {          for(i=0;i<bp->im;++i) {             m = (long) (bp->jm*bp->im*k + bp->im*j + i);              ierr = inPlot3d_ReadNumber(fp,&d);             if(ierr != 0) {                fprintf(stdout," e  Error reading a number \n");                return(1);             }             bp->x[m] = d;          }       }    }     for(k=0;k<bp->km;++k) {       fprintf(stdout,".");       for(j=0;j<bp->jm;++j) {          for(i=0;i<bp->im;++i) {             m = (long) (bp->jm*bp->im*k + bp->im*j + i);              ierr = inPlot3d_ReadNumber(fp,&d);             if(ierr != 0) {                fprintf(stdout," e  Error reading a number \n");                return(1);             }             bp->y[m] = d;          }       }    }     for(k=0;k<bp->km;++k) {       fprintf(stdout,".");       for(j=0;j<bp->jm;++j) {          for(i=0;i<bp->im;++i) {             m = (long) (bp->jm*bp->im*k + bp->im*j + i);              ierr = inPlot3d_ReadNumber(fp,&d);             if(ierr != 0) {                fprintf(stdout," e  Error reading a number \n");                return(1);             }             bp->z[m] = d;          }       }    }     return(0); }    int inPlot3d_ReadNumber(FILE *fp, double *d) {     int is,ie,ic,il;    char c,s[100];      memset(s,0,(size_t) 100);     is = 0;    ie = 0;    il = 0;     while(ie == 0) {       ic = fgetc(fp);        if(ic == EOF) {          fprintf(stdout," e  Premature EOF reached??? \n");          return(1);       }        c = (char) ic;       if(c == '\0') {          fprintf(stdout," e  Read null character from file \n");          return(1);       }        if(c == 'd' || c == 'D') c = 'e';       if(c == 'E')             c = 'e';        if(is == 0) {          if(c != ' ' && c != '\n' && c != ',') {             is = 1;              s[il] = c;             il = il + 1;          }       } else {          if(c == ' ' || c == '\n' || c == ',') {             ie = 1;          } else {             s[il] = c;             il = il + 1;          }       }     }     *d = strtod(s,NULL);     return(0); }   int inPlot3d_WriteFileGrid(char *file, int nb, struct inPTD_block *blocks) {     int n;    FILE *fp;      if((fp=fopen(file,"w"))==NULL) {       fprintf(stdout," e  Could not open multi-block grid file for writing\n");       return(1);    }     fprintf(fp,"  %d \n",nb);    for(n=0;n<nb;++n) {       struct inPTD_block *bp = &(blocks[n]);        fprintf(fp,"  %d  %d  %d \n", bp->im, bp->jm, bp->km);        fprintf(stdout,"    Block[%d]: %d %d %d\n",n,bp->im,bp->jm,bp->km);    }      for(n=0;n<nb;++n) {       struct inPTD_block *bp = &(blocks[n]);       long size = (long) (bp->km * bp->jm * bp->im);       long m;       short i,il = 4;        i = 0;       for(m=0;m<size;++m) {          fprintf(fp," %18.11e",bp->x[m]);          i = i + 1;          if(i == il) {             fprintf(fp,"\n");             i = 0;          }       }        i = 0;       for(m=0;m<size;++m) {          fprintf(fp," %18.11e",bp->y[m]);          i = i + 1;          if(i == il) {             fprintf(fp,"\n");             i = 0;          }       }        i = 0;       for(m=0;m<size;++m) {          fprintf(fp," %18.11e",bp->z[m]);          i = i + 1;          if(i == il) {             fprintf(fp,"\n");             i = 0;          }       }     }     fclose(fp);     return(0); }  
