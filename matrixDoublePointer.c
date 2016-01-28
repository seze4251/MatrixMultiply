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

void matrixInit() {
    srand((unsigned int) time(NULL));
}


double ** newMatrix(int rows, int cols) {
    
    // Create a column vector that holds pointers to the start of each row
    double ** mtx = (double **) calloc(rows, sizeof(double *));
    
    // The first position on A points to the start of the vector length row*col
    mtx[0] = (double*) calloc(rows * cols, sizeof(double));
    
    // Sets every A value to be pointing to the 0th position of the next row
    for (int n=1; n<rows; ++n) {
        mtx[n] = mtx[n-1]+ cols;
    }
    return mtx;
    
    // A[row][col] returns the A(row,col)
}


// Randomizes the values in a square matrix
// int size = rows * cols
void randomizeMatrix(int size, double ** mtx) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            *(mtx[i] +j) = rand();
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
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols ; j++) {
            printf("%11.2f", mtx[i][j]);
        }
        printf("\n");
    }
}


// Takes two square matrixes and multiplies them together and returns the product
// Length is the # of rows = # of columns
double ** matrixProduct(double ** mtxA, double ** mtxB, double ** mtxC, int length) {
    // If mtxA, mtxB, mtxC is NULL, return NULL
    if (!mtxA || !mtxB || !mtxC) {
        return NULL;
    }
    
    // Perform the matrix multiplication
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length ; j++) {
            for (int k = 0; k < length ; k++) {
                mtxC[i][j] = mtxC[i][j] + mtxA[i][k] * mtxB[k][j];
            }
        }
    }
    
    // Return the result
    return mtxC;
    
}

// Evans double optimized
double ** matrixProduct2(double ** mtxA, double ** mtxB, double ** mtxC, int length) {
    // If mtxA, mtxB, mtxC is NULL, return NULL
    if (!mtxA || !mtxB || !mtxC) {
        return NULL;
    }
    
    // Perform the matrix multiplication;
    int ii, kk, j, i, k;
    double acc00, acc01, acc10, acc11;
    int ib = 20;
    int kb = 50;
    for (ii = 0; ii < length; ii++) {
        for (kk = 0; kk < length; kk ++) {
            for ( j = 0; j < length; j++) {
                for (i = ii; i < ii + ib; i += 2) {
                    if (kk == 0) {
                        acc00 = acc01 = acc10 = acc11 = 0;
                    } else {
                        acc00 = mtxC[i][j];
                        acc01 = mtxC[i][j+1];
                        acc10 = mtxC[i+1][j];
                        acc11 = mtxC[i+1][j+1];
                    }
                    for (k = kk; k < kk + kb; k++) {
                        acc00 += mtxA[i][k] * mtxB[k][j];
                        acc01 += mtxA[i][k] * mtxB[k][j+1];
                        acc10 += mtxA[i+1][k] * mtxB[k][j];
                        acc11 += mtxA[i+1][k] * mtxB[k][j+1];
                    }
                    mtxC[i][j] = acc00;
                    mtxC[i][j+1] = acc01;
                    mtxC[i+1][j] = acc10;
                    mtxC[i+1][j+1] = acc11;
                }
                
            }
        }
    }
    
    // Return the result
    return mtxC;
    
}




