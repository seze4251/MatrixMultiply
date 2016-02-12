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
#include <mpi.h>


// Requires command line input of m, n, and p
// Assume Square Matrix Divisible by 4, n = m = p and divisible by 4
int main(int argc, const char * argv[]) {
    
    //Initialize Base Vars
    int nprocs, myrank;
	int tag = 0;  // What is the purpose of this, is this a message identifier that I should do something with?
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
    
    // The goal here is to split matrix A horizontally by the number of processes that are running
    // The sent A will be split evenly with the last process getting a slightly smaller load
    // B will be broadcasted
    // C will be built locally
    int rem = m % nprocs;
    int scatterSize = m/nprocs;
    
    // Problems:  1 - Server Process is not getting a dedicated part of matrix A
    //            Soulution 1: make two aditional matrixes in the server process for A and C to pass into the matrix multiplicatoin
    //            Soulution 2: (Implemented) reset the rows in A and C after the Scatter and Before the broadcast, so the matrix multiplication only works where it supposed to and reset A and C to there intial values before the Gather
    //           2 - I am sometimes using scatterSize -1 and I am not sure if I have to do that or not
                    // I originally thought that I did because data starts at 0 and m/amount of cores does not so I think I do
    //          3 - Figure out Timing problem, may use gprof equivlent for timing
    
    
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
// To Few Arguments Here
MPI_Scatter(mtxA -> data[scatterSize -1], scatterSize -1, MPI_DOUBLE, mtxA -> data[0], m/nprocs -1, serverRank, MPI_COMM_WORLD);
        } else {
            // Scatter A without Seg Fault
            int i;
            for (i = 1; i < nprocs - 1; i++) {
            // To Few Arguments Here Aswell
			MPI_Scatter(mtxA -> data[scatterSize -1], scatterSize -1, MPI_DOUBLE, mtxA -> data[0], m/nprocs -1, serverRank, MPI_COMM_WORLD);
            }
            MPI_Send(mtxA -> data[ scatterSize * (nprocs -1)], scatterSize - rem -1, MPI_DOUBLE, nprocs -1, tag,
                     MPI_COMM_WORLD);
        }
        
        // Reset mtxA and mtxC
        mtxA -> rows = scatterSize -1;
        mtxC -> rows = scatterSize -1;
        
    } else {
        // All other Processes not the Server Process will Recive the buffer
        matrix * mtxB = newMatrix(m, p);
        
        if (myrank == nprocs -1 ) {
            matrix * mtxA = newMatrix(scatterSize - rem -1, m);
            matrix * mtxC = newmatrix(scatterSize - rem -1, p);
            
        } else {
            matrix * mtxA = newmatrix(scatterSize, p);
            matrix * mtxC = newmatrix(scatterSize, p);
            
        }
        
		// Not confident in these commands
        MPI_Recv(mtxC,serverRank, tag, MPI_COMM_WORLD, status);
        
        MPI_Recv(* mtxB, sizeof(* mtxB), MPI_Matrix,
                 serverRank, tag, MPI_COMM_WORLD, status);
        
    }
    
    // Perform Matrix Multplicaiton
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxC, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
    printf("Error Code: %d, From Process %d", err, myrank);
    
    if (myrank == 0) {
        mtxA -> rows = m;
        mtxC -> rows = m;
    }
    
    // Send Matrix C back to Server Process
    if (myrank == nprocs -1 ) {
        // Send Back Indivdiual Case
        
    } else {
        // Send Back All other cases
        MPI_Gather( mtxC -> data[0], (mtxC -> rows * mtxC -> cols) -1, MPI_DOUBLE, mtxC -> data [0], (mtxC -> rows * mtxC -> cols ) -1, MPI_DOUBLE, serverRank, MPI_COMM_WORLD);
        
    }
    
    MPI_Barrier;
	// Why would mtxA and mtxB be undeclared if I used them earlier ??    
    if (myrank == serverRank) {
	    matrix * mtxTest = newMatrix(n, p);
        matrixProductCacheObliv(mtxA, mtxB, mtxTest, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
        // Test Correctness
        if(subtractMatrix(mtxC, mtxTest)) {
            printf("\n Matrix Product Cache Obliv incorrect \n");
        }
		deleteMatrix(mtxTest);
        
    }
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
    return 0;
}



