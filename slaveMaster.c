//
//  slaveMaster.c
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

// Master Logic
void handleMasterInit(matrix * mtxA, int * trash, int place [], int load, int m, int tagA, MPI_Status status, MPI_Request req []);
void handleMasterBody(matrix * mtxA, matrix * mtxC, int place [], int load, int m, int tagA, int n, MPI_Status status, MPI_Request req []);
void handleMasterCompute(matrix * mtxA, matrix * mtxB, matrix * mtxC, int place []);
void handleMasterFinishShort(int trash [], int tagFinilize, int nprocs, MPI_Request req []);
void handleMasterFinishLong(matrix * mtxC, int nprocs, int trash [], int place [], int hasData, int tagC, int tagFinilize, \
                            MPI_Status status, MPI_Request req []);
void finish(int trash [], int tagFinilize, int nprocs, MPI_Request req []);
// Slave Logic
void handleSlaveInit(int * trash, int serverRank, int tagInit, int myrank, MPI_Request req []) ;
void handleSlaveBody(matrix * mtxA, matrix * mtxB, matrix * mtxC, int serverRank, int tagA, int tagC, int m, int myrank, MPI_Request req[], MPI_Status status);
void handleSlaveFinish(int * trash, int serverRank, int tagFinilize, int myrank, MPI_Request req []);
void handleServerFinish(matrix * mtxA, matrix * mtxB, matrix * mtxC, int n, int p, double starttime, double endtime, double totaltime);

int main(int argc, char * argv[]) {
    // Initialize MPI
    MPI_Init(&argc, &argv);
    
    //Initialize Base Vars on all processes
    int nprocs, myrank, mpi_error;
    const int serverRank = 0;
    double starttime, endtime, totaltime;
    
    // Determine # of procs and my rank
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    
    
    // Read in Data
    int buff[4], n, m, p, load;
    
    if (myrank == serverRank) {
        while (1) {
            printf("Please enter 3 ints to dimensionlize the Matrix followed by a int for load distribution (4 total) \n");
            int check = scanf("%d %d %d %d",&n, &m, &p, &load);
            if (n > 0 && m > 0 && p > 0 && load > 0 && load <= n) {
                break;
            } else {
                printf("Please Enter Three Numbers that Are greater than 0 \n");
            }
        }
        buff[0] = n; buff[1] = m; buff[2] = p; buff[3] = load;
    }
    
    mpi_error = MPI_Bcast(buff, 4, MPI_INT, serverRank, MPI_COMM_WORLD); // MPI Bcast blocks until Everyone is here
    n = buff[0]; m = buff[1]; p = buff[2]; load = buff[3];
    matrix * mtxA, * mtxC;
    matrix * mtxB = newMatrix(m, p);
    
    if (myrank == serverRank) {
        mtxA = newMatrix(n, m);
        mtxC = newMatrix(n, p);
        randomizeMatrix(mtxA);
        randomizeMatrix(mtxB);
        starttime = MPI_Wtime();
        
    } else {
        mtxA = newMatrix(4 * load, m);
        mtxC = newMatrix(4 * load, p);
    }
    
    
    // Broadcast B
    MPI_Request req [nprocs];
    mpi_error = MPI_Ibcast(mtxB -> data[0], m*p, MPI_DOUBLE, serverRank, MPI_COMM_WORLD, req);
    
    // Status Variable
    MPI_Status status;
    // Loop Counter
    int i = 0;
    int flag = 0, tagA = 1, tagC = 2, tagInit = 3, tagFinilize = 53;
    int place[nprocs], trash[1];
    trash[0] = 100;
    
    if (myrank == serverRank) {
        for (i = 0; i < nprocs; i++) {
            place[i] = 0;
        }
    }
    
    // Get matrix B B
    if (myrank != serverRank) {
        MPI_Wait(req, MPI_STATUS_IGNORE);
    }
    
    while (1) {
        if (myrank == serverRank) {
            if (place[0] != n) {
                //Master
                MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, & flag, & status);
                
                if (flag == 1) {
                    if (status.MPI_TAG == tagInit ) {
                        handleMasterInit(mtxA, trash, place, load, m, tagA, status, req);
                        
                    } else if ( status.MPI_TAG == tagC ) {
                        handleMasterBody(mtxA, mtxC, place, load, m, tagA, n, status, req);
                        
                    } else {
                        // Some other tag that we are not expecting: Log Error
                        printf("Error: Log not implemented yet, Message Tag inncorrect \n");
                    }
                    
                } else if( flag == 0) {
                    // Compute rows of A and check back for more data
                    handleMasterCompute(mtxA, mtxB, mtxC, place);
                }
            } else {
                // Master End condition
                int hasData = 0;
                
                for (i = 1; i < nprocs; i++) {
                    if (place[i] != 0) {
                        hasData += 1;
                    }
                }
                
                if (!hasData) {
                    // Case 1: Master did all the work (small matrix)
                    handleMasterFinishShort(trash, tagFinilize, nprocs, req);
                    break;
                } else {
                    // Case 2: Others Have data
                    handleMasterFinishLong(mtxC, nprocs, trash, place, hasData, tagC, tagFinilize, status, req);
                    break;
                }
            }
        } else {
            // All Slaves
            if (i == 0) {
                //Initialize Slave
                i = 1;
                handleSlaveInit(trash, serverRank, tagInit, myrank, req);
            }
            
            MPI_Probe(serverRank, MPI_ANY_TAG, MPI_COMM_WORLD, &status); //Blocking probe to not waste CPU time
            
            if (status.MPI_TAG == tagA) {
                handleSlaveBody(mtxA, mtxB, mtxC, serverRank, tagA, tagC, m, myrank, req, status);
                
            } else if (status.MPI_TAG == tagFinilize) {
                handleSlaveFinish(trash, serverRank, tagFinilize, myrank, req);
                break;
            }
        }
    }
    
    printf("I rank %d made it out of the infinite loop\n", myrank);
    if (myrank == serverRank) {
        handleServerFinish(mtxA, mtxB, mtxC, n, p, starttime, endtime, totaltime);
    }
    
    // Release Heap Memory and Terminate MPI
    deleteMatrix(mtxA);
    // printf("Deleted Test Matrix A rank: %d\n",myrank);
    deleteMatrix(mtxB);
    //printf("Deleted Test Matrix B rank: %d\n",myrank);
    deleteMatrix(mtxC);
    //  printf("Deleted Matrix C rank: %d\n", myrank);
    MPI_Finalize();
    printf("Closed MPI rank: %d\n", myrank);
    return 0;
}

/*
 * Start Sub Function Definitions
 *
 */

void handleMasterInit(matrix * mtxA, int * trash, int place [], int load, int m, int tagA, MPI_Status status, MPI_Request req []) {
    int mpi_error;
    printf("SERVER: Initilize Message from Process %d \n",status.MPI_SOURCE);
    mpi_error = MPI_Irecv(trash, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, req + status.MPI_SOURCE);
    mpi_error = MPI_Isend(mtxA -> data[place[0]], load*m, MPI_DOUBLE, status.MPI_SOURCE, tagA, MPI_COMM_WORLD, \
                          req );
    place[status.MPI_SOURCE] = place[0];
    place[0] += load;
    
    //Free Request arrive Request
    MPI_Request_free(req + status.MPI_SOURCE);
}

void handleMasterBody(matrix * mtxA, matrix * mtxC, int place [], int load, int m, int tagA, int n, MPI_Status status, MPI_Request req []) {
    int count, mpi_error;
    mpi_error = MPI_Get_count(&status, MPI_DOUBLE, &count);
    mpi_error = MPI_Irecv(mtxC -> data[place[status.MPI_SOURCE]], count, MPI_DOUBLE, status.MPI_SOURCE,
                          status.MPI_TAG, MPI_COMM_WORLD, req + status.MPI_SOURCE);
    
    if (place[0] + load >= n) {
        load = n - place[0];
    }
    
    
    // Send the new Message
    mpi_error = MPI_Isend(mtxA -> data[place[0]], load*m, MPI_DOUBLE, status.MPI_SOURCE, tagA, MPI_COMM_WORLD, \
                          req);
    
    place[status.MPI_SOURCE] = place[0];
    place[0] += load;
    
    MPI_Wait(req + status.MPI_SOURCE, MPI_STATUS_IGNORE);
    MPI_Request_free(req);
}

void handleMasterCompute(matrix * mtxA, matrix * mtxB, matrix * mtxC, int place []) {
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxC, place[0], place[0] + 1, 0, mtxA->cols, 0, mtxB->cols);
    place[0] += 1;
}

void finish(int trash [], int tagFinilize, int nprocs, MPI_Request req []) {
    // Send termination message to all processes
    int i, mpi_error;
    for (i = 1; i < nprocs; i++) {
        mpi_error = MPI_Isend(trash, 1, MPI_INT, i, tagFinilize, MPI_COMM_WORLD, req + i);
    }
    
    // Wait for all messages to go through to avoid seg fault
    MPI_Waitall(nprocs -1, req + 1, MPI_STATUS_IGNORE);
}

void handleMasterFinishShort(int trash [], int tagFinilize, int nprocs, MPI_Request req []) {
    finish(trash, tagFinilize, nprocs, req);
}

void handleMasterFinishLong(matrix * mtxC, int nprocs, int trash [], int place [], int hasData, int tagC, int tagFinilize,
                            MPI_Status status, MPI_Request req []) {
    int IHasData [hasData], mpi_error, i;
    
    for (i = 1; i < nprocs; i++) {
        if (place[i] != 0) {
            IHasData[i] = i;
        }
    }
    
    i = 0;
    while (i < hasData ) {
        MPI_Probe(MPI_ANY_SOURCE, tagC, MPI_COMM_WORLD, & status);
        int count, mpi_error;
        mpi_error = MPI_Get_count(&status, MPI_DOUBLE, &count);
        
        mpi_error = MPI_Irecv(mtxC -> data[place[status.MPI_SOURCE]], count, MPI_DOUBLE, status.MPI_SOURCE,
                              tagC, MPI_COMM_WORLD, req);
        //Wait for Data to go in
        MPI_Wait(req, MPI_STATUS_IGNORE);
        i++;
    }
    finish(trash, tagFinilize, nprocs, req);
}



void handleSlaveInit(int * trash, int serverRank, int tagInit, int myrank, MPI_Request req []) {
    int mpi_error = MPI_Isend(trash, 1, MPI_INT, serverRank, tagInit, MPI_COMM_WORLD, req);
    MPI_Wait(req, MPI_STATUS_IGNORE);
}

void handleSlaveBody(matrix * mtxA, matrix * mtxB, matrix * mtxC, int serverRank, int tagA, int tagC, int m, int myrank, MPI_Request req[], MPI_Status status) {
    int count, mpi_error;
    mpi_error = MPI_Get_count( &status, MPI_DOUBLE, &count);
    
    // Recive matrix A
    mpi_error = MPI_Irecv(mtxA -> data[0], count, MPI_DOUBLE, serverRank, tagA, MPI_COMM_WORLD, req);
    zeroMatrix(mtxC);
    int rows = count / m;
    mtxA -> rows = rows;
    mtxC -> rows = rows;
    
    MPI_Wait(req, MPI_STATUS_IGNORE);
    
    // Compute Product
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxC, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
    
    mpi_error = MPI_Isend(mtxC -> data[0], mtxC -> rows * mtxC -> cols, MPI_DOUBLE, serverRank, tagC, MPI_COMM_WORLD, req);
    MPI_Wait(req, MPI_STATUS_IGNORE);
}


void handleSlaveFinish(int * trash, int serverRank, int tagFinilize, int myrank, MPI_Request req []) {
    int mpi_error;
    mpi_error = MPI_Irecv(trash, 1, MPI_INT, serverRank, tagFinilize, MPI_COMM_WORLD, req);
    MPI_Wait(req, MPI_STATUS_IGNORE);
}

void handleServerFinish(matrix * mtxA, matrix * mtxB, matrix * mtxC, int n, int p, double starttime, double endtime, double totaltime) {
    //Time
    endtime = MPI_Wtime();
    totaltime = endtime - starttime;
    printf("Total Running Time: %5.3f\n",totaltime);
    
    // Print to File
    /*FILE * file = fopen("OutputParallel","a");
     fprintf(file, " %d \t\t %lu /n",m,total_t);
     fclose(file);
     */
    
    matrix * mtxTest = newMatrix(n, p);
    matrixProductCacheObliv(mtxA, mtxB, mtxTest, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
    printf("Completed Test Multiplication \n");
    
    // Test Correctness  DEBUG
    /* printf("Matrix Test: \n");
     printMatrix(mtxTest);
     printf("Matrix C: \n");
     printMatrix(mtxC);
     */
    
    if (subtractMatrix(mtxC, mtxTest)) {
        printf("\n Matrix Product Cache Obliv incorrect \n");
    } else {
        printf(" \n Matrix Product Correct!!!!!!!! \n");
    }
    deleteMatrix(mtxTest);
    printf("Deleted Test Matrix Server\n");
}






