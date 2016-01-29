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
     */
    // Test Primary Implementation of matrixDoublePointer.c
    
    double ** mtxA, **mtxB, ** mtxC;
    const int size = 5;
    matrixInit();
    
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
    clock_t start_t, end_t, total_t;
    int testSize = 1000;
    double ** mtxCorrect;
    
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
}
