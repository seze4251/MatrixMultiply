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
#include <assert.h>
#include "matrixfix.h"


int main(int argc, const char * argv[]) {
    //Test all basic Functions
    const int m = 3, n = 4 , p = 5 ;
    clock_t start_t, end_t, total_t;
    matrix * mtxA, * mtxB, * mtxC, * mtxTest;
    
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    mtxC = newMatrix(n, p);
    mtxTest = newMatrix(n, p);
    
    if (!mtxA || !mtxB || !mtxC) {
        printf("One of the matrixes is Null");
        return -1;
    }
    
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
    
    // Test Baseline: Matrix Mulplicaiton
       start_t = clock();
    
    if (!matrixProduct(mtxA, mtxB, mtxC) ) {
        int err = matrixProduct(mtxA, mtxB, mtxC);
        printf("Matrix Multiplication Baseline, Error Code: %d \n", err);
    }
    
     end_t = clock();
    
    printf("Matrix C =[A][B]:\n");
    printMatrix(mtxC);
    printf("Total time taken by CPU for Baseline: %lu\n\n", total_t);
    
    // Test Improvement 1
    // *************************************************
    
    start_t = clock();
    
    if (!matrixProductFix1(mtxA, mtxB, mtxTest) ) {
        int err = matrixProduct1(mtxA, mtxB, mtxC);
        printf("Matrix Multiplication Fix 1, Error Code: %d \n", err);
    }
    
    end_t = clock();
    
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    if(! subtractMatrix(mtxC, mtxTest)) {
        printf("\n Matrix Product Fix 1 incorrect \n");
        
    }
    
    printf("Total time taken by CPU for Case1: %lu\n\n", total_t);
    
    
    
    // Test Improvement 2
    // *************************************************
    
    start_t = clock();
    
    if (!matrixProductFix2(mtxA, mtxB, mtxTest) ) {
        int err = matrixProductFix2(mtxA, mtxB, mtxC);
        printf("Matrix Multiplication Fix 2, Error Code: %d \n", err);
    }
    
    end_t = clock();
    
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    if(! subtractMatrix(mtxC, mtxTest)) {
        printf("\n Matrix Product Fix 2 incorrect \n");
        
    }
    
    printf("Total time taken by CPU for Case2: %lu\n\n", total_t);
    
    
    // Test Improvement 3
    // *************************************************
    start_t = clock();
    
    if (!matrixProductFix3(mtxA, mtxB, mtxTest) ) {
        int err = matrixProductFix3(mtxA, mtxB, mtxC);
        printf("Matrix Multiplication Fix 3, Error Code: %d \n", err);
    }
    
    end_t = clock();
    
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    if(! subtractMatrix(mtxC, mtxTest)) {
        printf("\n Matrix Product Fix 3 incorrect \n");
        
    }
    
    printf("Total time taken by CPU for Case3: %lu\n\n", total_t);
    
    
    
    // Free all Memory
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
    deleteMatrix(mtxTest);

    return 0;
}
