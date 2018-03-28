
int inPlot3d_ReadFileGrid(char *file, int *nb, struct inPTD_block **blocks);

int inPlot3d_ReadBlockGrid(FILE *fp, struct inPTD_block *bp);

int inPlot3d_ReadNumber(FILE *fp, double *d);

int inPlot3d_WriteFileGrid(char *file, int nb, struct inPTD_block *blocks);

