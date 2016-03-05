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
int main(int argc, char * argv[]) {
    
    //Initialize Base Vars
    int nprocs, myrank, tagA = 1, tagC = 2;
    const int serverRank = 0;
    MPI_Status status;
    matrix * mtxA, * mtxC;
    
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
    
    int rem = n % nprocs;
    int scatterSize = n / nprocs;
    int i, length;
    
    // Set up server Ranks Vars
    matrix * mtxB = newMatrix(m, p);
    if (myrank == serverRank) {
        mtxA = newMatrix(n, m);
        mtxC = newMatrix(n, p);
        randomizeMatrix(mtxA);
        randomizeMatrix(mtxB);
    }
    double starttime, endtime, totaltime;
    if (myrank == serverRank) {
        starttime = MPI_Wtime();
    }
    
    MPI_Bcast(mtxB -> data[0], m*p, MPI_DOUBLE, serverRank, MPI_COMM_WORLD);
    
    //Server Process uses a blocking Send to distribute matrix <- Improvement to use non-blocking but complicates
    if (myrank == serverRank) {
        // Length describes how many rows are being sent
        length = scatterSize;
        for (i = 1; i < nprocs; i++) {
            if (i == nprocs - 1) {
                length = scatterSize + rem;
            }
            
            MPI_Send(mtxA -> data[ scatterSize + scatterSize * (i -1)], length*m, MPI_DOUBLE, i, tagA,
                     MPI_COMM_WORLD);
        }
        
        // Reset mtxA and mtxC
        mtxA -> rows = scatterSize;
        mtxC -> rows = scatterSize;
        
    } else {
        // All other Processes not the Server Process will Recive the buffer
        
        if (myrank == nprocs -1 ) {
            mtxA = newMatrix(scatterSize + rem, m);
            mtxC = newMatrix(scatterSize + rem, p);
            MPI_Recv(mtxA -> data[0], (scatterSize + rem)*m, MPI_DOUBLE, serverRank, tagA, MPI_COMM_WORLD, &status);
            
        } else {
            mtxA = newMatrix(scatterSize, m);
            mtxC = newMatrix(scatterSize, p);
            MPI_Recv(mtxA -> data[0], scatterSize*m, MPI_DOUBLE, serverRank, tagA, MPI_COMM_WORLD, &status);
        }
    }
    
    // Perform Matrix Multplicaiton
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxC, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
    printf("Process Number:%d  Error Code: %d\n", myrank, err);
    
    if (myrank == serverRank) {
        mtxA -> rows = n;
        mtxC -> rows = n;
        length = scatterSize;
        
        for (i = 1; i < nprocs; i++) {
            if (i == nprocs -1) {
                length = scatterSize + rem;
            }
            
            MPI_Status newS;
            int mpiER =  MPI_Recv(mtxC -> data [scatterSize + scatterSize * (i-1)], length * p, MPI_DOUBLE, i, i+tagC, MPI_COMM_WORLD, &newS);
        }
    } else {
        if (myrank == nprocs -1) {
            MPI_Send(mtxC -> data[0], (scatterSize + rem) * p, MPI_DOUBLE, serverRank, myrank+tagC, MPI_COMM_WORLD);
            
        } else {
            MPI_Send(mtxC -> data[0], scatterSize * p, MPI_DOUBLE, serverRank, myrank+tagC, MPI_COMM_WORLD);
        }
        
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (myrank == serverRank) {
        endtime = MPI_Wtime();
        totaltime = endtime - starttime;
        printf("Total Running Time: %5.3f",totaltime);
        
        // Print to File
        FILE * file = fopen("OutputParallel","a");
        fprintf(file, " %d \t\t %5.4f /n",m,totaltime);
        fclose(file);
        
        matrix * mtxTest = newMatrix(n, p);
        matrixProductCacheObliv(mtxA, mtxB, mtxTest, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
        // Test Correctness
        /*
         printf("Matrix Test: \n");
         printMatrix(mtxTest);
         printf("Matrix C: \n");
         printMatrix(mtxC);
         */
        if(subtractMatrix(mtxC, mtxTest)) {
            printf("\n Matrix Product Cache Obliv incorrect \n");
        }
        deleteMatrix(mtxTest);
    }
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
    MPI_Finalize();
    return 0;
}



