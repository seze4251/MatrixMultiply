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
#include <unistd.h>

// Master Logic
void handleMasterInit(matrix * mtxA, int * trash, int place[][2], int load, int m, int tagA, MPI_Status status, MPI_Request req []);
void handleMasterBody(matrix * mtxC, int place[][2], MPI_Status status);
void handleMasterRequestMore(matrix * mtxA, int trash [], int place[][2], int load, int m, int tagA, int n, MPI_Status status, MPI_Request req []);
void handleMasterCompute(matrix * mtxA, matrix * mtxB, matrix * mtxC, int place[][2]);
void handleMasterFinishShort(int trash [], int tagFinilize, int nprocs, MPI_Request req []);
void handleMasterFinishLong(matrix * mtxC, int nprocs, int trash [], int place[][2], int hasData, int tagC, int tagFinilize, \
                            MPI_Status status, MPI_Request req []);
void finish(int trash [], int tagFinilize, int nprocs, MPI_Request req []);
// Slave Logic

int handleSlaveInit(matrix * mtxA, matrix * mtxC, int m, int * trash, int serverRank, int tagInit, int tagA, int tagFinilize, int myrank, MPI_Status status);
void handleSlaveBody(matrix * mtxA_one, matrix * mtxA_two, matrix * mtxB, matrix * mtxC_one, matrix * mtxC_two, int serverRank, int tagA, int tagC, int tagFinilize, int tagMoreData, int m, int myrank);
void handleServerFinish(matrix * mtxA, matrix * mtxB, matrix * mtxC, int n, int p, double starttime, double endtime, double totaltime);
int * setPlace( int place[][2], int status);
int * findPlace(int place[][2], int status);
void printPlace(int place[][2], int nprocs);


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
            if (n > 0 && m > 0 && p > 0 && load > 0 && load <= n && load > 1) {
                break;
            } else {
                printf("Please Enter Three Numbers that Are greater than 0, load must be >= 2 \n");
            }
        }
        buff[0] = n; buff[1] = m; buff[2] = p; buff[3] = load;
    }
    mpi_error = MPI_Bcast(buff, 4, MPI_INT, serverRank, MPI_COMM_WORLD); // MPI Bcast blocks until Everyone is here
    n = buff[0]; m = buff[1]; p = buff[2]; load = buff[3];
    matrix * mtxA, * mtxC, * mtxA_one, * mtxA_two, * mtxC_one, * mtxC_two;
    matrix * mtxB = newMatrix(m, p);
    
    if (myrank == serverRank) {
        mtxA = newMatrix(n, m);
        mtxC = newMatrix(n, p);
        randomizeMatrix(mtxA);
        randomizeMatrix(mtxB);
        starttime = MPI_Wtime();
    } else {
        mtxA_one = newMatrix(4 * load, m);
        mtxA_two = newMatrix(4 * load, m);
        mtxC_one = newMatrix(4 * load, p);
        mtxC_two = newMatrix(4 * load, p);
    }
    
    
    // Broadcast B
    MPI_Request req [nprocs];
    mpi_error = MPI_Ibcast(mtxB -> data[0], m*p, MPI_DOUBLE, serverRank, MPI_COMM_WORLD, req);
    
    // Status Variable
    MPI_Status status;
    // Loop Counter
    int i = 0;
    int flag, tagA = 1, tagC = 7, tagInit = 3, tagMoreData = 4, tagFinilize = 5;
    int place [nprocs][2], trash [1];
    trash[0] = -1;
    
    if (myrank == serverRank) {
        int j;
        for (j = 0; j < 2; j++) {
            for (i = 0; i < nprocs; i++) {
                
                if (i == 0 && j == 0) {
                    place[i][j] = 0;
                } else {
                    place[i][j] = -1;
                }
            }
        }
    }
    
    // Get matrix B B
    if (myrank != serverRank) {
        MPI_Wait(req, MPI_STATUS_IGNORE);
    }
    
    
    while (1) {
        if (myrank == serverRank) {
            // Master
            if (place[0][0] != n) {
                MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, & flag, & status);
                
                if (flag == 1) {
                    if (status.MPI_TAG == tagInit ) {
                        handleMasterInit(mtxA, trash, place, load, m, tagA, status, req);
                        
                    } else if ( status.MPI_TAG == tagC ) {
                        // Master Handle Matrix C message
                        handleMasterBody(mtxC, place, status);
                        
                    } else if(status.MPI_TAG == tagMoreData) {
                        // Master Handle Request for more data
                        handleMasterRequestMore(mtxA, trash, place, load, m, tagA, n, status, req);
                        
                    } else {
                        // Master Log Unexpected Tag
                        printf("Error: Log not implemented yet, Message Tag inncorrect \n");
                    }
                    
                } else if( flag == 0) {
                    // Master Compute
                    handleMasterCompute(mtxA, mtxB, mtxC, place);
                }
            } else {
                // Master Finish
                printf("MASTER: ENTER FINISH\n");
                int hasData = 0, j;
                
                for (i = 1; i < nprocs; i++) {
                    for (j = 0; j < 2; j++) {
                        if (place[i][j] != -1) {
                            hasData += 1;
                        }
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
                int checkExit = handleSlaveInit(mtxA_one, mtxC_one, m, trash, serverRank, tagInit, tagA, tagFinilize, myrank, status);
                if (checkExit) {
                    break;
                }
            }
            
            handleSlaveBody(mtxA_one, mtxA_two, mtxB, mtxC_one, mtxC_two, serverRank, tagA, tagC, tagFinilize, tagMoreData, m, myrank);
            break;
        }
    }
    
    printf("I rank %d made it out of the infinite loop\n", myrank);
    if (myrank == serverRank) {
        handleServerFinish(mtxA, mtxB, mtxC, n, p, starttime, endtime, totaltime);
    }
    
    if (myrank == serverRank) {
        // Release Heap Memory and Terminate MPI
        deleteMatrix(mtxA);
        // printf("Deleted Test Matrix A rank: %d\n",myrank);
        deleteMatrix(mtxB);
        //printf("Deleted Test Matrix B rank: %d\n",myrank);
        deleteMatrix(mtxC);
        
    } else {
        // Release Heap Memory and Terminate MPI
        deleteMatrix(mtxA_one);
        deleteMatrix(mtxA_two);
        deleteMatrix(mtxC_one);
        deleteMatrix(mtxC_two);
        deleteMatrix(mtxB);
    }
    
    MPI_Finalize();
    printf("Closed MPI rank: %d\n", myrank);
    return 0;
}


/*
 * Start Sub Function Definitions
 *
 */


void handleMasterInit(matrix * mtxA, int * trash, int place[][2], int load, int m, int tagA, MPI_Status status, MPI_Request req []) {
    int mpi_error;
    printf("SERVER: Initilize Message from Process %d \n",status.MPI_SOURCE);
    mpi_error = MPI_Irecv(trash, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, req + 1);
    mpi_error = MPI_Isend(mtxA -> data[place[0][0]], load*m, MPI_DOUBLE, status.MPI_SOURCE, tagA, MPI_COMM_WORLD, req);
    
    place[status.MPI_SOURCE][0] = place[0][0];
    place[0][0] += load;
    
    //Free Request arrive Request
    MPI_Request_free(req + 1);
}

// Returns a pointer to the Lowest NON -1 entry of place
// If both are 0, print an error
int * findPlace(int place[][2], int status) {
    int * holder;
    if (place[status][0] != -1 && place[status][0] < place[status][1]) {
        holder = & place[status][0];
    } else if( place[status][1] != -1 && place[status][0] > place[status][1]) {
        holder = & place[status][1];
    } else {
        printf("Function Find lowest Place requires both entries be non 0\n");
    }
    return holder;
}

void handleMasterBody(matrix * mtxC, int place[][2], MPI_Status status) {
    int count, mpi_error, * holder;
    mpi_error = MPI_Get_count(&status, MPI_DOUBLE, &count);
    holder = findPlace(place, status.MPI_SOURCE);
    
    // Blocking Recv
    mpi_error = MPI_Recv(mtxC -> data[*holder], count, MPI_DOUBLE, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
    *holder = -1;
}

// Return either place with a 0 or the lowest place
int * setPlace( int place[][2], int status) {
    int * holder;
    
    if (place[status][0] < place[status][1]) {
        holder = & place[status][0];
        
    } else if (place[status][0] > place[status][1]) {
        holder = & place[status][1];
        
    } else {
        printf("ERROR: in setPlace\n");
    }
    return holder;
}

void handleMasterRequestMore(matrix * mtxA, int trash[], int place[][2], int load, int m, int tagA, int n, MPI_Status status, MPI_Request req []) {
    int mpi_error;
    
    if (place[0][0] + load >= n) {
        load = n - place[0][0];
    }
    
    // Recive message and throw it away
    mpi_error = MPI_Irecv(trash, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, req);
    MPI_Request_free(req);
    
    // Send the new Message
    mpi_error = MPI_Isend(mtxA -> data[place[0][0]], load*m, MPI_DOUBLE, status.MPI_SOURCE, tagA, MPI_COMM_WORLD, \
                          req);
    
    int * holder = setPlace(place, status.MPI_SOURCE);
    * holder = place[0][0];
    place[0][0] += load;
    MPI_Request_free(req);
}

void handleMasterCompute(matrix * mtxA, matrix * mtxB, matrix * mtxC, int place[][2]) {
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxC, place[0][0], place[0][0] + 1, 0, mtxA->cols, 0, mtxB->cols);
    place[0][0] += 1;
}

void finish(int trash [], int tagFinilize, int nprocs, MPI_Request req []) {
    // Send termination message to all processes
    int i, mpi_error;
    for (i = 1; i < nprocs; i++) {
        mpi_error = MPI_Isend(trash, 1, MPI_INT, i, tagFinilize, MPI_COMM_WORLD, req + i);
    }
    
    // Wait for all messages to go through to avoid seg fault
    MPI_Waitall(nprocs -1, req + 1, MPI_STATUS_IGNORE);
    printf("SERVER: MADE IT PAST !FINAL! WAIT ALL\n");
}

void handleMasterFinishShort(int trash [], int tagFinilize, int nprocs, MPI_Request req []) {
    finish(trash, tagFinilize, nprocs, req);
}

//Cases: Slaves have 1 data or 2 sets of data Wahoo
void handleMasterFinishLong(matrix * mtxC, int nprocs, int trash [], int place[][2], int hasData, int tagC, int tagFinilize,
                            MPI_Status status, MPI_Request req []) {
    int  mpi_error, count, i, j;
    int * holder;
    
    for (i = 0; i < hasData; i++) {
        MPI_Probe(MPI_ANY_SOURCE, tagC, MPI_COMM_WORLD, & status);
        
        if (place[status.MPI_SOURCE][0] < place[status.MPI_SOURCE][1] && place[status.MPI_SOURCE][0] != -1 ) {
            holder = & place[status.MPI_SOURCE][0];
            
        } else if (place[status.MPI_SOURCE][0] > place[status.MPI_SOURCE][1] && place[status.MPI_SOURCE][1] != -1) {
            holder = & place[status.MPI_SOURCE][1];
            
        } else if(place[status.MPI_SOURCE][0] == -1  && place[status.MPI_SOURCE][1] != -1) {
            holder = & place[status.MPI_SOURCE][1];
            
        } else if (place[status.MPI_SOURCE][1] == -1  && place[status.MPI_SOURCE][0] != -1) {
            holder = & place[status.MPI_SOURCE][0];
            
        } else {
            // Both Places are -1 which is bad
            printf("\n Both Place = -1 :( in Finish Long \n");
        }
        
        mpi_error = MPI_Get_count(&status, MPI_DOUBLE, &count);
        
        //Blocking Recv
        mpi_error = MPI_Recv(mtxC -> data[*holder], count, MPI_DOUBLE, status.MPI_SOURCE, tagC, MPI_COMM_WORLD, & status);
        * holder = -1;
    }
    
    finish(trash, tagFinilize, nprocs, req);
}



int handleSlaveInit(matrix * mtxA, matrix * mtxC, int m, int * trash, int serverRank, int tagInit, int tagA, int tagFinilize, int myrank, MPI_Status status) {
    printf("SLAVE: %d Send Initialize\n",myrank);
    // Blocking Send
    int mpi_error = MPI_Send(trash, 1, MPI_INT, serverRank, tagInit, MPI_COMM_WORLD);
    
    int count;
    MPI_Probe(serverRank, MPI_ANY_TAG, MPI_COMM_WORLD, & status);
    
    if (status.MPI_TAG == tagA) {
        mpi_error = MPI_Get_count(&status, MPI_DOUBLE, & count);
        //Blocking Recv
        mpi_error = MPI_Recv(mtxA -> data[0], count, MPI_DOUBLE, serverRank, tagA, MPI_COMM_WORLD, & status);
        
        //Assign Matrix Proper Dimensions
        int rows = count / m;
        mtxA -> rows = rows;
        mtxC -> rows = rows;
        return 0;
        
    } else {
        mpi_error = MPI_Recv(trash, 1, MPI_INT, serverRank, tagFinilize, MPI_COMM_WORLD, & status);
        return 1;
    }
}

void handleSlaveBody(matrix * mtxA_one, matrix * mtxA_two, matrix * mtxB, matrix * mtxC_one, matrix * mtxC_two, int serverRank, int tagA, int tagC, int tagFinilize, int tagMoreData, int m, int myrank) {
    MPI_Request req[3];
    MPI_Status status [2];
    int calc, rem, err, count, mpi_error, i = 0, flag = 0, rows;
    int trash[1];
    trash[0] = -1;
    double ** holder;
    mtxA_two -> rows = mtxA_one -> rows;
    mtxC_two -> rows = mtxA_one -> rows;
    
    // Need to start loop with mtxA_one being full and mtxC_one initialized to 0
    printf("SLAVE: %d entering main body loop\n",myrank);
    while (1) {
        // Determine sizes of data
        calc = mtxA_one -> rows / 2;
        rem = mtxA_one -> rows % 2;
        
        // Ask For More Data
        mpi_error = MPI_Isend(trash, 1, MPI_INT, serverRank, tagMoreData, MPI_COMM_WORLD, req); //req
        
        // Compute First Product --never touch mtxA_one cols
        err = matrixProductCacheObliv(mtxA_one, mtxB, mtxC_one, 0, calc, 0, mtxA_one->cols, 0, mtxB->cols);
        
        // Attempt Recv in mtxA_two
        MPI_Iprobe(serverRank, tagA, MPI_COMM_WORLD, & flag, status);
        
        if (flag == 1) {
            
            mpi_error = MPI_Get_count( status, MPI_DOUBLE, &count);
            mpi_error = MPI_Irecv(mtxA_two -> data[0], count, MPI_DOUBLE, serverRank, tagA, MPI_COMM_WORLD, req + 1); // req + 1
            
        }
        
        // Compute Second Product
        err = matrixProductCacheObliv(mtxA_one, mtxB, mtxC_one, calc, 2*calc + rem, 0, mtxA_one->cols, 0, mtxB->cols);
        
        // Wait for previous send mtxC request to go through
        if (i != 0) {
            MPI_Wait(req+2, MPI_STATUS_IGNORE);
        }
        
        
        // switch pointers
        holder = mtxC_two -> data;
        mtxC_two -> data = mtxC_one -> data;
        
        // Send mtxC_two
        mpi_error = MPI_Isend(mtxC_two -> data[0], mtxC_one -> rows * mtxC_one -> cols, MPI_DOUBLE, serverRank, tagC, MPI_COMM_WORLD, req + 2); // req + 2
        MPI_Wait(req+2, MPI_STATUS_IGNORE);
        
        // Finish switching pointers
        mtxC_one -> data = holder;
        
        // Zero mtxC_one in preperation for new matrix multiplication
        
        zeroMatrix(mtxC_one);
        
        MPI_Wait(req, MPI_STATUS_IGNORE); // Wait for the more data request to be received
        
        // Finish Recive or block Recv
        if (flag == 1) {
            // Finish Recv
            MPI_Wait(req + 1, MPI_STATUS_IGNORE);
            
            
        } else {
            //Blocking Probe -- Need not to touch (status + 0)
            MPI_Probe(serverRank, MPI_ANY_TAG, MPI_COMM_WORLD, status);
            
            if (status[0].MPI_TAG == tagA) {
                mpi_error = MPI_Get_count(status, MPI_DOUBLE, &count);
                // Blocking Recv
                mpi_error = MPI_Recv(mtxA_two -> data[0], count, MPI_DOUBLE, serverRank, tagA, MPI_COMM_WORLD, status + 1);
                
            } else if(status[0].MPI_TAG == tagFinilize) {
                //Finishing Cases for slaves:
                // Normal - have a finishing message with no A waiting
                // Bad - Have a finishing message + have mtxA data waiting
                
                MPI_Wait(req + 2, MPI_STATUS_IGNORE); // Wait on Last C to go through
                MPI_Iprobe(serverRank, tagA, MPI_COMM_WORLD, & flag, status + 1);
                
                if (flag == 1) {
                    // Take Care of Last A
                    printf("Extremely rare case in HandleSlaveBody occured myrank: %d\n", myrank);
                    
                    // Recv A
                    mpi_error = MPI_Get_count(status + 1, MPI_DOUBLE, &count);
                    mpi_error = MPI_Recv(mtxA_one -> data[0], count, MPI_DOUBLE, serverRank, tagA, MPI_COMM_WORLD, status + 1);
                    mtxC_one -> rows = count / m;
                    mtxA_one -> rows = count / m;
                    
                    // Compute Final C
                    err = matrixProductCacheObliv(mtxA_one, mtxB, mtxC_one, 0, mtxA_one->rows, 0, mtxA_one->cols, 0, mtxB->cols);
                    
                    // Blocking Send C
                    mpi_error = MPI_Send(mtxC_one -> data[0], mtxA_one -> rows * mtxC_one -> cols, MPI_DOUBLE, serverRank, tagC, MPI_COMM_WORLD);
                }
                printf("SLAVE: %d About to enter Final Recv\n",myrank);
                mpi_error = MPI_Recv(trash, 1, MPI_INT, serverRank, tagFinilize, MPI_COMM_WORLD, status);
                break;
            }
        }
        
        //Switch Pointers
        holder = mtxA_one -> data;
        mtxA_one -> data = mtxA_two -> data;
        mtxA_two -> data = holder;
        
        //Reset rows and Columns of mtxA_one and two
        rows = count / m;
        mtxC_one -> rows = rows;
        mtxA_one -> rows = rows;
        mtxA_two -> rows = rows;
        mtxC_two -> rows = rows;
        i++;
    }
}




void handleServerFinish(matrix * mtxA, matrix * mtxB, matrix * mtxC, int n, int p, double starttime, double endtime, double totaltime) {
    //Time
    endtime = MPI_Wtime();
    totaltime = endtime - starttime;
    printf("Total Running Time: %5.3f\n",totaltime);
    
    matrix * mtxTest = newMatrix(n, p);
    matrixProductCacheObliv(mtxA, mtxB, mtxTest, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
    
    // Print to File
    /*FILE * file = fopen("OutputParallel","a");
     fprintf(file, " %d \t\t %lu /n",m,total_t);
     fclose(file);
     */
    
    //printf("Completed Test Multiplication \n");
    /* sleep(3);
     // Test Correctness  DEBUG
     printf("Matrix Test: \n");
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

void printPlace(int place[][2], int nprocs) {
    int i, j;
    for (i = 0; i < nprocs; i ++) {
        for (j = 0 ; j < 2; j++) {
            printf("%d\t",place[i][j]);
        }
        printf("\n");
    }
    
    
}




