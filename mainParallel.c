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
#include <mpi.h>

void testCacheObliv(int n, int m, int p, bool output);
void testCacheOblivNonBlocking(int n, int m, int p, bool output);
void testCacheOblivBlocking(int n, int m, int p, bool output);
void testCacheOblivOpenMP(int n, int m, int p, bool output);


// Requires command line input of m, n, and p
// Assume Square Matrix Divisible by 4, n = m = p and divisible by 4
int main(int argc, const char * argv[]) {
    
    if (argc != 4) {
        printf("Please enter 3 integers on the command line for n, m, and p");
        return -1;
    }
    
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int p = atoi(argv[3]);
    
    if (n < 1 || m < 1 || p < 1) {
        printf("n, m, and p must be greater or equal to 1");
        return -2;
    }
    
    int nprocs, myrank, tag = 8;
    const int serverRank = 0;
    MPI_Status status;
    // Initialize MPI
    MPI_Init(&argc, &argv);
    
    // Determine # of procs and my rank
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    
    
    // Declare Matrixes
    matrix * mtxA = newMatrix(n, m);
    matrix * mtxB = newMatrix(m, p);
    matrix * mtxC = newmatrix(n, p);
    
    // Declare MPI_Matrix Data Type
    
    if (myrank == serverRank) {
        // Have Server Process randomize matrix up matrix
        randomizeMatrix(mtxA);
        randomizeMatrix(mtxB);
        
        // Broadcast 3 Matrixes to all processes
        MPI_Bcast(* mtxA, , MPI_Matrix, serverRank, MPI_COMM_WORLD);
        MPI_Bcast(* mtxB, sizeof(* mtxB), MPI_Matrix, serverRank, MPI_COMM_WORLD);
        MPI_Bcast(* mtxC, sizeof(* mtxC), MPI_Matrix, serverRank, MPI_COMM_WORLD);
    }else {
        // Have all other proccesses receive matrix
        matrix * mtxA, * mtxB, * mtxC;
        
        MPI_Recv(* mtxA, sizeof(* mtxA), MPI_Matrix,
                 serverRank, tag, MPI_COMM_WORLD, status);
        
        MPI_Recv(* mtxB, sizeof(* mtxB), MPI_Matrix,
                 serverRank, tag, MPI_COMM_WORLD, status);
        
        MPI_Recv(* mtxC, sizeof(* mtxC), MPI_Matrix,
                 serverRank, tag, MPI_COMM_WORLD, status);
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    /*    // Initialize
     matrixInit();
     int n, m, p;
     bool output;
     
     // Test Cache Obliv Parallel
     
     
     
     
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
    randomizeMatrix(mtxA);
    randomizeMatrix(mtxB);
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
    randomizeMatrix(mtxA);
    randomizeMatrix(mtxB);
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
    randomizeMatrix(mtxA);
    randomizeMatrix(mtxB);
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
    randomizeMatrix(mtxA);
    randomizeMatrix(mtxB);
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
