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
    int n, m, p;
    while (1) {
        int check = scanf("%d %d %d",&n, &m, &p);
        if (n > 0 && m > 0 && p > 0) {
            break;
        } else {
            printf("Please Enter Three Numbers that Are greater than 0 \n");
        }
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
        //       constMatrix(mtxA,3);
        //     constMatrix(mtxB,4);
        //	printf("Server Process mtxA\n");
        //	printMatrix(mtxA);
        //	printf("\n\n");
    }
    clock_t start_t = clock();
    
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
    
    /*	if (myrank == 1) {
     printf("mtxA \n");
     printMatrix(mtxA);
     printf("\n mtxB\n");
     printMatrix(mtxB);
     }
     */
    // Perform Matrix Multplicaiton
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxC, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
    printf("Process Number:%d  Error Code: %d\n", myrank, err);
    
    if (myrank == serverRank) {
        mtxA -> rows = n;
        mtxC -> rows = n;
        printf("I made it this Far\n");
        
        length = scatterSize;
        for (i = 1; i < nprocs; i++) {
            if (i == nprocs -1) {
                length = scatterSize + rem;
                printf("Do I make it here\n");
            }
            MPI_Status newS;
            printf("I am sure this line is my error i = %d scattersize = %d length = %d  rem = %d\n",i,scatterSize,length,rem);
            int mpiER =  MPI_Recv(mtxC -> data [scatterSize + scatterSize * (i-1)], length * p, MPI_DOUBLE, i, i+tagC, MPI_COMM_WORLD, &newS);
            printf("WAHOOOOOO\n");
        }
        printf("Did I work?\n");
    } else {
        if (myrank == nprocs -1) {
            MPI_Send(mtxC -> data[0], (scatterSize + rem) * p, MPI_DOUBLE, serverRank, myrank+tagC, MPI_COMM_WORLD);
            printf("Sure did\n");
        } else {
            MPI_Send(mtxC -> data[0], scatterSize * p, MPI_DOUBLE, serverRank, myrank+tagC, MPI_COMM_WORLD);
            printf("Sure Can!\n");
        }
        
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    printf("made it here222222 Process Number: %d\n", myrank);
    if (myrank == serverRank) {
        // End Clock
        printf("Start Clock: %lu\n",start_t);
        clock_t end_t = clock();
        printf("End Clock: %lu\n " ,end_t);
        clock_t total_t = (double) (end_t - start_t) / CLOCKS_PER_SEC;
        printf("Total Time: %lu \n", total_t);
        // Print to File
        FILE * file = fopen("OutputParallel","a");
        fprintf(file, " %d \t\t %lu /n",m,total_t);
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



