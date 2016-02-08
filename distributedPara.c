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



// Requires command line input of m, n, and p
// Assume Square Matrix Divisible by 4, n = m = p and divisible by 4
int main(int argc, const char * argv[]) {
    
    //Initialize Base Vars
    int nprocs, myrank;
    const int serverRank = 0;
    MPI_Status status;
    
    // Initialize MPI
    MPI_Init(&argc, &argv);
    
    // Determine # of procs and my rank
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    
    // Make sure Command Line Input is acceptable
    if (argc != 4) {
        printf("Please enter 3 integers on the command line for n, m, and p where A[n,m] and B is [m,p] \n");
        return -1;
    }
    
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int p = atoi(argv[3]);
    
    if (n < 1 || m < 1 || p < 1) {
        printf("n, m, and p must be greater or equal to 1");
        return -2;
    }
    
    int rem = m % nprocs;
    int scatterSize = m/nprocs;
    
    //Server Process
    if (myrank == serverRank) {
        
        // Declare and Randomize
        matrix * mtxA = newMatrix(n, m);
        matrix * mtxB = newMatrix(m, p);
        matrix * mtxC = newmatrix(n, p);
        randomizeMatrix(mtxA);
        randomizeMatrix(mtxB);
        // Take Server Process cut
        
        // Broadcast 3 Matrixes to all processes
        MPI_Bcast(mtxB -> data[0], m*p -1, MPI_DOUBLE, serverRank, MPI_COMM_WORLD);
        
        if (rem == 0) {
            // Scatter A
            int scatterSize = m/nprocs;
            MPI_Scatter(mtxA -> data[scatterSize -1], scatterSize -1, MPI_DOUBLE, mtxA -> data[0], m/nprocs -1, serverRank, MPI_COMM_WORLD);
        } else {
            // Scatter A without Seg Fault
            int i;
            for (int i = 1; i < nprocs - 1; i++) {
                MPI_Scatter(mtxA -> data[scatterSize -1], scatterSize -1, MPI_DOUBLE, mtxA -> data[0], m/nprocs -1, serverRank, MPI_COMM_WORLD);
            }
            MPI_Send(mtxA -> data[ scatterSize * (nprocs -1)], scatterSize - rem -1, MPI_DOUBLE, nprocs -1, tag,
                     MPI_COMM_WORLD)
        }
        
    } else {
        // All other Processes not the Server Process will Recive the buffer
        matrix * mtxB = newMatrix(m, p);
        
        if (myrank == nprocs -1 ) {
            matrix * mtxA = newMatrix(scatterSize - rem -1, m);
            matrix * mtxC = newmatrix(scatterSize - rem -1, p);
            
        } else {
            matrix * mtxA = newmatrix(scatterSize -1, p);
            matrix * mtxC = newmatrix(scatterSize -1, p);
            
        }
        
        MPI_Recv(mtx, sizeof(* mtxA), MPI_Matrix,
                 serverRank, tag, MPI_COMM_WORLD, status);
        
        MPI_Recv(* mtxB, sizeof(* mtxB), MPI_Matrix,
                 serverRank, tag, MPI_COMM_WORLD, status);
        
    }
    
    // Perform Matrix Multplicaiton
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxC, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
    printf("Error Code: %d, From Process %d", err, myrank);
    
    // Send Matrix C back to Server Process
    if (myrank == nprocs -1 ) {
        // Send Back Indivdiual Case
        
    } else {
        // Send Back All other cases
        MPI_Gather( mtxC -> data[0],   , MPI_DOUBLE, mtxC -> data [0],   , MPI_DOUBLE, serverRank, MPI_COMM_WORLD);
        
    }
    
    MPI_Barrier;
    
    if (myrank == serverRank) {
        mtxTest = newMatrix(n, p);
        matrixProductCacheObliv(mtxA, mtxB, mtxTest, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
        // Test Correctness
        if(subtractMatrix(mtxC, mtxTest)) {
            printf("\n Matrix Product Cache Obliv incorrect \n");
        }
        
    }
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
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
    
    randomizeMatrix(mtxA);
    randomizeMatrix(mtxB);
    matrixProduct(mtxA, mtxB, mtxC);
    
    // Perform Operation
    start_t = clock();
    
    end_t = clock();
    
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    printf("Error code: %d\n",err);
    
    
    
    
}


