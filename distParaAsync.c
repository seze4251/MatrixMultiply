//
//  distParaAsync.c
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


int main(int argc, char * argv[]) {
    // Initialize MPI
    MPI_Init(&argc, &argv);
    
    //Initialize Base Vars on all processes
    int nprocs, myrank, tagA = 1, tagC = 2, mpi_error;
    const int serverRank = 0;
    double starttime, endtime, totaltime;
    MPI_Status status;
    matrix * mtxA, * mtxC;
    
    // Determine # of procs and my rank
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    
    // Ancient Implementation - unpredictable on different machines
    /*
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
     */
    
    // Read in Data
    int buff[3], n, m, p;
    
    if (myrank == serverRank) {
        while (1) {
            printf("Please enter 3 ints to dimensionlize the Matrix\n");
            int check = scanf("%d %d %d",&n, &m, &p);
            if (n > 0 && m > 0 && p > 0) {
                break;
            } else {
                printf("Please Enter Three Numbers that Are greater than 0 \n");
            }
        }
        buff[0] = n; buff[1] = m; buff[2] = p;
    }
    mpi_error = MPI_Bcast(buff, 3, MPI_INT, serverRank, MPI_COMM_WORLD); // MPI Bcast blocks until Everyone is here
    n = buff[0]; m = buff[1]; p = buff[2];
    
    
    // Determine necissary variables for splitting by # of processes
    int rem = n % nprocs;
    int scatterSize = n / nprocs;
    int i, length;
    
    // Everyone makes matrixes at the same time!
    matrix * mtxB = newMatrix(m, p);
    
    if (myrank == serverRank) {
        mtxA = newMatrix(n, m);
        mtxC = newMatrix(n, p);
        randomizeMatrix(mtxA);
        randomizeMatrix(mtxB);
        starttime = MPI_Wtime();
        
    } else if (myrank == nprocs -1 ) {
        mtxA = newMatrix(scatterSize + rem, m);
        mtxC = newMatrix(scatterSize + rem, p);
        
        
    } else {
        mtxA = newMatrix(scatterSize, m);
        mtxC = newMatrix(scatterSize, p);
    }
    
    // Broadcast B
    MPI_Request req [nprocs];
    mpi_error = MPI_Ibcast(mtxB -> data[0], m*p, MPI_DOUBLE, serverRank, MPI_COMM_WORLD, req);
    
    // Sends A
    if (myrank == serverRank) {
        
        length = scatterSize;
        for (i = 1; i < nprocs; i++) {
            if (i == nprocs - 1) {
                length = scatterSize + rem;
            }
            mpi_error = MPI_Isend(mtxA -> data[ scatterSize * i], length*m, MPI_DOUBLE, i, tagA, \
                                  MPI_COMM_WORLD, (req+i));
        }
        
        // Set rows of A and C so that serverProcess does work!
        mtxA -> rows = scatterSize;
        mtxC -> rows = scatterSize;
        
    } else {
        if (myrank == nprocs -1 ) {
            mpi_error = MPI_Irecv(mtxA -> data[0], (scatterSize + rem)*m, MPI_DOUBLE, serverRank, tagA, MPI_COMM_WORLD, \
                                  req + myrank);
            
        } else {
            mpi_error = MPI_Irecv(mtxA -> data[0], scatterSize*m, MPI_DOUBLE, serverRank, tagA, MPI_COMM_WORLD, req + myrank);
        }
    }
    
    if ( myrank != serverRank) {
        MPI_Wait(req, MPI_STATUS_IGNORE);
        MPI_Wait(req + myrank, MPI_STATUS_IGNORE);
    }
    
    printf("MADE IT To the Main Multiplication on PROCESS: %d\n", myrank);
    // Perform Matrix Multplicaiton
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxC, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
    printf("Post Multiplicaiton: Process Number:%d  Error Code: %d\n", myrank, err); //Debug
    
    if (myrank == serverRank) {
        
        for ( i = 1; i < nprocs; i++) {
            MPI_Request_free(req+i);
            printf("Free requests on Server for: i = %d\n",i);
        }
        // Resize rows of A and C
        mtxA -> rows = n;
        mtxC -> rows = n;
        length = scatterSize;
        
        for (i = 1; i < nprocs; i++) {
            if (i == nprocs -1) {
                length = scatterSize + rem;
            }
            mpi_error =  MPI_Irecv(mtxC -> data [scatterSize * i], length * p, MPI_DOUBLE, i, tagC, \
                                   MPI_COMM_WORLD, req + i);
        }
        printf("Finished All Recives on Server Rank");
        
    } else {
        if (myrank == nprocs -1) {
            MPI_Isend(mtxC -> data[0], (scatterSize + rem) * p, MPI_DOUBLE, serverRank, tagC, MPI_COMM_WORLD, req+myrank);
          	MPI_Wait(req + myrank, MPI_STATUS_IGNORE);

        } else {
            MPI_Isend(mtxC -> data[0], scatterSize * p, MPI_DOUBLE, serverRank, tagC, MPI_COMM_WORLD, req + myrank);
           	MPI_Wait(req + myrank, MPI_STATUS_IGNORE);
        }
        printf("Finished All Sends on Non Server Process\n");
    }
    
    if (myrank == serverRank) {
        
        endtime = MPI_Wtime();
        totaltime = endtime - starttime;
        printf("Total Running Time: %5.3f",totaltime);
        
        // Print to File
        /*FILE * file = fopen("OutputParallel","a");
         fprintf(file, " %d \t\t %lu /n",m,total_t);
         fclose(file);
         */
        
        matrix * mtxTest = newMatrix(n, p);
        printf("Created Test Matrix\n");
        
        matrixProductCacheObliv(mtxA, mtxB, mtxTest, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
        printf("Completed Test Multiplication \n");
        MPI_Waitall(nprocs - 1, req + 1, MPI_STATUS_IGNORE);  // This wait all should be higher for timing but it hides communication being down here
        printf("Finshed Waitall in Server Process\n");
        if (subtractMatrix(mtxC, mtxTest)) {
            printf("\n Matrix Product Cache Obliv incorrect \n");
        } else {
            printf(" \n Matrix Product Correct!!!!!!!! \n");
        }
        deleteMatrix(mtxTest);
        printf("Deleted Test Matrix Server\n");
        
    }
    

    deleteMatrix(mtxA);
    printf("Deleted Test Matrix A rank: %d\n",myrank);
    deleteMatrix(mtxB);
    printf("Deleted Test Matrix B rank: %d\n",myrank);
    deleteMatrix(mtxC);
    printf("Deleted Matrix C rank: %d\n", myrank);
    MPI_Finalize();
    printf("Closed MPI rank: %d\n", myrank);
    return 0;
}



