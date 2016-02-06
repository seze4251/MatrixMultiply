//
//  mainParallel
//  MatrixFix
//
//  Created by Seth on 2/2/16.
//  Copyright © 2016 Seth. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrixFix.h"
#include <stdbool.h>

void testCacheObliv(int n, int m, int p, bool output);
void testCacheOblivNonBlocking(int n, int m, int p, bool output);
void testCacheOblivBlocking(int n, int m, int p, bool output);
void testCacheOblivOpenMP(int n, int m, int p, bool output);


int main(int argc, const char * argv[]) {
    // Initialize
    matrixInit();
    int n, m, p;
    bool output;
  
    // Test Cache Obliv Parallel
    
    
    
    /*
     testCacheObliv(n, m, p, output);
     testCacheOblivNonBlocking(n, m, p, output);
     testCacheOblivBlocking(n, m, p, output);
     testCacheOblivOpenMP(n, m, p, output);
     */
    
    return 0;
}

void testCacheObliv(int n, int m, int p, bool output) {
    // Declare Clock
    clock_t start_t, end_t, total_t;
    // Declare and Init Matrixies
    matrix * mtxA, * mtxB, * mtxC, * mtxTest;
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    mtxC = newMatrix(n, p);
    mtxTest = newMatrix(n, p);
    constMatrix(mtxA,3);
    constMatrix(mtxB,5);
    matrixProduct(mtxA, mtxB, mtxC);
    
    // Perform Operation
    start_t = clock();
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxTest, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
    end_t = clock();
    
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    printf("Error code: %d\n",err);
    
    // Test Correctness
    if(subtractMatrix(mtxC, mtxTest)) {
        printf("\n Matrix Product Cache Obliv incorrect \n");
    }
    
    printf("Total time taken by CPU for Cache Obliv: %lu\n\n", total_t);
    
    if ( output ) {
        printf("Matrix A:\n");
        printMatrix(mtxA);
        printf("\n\n");
        printf("Matrix B:\n");
        printMatrix(mtxB);
        printf("\n\n");
        printf("mtx C \n");
        printMatrix(mtxC);
        printf("mtxTest\n");
        printMatrix(mtxTest);
    }
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
    deleteMatrix(mtxTest);
}

void testCacheOblivBlocking(int n, int m, int p, bool output) {
    // Declare Clock
    clock_t start_t, end_t, total_t;
    // Declare and Init Matrixies
    matrix * mtxA, * mtxB, * mtxC, * mtxTest;
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    mtxC = newMatrix(n, p);
    mtxTest = newMatrix(n, p);
    constMatrix(mtxA,3);
    constMatrix(mtxB,5);
    matrixProduct(mtxA, mtxB, mtxC);
    
    // Perform Operation
    start_t = clock();
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxTest, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
    end_t = clock();
    
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    printf("Error code: %d\n",err);
    
    // Test Correctness
    if(subtractMatrix(mtxC, mtxTest)) {
        printf("\n Matrix Product Cache Obliv incorrect \n");
    }
    
    printf("Total time taken by CPU for Cache Obliv: %lu\n\n", total_t);
    
    if ( output ) {
        printf("Matrix A:\n");
        printMatrix(mtxA);
        printf("\n\n");
        printf("Matrix B:\n");
        printMatrix(mtxB);
        printf("\n\n");
        printf("mtx C \n");
        printMatrix(mtxC);
        printf("mtxTest\n");
        printMatrix(mtxTest);
    }
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
    deleteMatrix(mtxTest);
}

void testCacheOblivNonBlocking(int n, int m, int p, bool output) {
    // Declare Clock
    clock_t start_t, end_t, total_t;
    // Declare and Init Matrixies
    matrix * mtxA, * mtxB, * mtxC, * mtxTest;
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    mtxC = newMatrix(n, p);
    mtxTest = newMatrix(n, p);
    constMatrix(mtxA,3);
    constMatrix(mtxB,5);
    matrixProduct(mtxA, mtxB, mtxC);
    
    // Perform Operation
    start_t = clock();
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxTest, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
    end_t = clock();
    
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    printf("Error code: %d\n",err);
    
    // Test Correctness
    if(subtractMatrix(mtxC, mtxTest)) {
        printf("\n Matrix Product Cache Obliv incorrect \n");
    }
    
    printf("Total time taken by CPU for Cache Obliv: %lu\n\n", total_t);
    
    if ( output ) {
        printf("Matrix A:\n");
        printMatrix(mtxA);
        printf("\n\n");
        printf("Matrix B:\n");
        printMatrix(mtxB);
        printf("\n\n");
        printf("mtx C \n");
        printMatrix(mtxC);
        printf("mtxTest\n");
        printMatrix(mtxTest);
    }
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
    deleteMatrix(mtxTest);
}

void testCacheOblivOpenMP(int n, int m, int p, bool output) {
    // Declare Clock
    clock_t start_t, end_t, total_t;
    // Declare and Init Matrixies
    matrix * mtxA, * mtxB, * mtxC, * mtxTest;
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    mtxC = newMatrix(n, p);
    mtxTest = newMatrix(n, p);
    constMatrix(mtxA,3);
    constMatrix(mtxB,5);
    matrixProduct(mtxA, mtxB, mtxC);
    
    // Perform Operation
    start_t = clock();
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxTest, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
    end_t = clock();
    
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    printf("Error code: %d\n",err);
    
    // Test Correctness
    if(subtractMatrix(mtxC, mtxTest)) {
        printf("\n Matrix Product Cache Obliv incorrect \n");
    }
    
    printf("Total time taken by CPU for Cache Obliv: %lu\n\n", total_t);
    
    if ( output ) {
        printf("Matrix A:\n");
        printMatrix(mtxA);
        printf("\n\n");
        printf("Matrix B:\n");
        printMatrix(mtxB);
        printf("\n\n");
        printf("mtx C \n");
        printMatrix(mtxC);
        printf("mtxTest\n");
        printMatrix(mtxTest);
    }
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
    deleteMatrix(mtxTest);
}
