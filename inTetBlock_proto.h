
int inTetBlock_PTDBlock( struct inPTD_block * );

int inTetBlock( int im, int jm, int km, 
                int *nno, int *nfa, int *nel, int jzone[7][3], int **icon );

int inTetBlock_WriteFluentCase( char *filename,
                                int nno, int nfa, int nel,
                                int jzone[7][3], int *icon,
                                double *x, double *y, double *z );
