//
//  mainFix.c
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
void testFix3(int n, int m, int p, bool output);
void testFix2(int n, int m, int p, bool output);
void testFix1(int n, int m, int p, bool output);
void testBaseFun(int n, int m, int p, bool output);
void testFUNCTIONS( int n, int m, int p, bool output);


int main(int argc, const char * argv[]) {
    // Initialize
    matrixInit();
    int n, m, p;
    bool output;
    
    // Speed Test of square Matrix
    int size = 2000;
    output = false;
    n = m = p = size;
    printf("Test Oblivious Started\n");
    testCacheObliv(n, m, p, output);
    printf("Test 3 Started\n");
    testFix3(n, m, p, output);
    printf("Base Started\n");
    testBaseFun(n, m, p, output);
    printf("Test 1 Started\n");
    testFix1(n, m, p, output);
    printf("Test 2 Started\n");
    testFix2(n, m, p, output);
    
    
    
    // Test Cache Obliv Parallel
    
    
    
    /*
     testFUNCTIONS(n, m, p, output);
     testBaseFun(n, m, p, output);
     testFix1(n, m, p, output);
     testFix2(n, m, p, output);
     testFix3(n, m, p, output);
     testCacheObliv(n, m, p, output);
     testCacheOblivNonBlocking(n, m, p, output);
     testCacheOblivBlocking(n, m, p, output);
     testCacheOblivOpenMP(n, m, p, output);
     */
    
    return 0;
}

void testFUNCTIONS( int n, int m, int p, bool output) {
    matrix * mtxA, * mtxB, * mtxC, * mtxTest;
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    mtxC = newMatrix(n, p);
    mtxTest = newMatrix(n, p);
    
    if (!mtxA || !mtxB || !mtxC) {
        printf("One of the matrixes is Null");
    }
    
    
    if (output) {
        //Print Matrixes - expect 0s
        printf("Expected all zeros, size %d by %d", n, m);
        printMatrix(mtxA);
        printf("\n\n");
        
        //Const matrix and print it
        const int value = 5;
        
        constMatrix(mtxA, value);
        constMatrix(mtxB, value -3);
        
        // Print Matrix to visually inspect
        printf("Matrix A:\n");
        printMatrix(mtxA);
        printf("\n\n");
        
        printf("Matrix B:\n");
        printMatrix(mtxB);
        printf("\n\n");
    }
    
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
    deleteMatrix(mtxTest);
    
}

void testBaseFun(int n, int m, int p, bool output) {
    clock_t start_t, end_t, total_t;
    matrix * mtxA, * mtxB, * mtxC, * mtxTest;
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    mtxC = newMatrix(n, p);
    randomizeMatrix(mtxA);
    randomizeMatrix(mtxB);
    mtxTest = newMatrix(n, p);
    
    start_t = clock();
    
    if (matrixProduct(mtxA, mtxB, mtxC) ) {
        zeroMatrix(mtxC);
        int err = matrixProduct(mtxA, mtxB, mtxC);
        printf("Matrix Multiplication Baseline, Error Code: %d \n", err);
    }
    
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    printf("Total time taken by CPU for Baseline: %lu\n\n", total_t);
    
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

void testFix1(int n, int m, int p, bool output) {
    clock_t start_t, end_t, total_t;
    matrix * mtxA, * mtxB, * mtxC, * mtxTest;
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    mtxC = newMatrix(n, p);
    mtxTest = newMatrix(n, p);
    randomizeMatrix(mtxA);
    randomizeMatrix(mtxB);
    
    matrixProduct(mtxA, mtxB, mtxC);
    
    start_t = clock();
    
    if (matrixProductFix1(mtxA, mtxB, mtxTest) ) {
        zeroMatrix(mtxTest);
        int err = matrixProductFix1(mtxA, mtxB, mtxTest);
        printf("Matrix Multiplication Fix 1, Error Code: %d \n", err);
    }
    
    end_t = clock();
    
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    if(subtractMatrix(mtxC, mtxTest)) {
        printf("\n Matrix Product Fix 1 incorrect \n");
    }
    
    printf("Total time taken by CPU for Case1: %lu\n\n", total_t);
    
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



void testFix2(int n, int m, int p, bool output) {
    clock_t start_t, end_t, total_t;
    matrix * mtxA, * mtxB, * mtxC, * mtxTest;
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    mtxC = newMatrix(n, p);
    randomizeMatrix(mtxA);
    randomizeMatrix(mtxB);
    mtxTest = newMatrix(n, p);
    
    matrixProduct(mtxA, mtxB, mtxC);
    
    start_t = clock();
    
    if (matrixProductFix2(mtxA, mtxB, mtxTest) ) {
        zeroMatrix(mtxTest);
        int err = matrixProductFix2(mtxA, mtxB, mtxTest);
        printf("Matrix Multiplication Fix 2, Error Code: %d \n", err);
    }
    
    end_t = clock();
    
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    if(subtractMatrix(mtxC, mtxTest)) {
        printf("\n Matrix Product Fix 2 incorrect \n");
    }
    
    printf("Total time taken by CPU for Case2: %lu\n\n", total_t);
    
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

void testFix3(int n, int m, int p, bool output) {
    clock_t start_t, end_t, total_t;
    matrix * mtxA, * mtxB, * mtxC, * mtxTest;
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    mtxC = newMatrix(n, p);
    mtxTest = newMatrix(n, p);
    randomizeMatrix(mtxA);
    randomizeMatrix(mtxB);
    
    matrixProduct(mtxA, mtxB, mtxC);
    
    
    start_t = clock();
    
    if (matrixProductFix3(mtxA, mtxB, mtxTest) ) {
        zeroMatrix(mtxTest);
        int err = matrixProductFix3(mtxA, mtxB, mtxTest);
        printf("Matrix Multiplication Fix 3, Error Code: %d \n", err);
    }
    
    end_t = clock();
    
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    
    if(subtractMatrix(mtxC, mtxTest)) {
        printf("\n Matrix Product Fix 3 incorrect \n");
    }
    
    printf("Total time taken by CPU for Case3: %lu\n\n", total_t);
    
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

