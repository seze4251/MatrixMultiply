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

// Pass in parameters size
int main(int argc, const char * argv[]) {
    if (argc != 2) {
        printf("Please put the size of the square matrix on the command line \n");
        return -1;
    }
    int size = atoi(argv[1]);
    printf("Square Matrix %d by %d\n",size,size);
    
    // Test Implementation of matrixDoublePointer.c
    double ** mtxA, **mtxB, ** mtxC;
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
    
    //Randomize matrix and print it
    randomizeMatrix(size, mtxA);
    randomizeMatrix(size, mtxB);
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
    
    // Free matrixes
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
    
    // Test 10,000 by 10,000 Baseline
    clock_t start_t, end_t, total_t;
    size = 5;
    // Declare Matrixes
    mtxA = newMatrix(size, size);
    mtxB = newMatrix(size, size);
    mtxC = newMatrix(size, size);

    // Randomize Matrixes
    randomizeMatrix(size, mtxA);
    randomizeMatrix(size, mtxB);
    randomizeMatrix(size, mtxC);
    printf("Long Loop Starting\n");
    
    // Start Clock
    start_t = clock();
    
    // Multiply Matrixes
    matrixProduct(mtxA, mtxB, mtxC, size);
    
    //  End clock
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    printMatrix(mtxC, size, size);
    printf("\n\n");
    printf("Total CPU Clocks: %lu\n", end_t - start_t);
    printf("Total time taken by CPU: %lu\n", total_t);
    printf("Exiting of the program...\n");
    

    
    /*
    // Evans Version
    printf("Long Loop Starting\n");
    
    // Start Clock
    start_t = clock();
    
    // Multiply Matrixes
    matrixProduct2(mtxA, mtxB, mtxC, size);
    
    //  End clock
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    printf("Total time taken by CPU: %lu\n", total_t  );
    printf("Exiting of the program...\n");
    */
    
}
