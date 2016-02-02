//
//  matrixDoublePointer.c
//  MatrixDoublePointer
//
//  Created by Seth on 1/24/16.
//  Copyright © 2016 Seth. All rights reserved.
//

#include "matrixDoublePointer.h"

//
//  matrix.c
//  Matrix_Multiplication
//
//  Created by Seth on 1/19/16.
//  Copyright © 2016 Seth. All rights reserved.
//

#include "matrixDoublePointer.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

void matrixInit() {
    srand((unsigned int) time(NULL));
}


double ** newMatrix(int rows, int cols) {
    
    // Create a column vector that holds pointers to the start of each row
    double ** mtx = (double **) calloc(rows, sizeof(double *));
    
    // The first position on A points to the start of the vector length row*col
    mtx[0] = (double*) calloc(rows * cols, sizeof(double));
    
    // Sets every A value to be pointing to the 0th position of the next row
    int n;
    for ( n=1; n<rows; ++n) {
        mtx[n] = mtx[n-1]+ cols;
    }
    return mtx;
    
    // A[row][col] returns the A(row,col)
}


// Randomizes the values in a square matrix
// int size = rows * cols
void randomizeMatrix(int size, double ** mtx) {
    int i, j;
    for ( i = 0; i < size; i++) {
        for ( j = 0; j < size; j++) {
            *(mtx[i] +j) = rand();
        }
        
    }
}

void constMatrix(int size, double ** mtx, int num) {
    int i, j;
    for ( i = 0; i < size; i++) {
        for ( j = 0; j < size; j++) {
            *(mtx[i] +j) = num;
        }
        
    }
}



// Deletes the Matrix
void deleteMatrix(double ** mtx) {
    free(mtx[0]);
    free(mtx);
}


// Prints the matrix
void printMatrix(double ** mtx, int rows, int cols) {
    printf("\n");
    int i, j;
    for ( i = 0; i < rows; i++) {
        for ( j = 0; j < cols ; j++) {
            printf("%11.2f", mtx[i][j]);
        }
        printf("\n");
    }
}

// Copy a Matrix
void copyMatrix(int rows, int cols, double ** mtxOrig, double ** mtxCopy) {
    int i, j;
    for ( i = 0; i < rows; i++) {
        for ( j = 0; j < cols; j++) {
            mtxCopy[i][j] = mtxOrig[i][j];
        }
    }
}

// Subtract a Matrix
bool subtractMatrix(int rows, int cols, double ** mtx1, double ** mtx2) {
    int i, j;
    
    for ( i = 0; i < rows; i++) {
        for ( j = 0; j < cols; j++) {
            if (mtx1[i][j] - mtx2[i][j] != 0) {
                return false;
            }
        }
    }
    return true;
}





// Takes two square matrixes and multiplies them together and returns the product
// Length is the # of rows = # of columns
double ** matrixProduct(double ** mtxA, double ** mtxB, double ** mtxC, int length) {
    // If mtxA, mtxB, mtxC is NULL, return NULL
    if (!mtxA || !mtxB || !mtxC) {
        return NULL;
    }
    int i, j, k;
    
    // Perform the matrix multiplication
    for (i = 0; i < length; i++) {
        for ( j = 0; j < length ; j++) {
            for ( k = 0; k < length ; k++) {
                mtxC[i][j] = mtxC[i][j] + mtxA[i][k] * mtxB[k][j];
            }
        }
    }
    
    // Return the result
    return mtxC;
    
}

double ** matrixProductFix1(double ** mtxA, double ** mtxB, double ** mtxC, int length) {
    // If mtxA, mtxB, mtxC is NULL, return NULL
    if (!mtxA || !mtxB || !mtxC) {
        return NULL;
    }
    
    double space00, space01, space10, space11;
    int i, j, k;
    
    // Perform the matrix multiplication
    for ( i = 0; i < length; i+=2) {
        for ( j = 0; j < length ; j+=2) {
            space00 = space01 = space10 = space11 = 0;
            for ( k = 0; k < length ; k++) {
                space00 += mtxA[i][k]*mtxB[k][j];
                space01 += mtxA[i][k]*mtxB[k][j+1];
                space10 += mtxA[i +1][k]*mtxB[k][j];
                space11 += mtxA[i +1][k]*mtxB[k][j+1];
                
                
            }
            mtxC[i][j] = space00;
            mtxC[i][j+1] = space01;
            mtxC[i+1][j] = space10;
            mtxC[i+1][j+1] = space11;
        }
    }
    
    // Return the result
    return mtxC;
    
}

double ** matrixProductFix2(double ** mtxA, double ** mtxB, double ** mtxC, int length) {
    // If mtxA, mtxB, mtxC is NULL, return NULL
    if (!mtxA || !mtxB || !mtxC) {
        return NULL;
    }
    
    double space00, space01, space10, space11;
    int ib = 20;
    int ii, j, i, k;
    
    // Perform the matrix multiplication
    for ( ii = 0; ii < length; ii+=ib) {
        for ( j = 0; j < length ; j+=2) {
            for ( i = ii; i < ii + ib; i+=2) {
                space00 = space01 = space10 = space11 = 0;
                for ( k = 0; k < length ; k++) {
                    space00 += mtxA[i][k]*mtxB[k][j];
                    space01 += mtxA[i][k]*mtxB[k][j+1];
                    space10 += mtxA[i +1][k]*mtxB[k][j];
                    space11 += mtxA[i +1][k]*mtxB[k][j+1];
                    
                    
                }
                mtxC[i][j] = space00;
                mtxC[i][j+1] = space01;
                mtxC[i+1][j] = space10;
                mtxC[i+1][j+1] = space11;
            }
        }
    }
    
    // Return the result
    return mtxC;
    
}


double ** matrixProductFix3(double ** mtxA, double ** mtxB, double ** mtxC, int length) {
    // If mtxA, mtxB, mtxC is NULL, return NULL
    if (!mtxA || !mtxB || !mtxC) {
        return NULL;
    }
    
    double space00, space01, space10, space11;
    int ib = 20;
    int kb = 50;
    int ii, kk, j, i, k;
    // Perform the matrix multiplication
    
    for (ii = 0; ii < length; ii+=ib) {
        for (kk = 0; kk < length; kk += kb) {
            for (j = 0; j < length ; j+=2) {
                for (i = ii; i < ii + ib; i+=2) {
                    if (kk == 0) {
                        space00 = space01 = space10 = space11 = 0;
                    } else {
                        space00 = mtxC[i][j];
                        space01 = mtxC[i][j+1];
                        space10 = mtxC[i+1][j];
                        space11 = mtxC[i+1][j+1];
                    }
                    
                    for (k = kk; k < kk + kb ; k++) {
                        space00 += mtxA[i][k]*mtxB[k][j];
                        space01 += mtxA[i][k]*mtxB[k][j+1];
                        space10 += mtxA[i +1][k]*mtxB[k][j];
                        space11 += mtxA[i +1][k]*mtxB[k][j+1];
                        
                        
                    }
                    mtxC[i][j] = space00;
                    mtxC[i][j+1] = space01;
                    mtxC[i+1][j] = space10;
                    mtxC[i+1][j+1] = space11;
                }
            }
        }
    }
    
    // Return the result
    return mtxC;
    
}

int max( int a, int b, int c) {
    if (a > b && a > c) {
        return a;
    } else if ( b > c) {
        return b;
    } else {
        return c;
    }
}

void randomizeMatrixNotSquare(int rows, int cols, double ** mtx) {
    int i, j;
    for ( i = 0; i < rows; i++) {
        for ( j = 0; j < cols; j++) {
            mtx[i][j] = rand();
        }
        
    }
}

void matrixProductNotSquare(double ** mtxA, double ** mtxB, double ** mtxC, int n, int m, int p) {
    
    int i, j, k;
    
    // Perform the matrix multiplication
    for (i = 0; i < n; i++) {
        for ( j = 0; j < p ; j++) {
            for ( k = 0; k < m ; k++) {
                mtxC[i][j] += mtxA[i][k] * mtxB[k][j];
            }
        }
    }
}

void constMatrixNonSquare(int row, int col, double ** mtx, int num) {
    int i, j;
    for ( i = 0; i < row; i++) {
        for ( j = 0; j < col; j++) {
            mtx[i][j] = num;
        }
        
    }
}


void matrixProductCacheObliv(double ** mtxA, double ** mtxB, double ** mtxC, int n, int m, int p, int startRA, int endRA, int startM, int endM, int startCB, int endCB) {
    // The way to call this is to pass in pointers to A,B, and C, size ints (n, m, p) and 0s for everything ELSE
    
    if (endRA == 0 || endM == 0 || endCB == 0) {
        endRA = n;
        endM = m;
        endCB = p;
    }
    
    if ( (endRA - startRA) * (endM - startM) + (endM - startM) * (endCB -startCB) <= 800) {
        
        int i, j, k;
        
        // Perform the matrix multiplication
        for (i = startRA; i < endRA; i++) {
            for ( k = 0; k < (endM - startM) ; k++) {
                for ( j = startCB; j < endCB ; j++) {
                    mtxC[i][j] +=  mtxA[i][k] * mtxB[k][j];
                }
            }
        }
        
        // Return the result
        return;
        
        
    } else {
        // A_n,m  B_m,p
        // Case 1
        
        if ( max( (endRA - startRA), (endM - startM), (endCB -startCB) ) ==  (endRA - startRA)) {
            // Split A horizonally
            int c = floor((endRA - startRA) /2);
            
            
            matrixProductCacheObliv(mtxA, mtxB, mtxC, n, m, p, startRA, c, startM, endM, startCB, endCB);
            matrixProductCacheObliv(mtxA, mtxB, mtxC, n, m, p, c + 1, endRA, startM, endM, startCB, endCB);
            return;
            
        } else         if (max( (endRA - startRA), (endM - startM), (endCB -startCB) ) ==  (endCB -startCB)) {
            // Split B Vertically
            int c = floor((endCB -startCB) /2);
            
            matrixProductCacheObliv(mtxA, mtxB, mtxC, n, m, p, startRA, endRA, startM, endM, startCB, c);
            matrixProductCacheObliv(mtxA, mtxB, mtxC, n, m, p, startRA, endRA, startM, endM, c+1, endCB);
            return;
            
        } else {
            // Split A vertically and B horizontally
            
            int c = floor((endCB -startCB) /2);
            int d = floor((endRA - startRA) /2);
            
            matrixProductCacheObliv(mtxA, mtxB, mtxC, n, m, p, startRA, d, startM, c - startCB, startCB, c);
            matrixProductCacheObliv(mtxA, mtxB, mtxC, n, m, p, d+1, endRA, endCB - c , endM, c+1, endCB);
            return;
        }
    }
}



