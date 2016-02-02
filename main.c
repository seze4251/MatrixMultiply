//
//  main.c
//  MatrixDoublePointer
//
//  Created by Seth on 1/24/16.
//  Copyright © 2016 Seth. All rights reserved.
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrixDoublePointer.h"
#include <stdbool.h>

// Pass in parameters size
int main(int argc, const char * argv[]) {
    /*
     if (argc != 2) {
     printf("CLI the size of the square matrix");
     return -1;
     }
     int size = atoi(argv[1]);
     printf("Square Matrix %d by %d\n",size,size);
     
    // Test Primary Implementation of matrixDoublePointer.c
    */
    double ** mtxA, **mtxB, ** mtxC;
    const int size = 5;
    matrixInit();
    /*
    mtxA = newMatrix(size, size);
    mtxB = newMatrix(size, size);
    mtxC = newMatrix(size, size);
    
    if (!mtxA || !mtxB || !mtxC) {
        printf("One of the matrixes is Null");
        return -1;
    }
    
    //Print Matrixes - expect 0s
    printf("Expected all zeros, size %d", size);
    printMatrix(mtxA, size, size);
    printf("\n\n");
    
    //Const matrix and print it
    int value = 5;
    constMatrix(size, mtxA, value);
    constMatrix(size, mtxB, value -3);
    printf("Matrix A:\n");
    printMatrix(mtxA, size, size);
    printf("\n\n");
    printf("Matrix B:\n");
    printMatrix(mtxB, size, size);
    printf("\n\n");
    
    // Test: Matrix Mulplicaiton
    matrixProduct(mtxA, mtxB, mtxC, size);
    printf("Matrix C =[A][B]:\n");
    printMatrix(mtxC, size, size);
    printf("\n Test Optimization Cases\n");
    
    // Free matrixes
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
    
    // Test All Cases
    // *************************************************
    // Base Case
     */
    clock_t start_t, end_t, total_t;
    int testSize = 10;
    double ** mtxCorrect;
    /*
    // Declare Matrixes
    mtxA = newMatrix(testSize, testSize);
    mtxB = newMatrix(testSize, testSize);
    mtxCorrect = newMatrix(testSize, testSize);
    
    // Randomize Matrixes
    randomizeMatrix(testSize, mtxA);
    randomizeMatrix(testSize, mtxB);
    
    printf("Long Loop Starting 1\n");
    
    // Start Clock
    start_t = clock();
    
    // Multiply Matrixes using first implementation
    matrixProduct(mtxA, mtxB, mtxCorrect, testSize);
    
    //  End clock
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    

    printf("Total time taken by CPU for Base Case: %lu\n\n", total_t);
    
    // Test Improvement 1
      // *************************************************
    double ** mtxTest1 = newMatrix(testSize, testSize);
    
    // Start Clock
    printf("Long Loop Starting 2\n");
    start_t = clock();
    
    // Multiply Matrixes using first implementation
    matrixProductFix1(mtxA, mtxB, mtxTest1, testSize);
    
    //  End clock
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    if(! subtractMatrix(testSize, testSize, mtxTest1, mtxCorrect)) {
        printf("\n Matrix Product Version 1 incorrect\n");
        
    }
    
    printf("Total time taken by CPU for Case1: %lu\n\n", total_t);
    
    
    
    // Test Improvement 2
      // *************************************************
    double ** mtxTest2 = newMatrix(testSize, testSize);
    
    // Start Clock
    printf("Long Loop Starting 3\n");
    start_t = clock();
    
    // Multiply Matrixes using first implementation
    matrixProductFix2(mtxA, mtxB, mtxTest2, testSize);
    
    //  End clock
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    if(! subtractMatrix(testSize, testSize, mtxTest2, mtxCorrect)) {
        printf("\n Matrix Product Version 2 incorrect\n");
        
    }
    
    printf("Total time taken by CPU for Case2: %lu\n\n", total_t);
    
    
    // Test Improvement 3
      // *************************************************
    double ** mtxTest3 = newMatrix(testSize, testSize);
    printf("Long Loop Starting 4\n");
    // Start Clock
    start_t = clock();
    
    // Multiply Matrixes using first implementation
    matrixProductFix3(mtxA, mtxB, mtxTest3, testSize);

    //  End clock
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    if(! subtractMatrix(testSize, testSize, mtxTest3, mtxCorrect)) {
        printf("\n Matrix Product Version 3 incorrect\n");
        
    }
    
    printf("\n");
    printf("Total time taken by CPU for Case3: %lu\n", total_t);
    
    
    // Free all Memory
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxCorrect);
    deleteMatrix(mtxTest1);
    deleteMatrix(mtxTest2);
    deleteMatrix(mtxTest3);
    
    // Large test of fix 3
    int sizeLarge = 10000;
    mtxA = newMatrix(sizeLarge, sizeLarge);
    mtxB = newMatrix(sizeLarge, sizeLarge);
    mtxC = newMatrix(sizeLarge, sizeLarge);
    randomizeMatrix(testSize, mtxA);
    randomizeMatrix(testSize, mtxB);
    
    printf("\n Starting the 10,000 by 10,000 Fix 3 \n");
    // Start Clock
    start_t = clock();
    
    // Multiply Matrixes using first implementation
    matrixProductFix3(mtxA, mtxB, mtxC, sizeLarge);
    
    //  End clock
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    printf("\n");
    printf("Total time taken by CPU for Case3: %lu\n", total_t);
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC); */
    
    // Test Cache Obliv with square Matrix
    testSize = 50;
    
    mtxA = newMatrix(testSize,testSize);
    mtxB = newMatrix(testSize,testSize);
    mtxC = newMatrix(testSize,testSize);
    double ** mtxCOR = newMatrix(testSize,testSize);
    
    randomizeMatrix(testSize, mtxA);
    randomizeMatrix(testSize, mtxB);
    
    printf("\n Starting Cache Obliv Test with Square Matrix \n");
    // Start Clock
    start_t = clock();
    
    // Multiply Matrixes
    matrixProductCacheObliv(mtxA, mtxB, mtxC, testSize, testSize, testSize, 0, 0, 0, 0, 0, 0);
    
    //  End clock
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    matrixProduct(mtxA, mtxB, mtxCOR, testSize);
    
    if(! subtractMatrix(testSize, testSize, mtxC, mtxCOR)) {
        printf("\n Cache Obliv Incorect \n");
        
    }
    
    printf("\n");
    printf("Total time taken by CPU for cache obliv Square: %lu\n", total_t);
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
    deleteMatrix(mtxCOR);
    
    
    // Test Cache Obliv Not square
    int n = 5;
    int m = 6;
    int p = 7;
    
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    mtxC = newMatrix(n, p);
    mtxCOR = newMatrix(n, p);
    
    //printf("\n Print MTXA not square \n");
    //printMatrix(mtxA, n, m);
    
    constMatrixNonSquare(n, m , mtxA,5);
    constMatrixNonSquare(m, p, mtxB,5);
    
    
    printf("\n Starting Cache Obliv Test with NON Square Matrix \n");
    // Start Clock
    start_t = clock();
    
    // Multiply Matrixes using first implementation
    matrixProductCacheObliv(mtxA, mtxB, mtxC, n, m, p, 0, 0, 0, 0, 0, 0);
    
    //  End clock
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    matrixProductNotSquare(mtxA, mtxB, mtxCOR, n, m, p);
    
    //printf("\n Print MTXC not square \n");
    //printMatrix(mtxC, n, p);
    
    //printf("\n Print MTXCOR not square \n");
    //printMatrix(mtxCOR, n, p);
    
    if(! subtractMatrix(n, p, mtxC, mtxCOR)) {
        printf("\n Cache Obliv Incorect \n");
    }
    
    printf("\n");
    printf("Total time taken by CPU for Cache Obliv Non Square: %lu\n", total_t);
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
    deleteMatrix(mtxCOR);
    
    
}



