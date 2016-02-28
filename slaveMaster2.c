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
void handleMasterRequestMore(matrix * mtxA, int place [], int load, int m, int tagMoreData, int n, MPI_Status status, MPI_Request req []);
void handleMasterCompute(matrix * mtxA, matrix * mtxB, matrix * mtxC, int place []);
void handleMasterFinishShort(int trash [], int tagFinilize, int nprocs, MPI_Request req []);
void handleMasterFinishLong(matrix * mtxC, int nprocs, int trash [], int place [], int hasData, int tagC, int tagFinilize, \
                            MPI_Status status, MPI_Request req []);
void finish(int trash [], int tagFinilize, int nprocs, MPI_Request req []);
// Slave Logic
void handleSlaveInit(matrix * mtxA, matrix * mtxC, int m, int * trash, int serverRank, int tagInit, int tagA, int myrank, MPI_Status status, MPI_Request req []);
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
        //printf("SERVER: load 1 = %d", load);
        buff[0] = n; buff[1] = m; buff[2] = p; buff[3] = load;
    }
    mpi_error = MPI_Bcast(buff, 4, MPI_INT, serverRank, MPI_COMM_WORLD); // MPI Bcast blocks until Everyone is here
    n = buff[0]; m = buff[1]; p = buff[2]; load = buff[3];
    matrix * mtxA, * mtxC;
    matrix * mtxB = newMatrix(m, p);
    
    if (myrank == serverRank) {
        mtxA = newMatrix(n, m);
        mtxC = newMatrix(n, p);
        constMatrix(mtxA, 2);
        constMatrix(mtxB, 3);
        //randomizeMatrix(mtxA);
        //randomizeMatrix(mtxB);
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
    int flag = 0, tagA = 1, tagC = 2, tagInit = 3, tagMoreData = 4; tagFinilize = 5;
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
       // printf("Slave: %d made it past recive of B\n", myrank);
    }
    
    while (1) {
        if (myrank == serverRank) {
            if (place[0] != n) {
                //Master
                MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, & flag, & status);
              //  printf("SERVER: Flag Value =  %d \n", flag);
                
                //   printf("Server: Recv Message from rank %d", status.MPI_SOURCE);
                if (flag == 1) {
                    if (status.MPI_TAG == tagInit ) {
                        handleMasterInit(mtxA, trash, place, load, m, tagA, status, req);
                        
                    } else if ( status.MPI_TAG == tagC ) {
                        handleMasterBody(mtxA, mtxC, place, load, m, tagA, n, status, req);
                        
                    } else if(status.MPI_TAG == tagMoreData) {
                        handleMasterRequestMore(mtxA, place, load, m, tagMoreData, n, status, req);
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
                handleSlaveInit(mtxA_one, mtxC_one, m, trash, serverRank, tagInit, tagA, myrank, status, req);
                //printf("Slave: %d made it Past Init \n", myrank);
            }
            
            int calc, rem, err;
            //MPI_Iprobe(serverRank, MPI_ANY_TAG, MPI_COMM_WORLD, & flag, & status);
            /*
             mtxA_one, mtxA_two , mtxC_one, mtxC_two
             1. compute some of mtxA
             2. send a message asking for more data
             3. compute more of mtxA
             */
            int count, mpi_error;
            double holder;
            
            while (1) {
                // Need to start loop with mtxA_one being full and mtxC_one initialized to 0
                // Determine sizes of data
                calc = mtxA_one / 2;
                rem = mtxA_one % 2;
                
                // Ask for More data
                mpi_error = MPI_Isend(trash, 1, MPI_INT, serverRank, tagMoreData, MPI_COMM_WORLD, req);
                MPI_Request_free(req);
                
                // Compute First Product
                err = matrixProductCacheObliv(mtxA_one, mtxB, mtxC_one, 0, calc, 0, mtxA->cols, 0, mtxB->cols);
                
                // Will Recv in mtxA_two
                MPI_Iprobe(serverRank, MPI_ANY_TAG, MPI_COMM_WORLD, & flag, & status);
                if (flag == 1) {
                    if (status.MPI_TAG == tagA) {
                        mpi_error = MPI_Get_count( &status, MPI_DOUBLE, &count);
                        mpi_error = MPI_Irecv(mtxA_two -> data[0], count, MPI_DOUBLE, serverRank, tagA, MPI_COMM_WORLD, req + myrank);
                    } else if (status.MPI_TAG == tagFinilize) {
                        //May need Several Finishes :(
                        handleSlaveFinish(trash, serverRank, tagFinilize, myrank, req); // Decide what to do with this shit!
                        break;
                    }
                }
                
                // Compute Second Product
                err = matrixProductCacheObliv(mtxA_one, mtxB, mtxC_one, calc, 2*calc, 0, mtxA->cols, 0, mtxB->cols);
                
                // switch pointers
                holder = mtxC_two -> data[0];
                mtxC_two -> data[0] = mtxC_one -> data [0];
                
                // Send mtxC_two
                mpi_error = MPI_Isend(mtxC_two -> data[0], mtxC_one -> rows * mtxC_one -> cols, MPI_DOUBLE, serverRank, tagC, MPI_COMM_WORLD, req);
                
                // Give mtxC data
                mtxC_one -> data[0] = holder;
                
                // Zero mtxC_one
                zeroMatrix(mtxC_one);

                // Finish Recive or block Recv
                if (flag == 1) {
                    // Finish Recv
                    MPI_Wait(req + myrank);
                } else {
                    MPI_Probe(serverRank, MPI_ANY_TAG, MPI_COMM_WORLD, & flag, & status);
                    if (status.MPI_TAG == tagA) {
                        mpi_error = MPI_Get_count( &status, MPI_DOUBLE, &count);
                        mpi_error = MPI_Recv(mtxA_two -> data[0], count, MPI_DOUBLE, serverRank, tagA, MPI_COMM_WORLD, status);
                    } else {
                        //May need Several Finishes :(
                        handleSlaveFinish(trash, serverRank, tagFinilize, myrank, req); // Decide what to do with this shit!
                        break;
                    }
                }
                
                holder = mtxA_one -> data [0];
                mtxA_one -> data [0]; = mtxA_two;
                mtxA_two = holder;
                
            }
            break;
        }
    }
    
    // Master -> sends data when asked for it
    
    // I start with a full A  ------ FIRM and C initilized to 0
    // I ask for more A
    // I compute 1/2 of A
    // I check if I have any messages and handle them -> slaveHandleMessage
    // I send half of C?
    // I
    // send 1/3 of C
    // I compute 2/3 of A
    // I check if I have any messages and handle them  -> slaveHandleMessage
    // send 1/3 of C
    // I compute 3/3 of A
    // I send C back
    // send 1/3 of C
    // I wait till I have full A

    
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
   // printf("SERVER: load = %d, m = %d \n", load, m);
    mpi_error = MPI_Isend(mtxA -> data[place[0]], load*m, MPI_DOUBLE, status.MPI_SOURCE, tagA, MPI_COMM_WORLD, \
                          req );
   // printf("SERVER: FINISHED SEND Init\n ");
    place[status.MPI_SOURCE] = place[0];
    place[0] += load;
    
    //Free Request arrive Request
    MPI_Request_free(req + status.MPI_SOURCE);
    //printf("Server: Made it past Free\n");
}

void handleMasterBody(matrix * mtxA, matrix * mtxC, int place [], int load, int m, int tagA, int n, MPI_Status status, MPI_Request req []) {
  //  printf("SERVER: Data message recived from Process %d \n", status.MPI_SOURCE);
    int count, mpi_error;
    mpi_error = MPI_Get_count(&status, MPI_DOUBLE, &count);
    
    mpi_error = MPI_Irecv(mtxC -> data[place[status.MPI_SOURCE]], count, MPI_DOUBLE, status.MPI_SOURCE,
                          status.MPI_TAG, MPI_COMM_WORLD, req + status.MPI_SOURCE);
    
    // Source of Error?
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

void handleMasterRequestMore(matrix * mtxA, int place [], int load, int m, int tagMoreData, int n, MPI_Status status, MPI_Request req []) {
    
    if (place[0] + load >= n) {
        load = n - place[0];
    }

    // Send the new Message
    mpi_error = MPI_Isend(mtxA -> data[place[0]], load*m, MPI_DOUBLE, status.MPI_SOURCE, tagA, MPI_COMM_WORLD, \
                          req);
    
    place[status.MPI_SOURCE] = place[0];
    place[0] += load;
    
    MPI_Request_free(req);
}

void handleMasterCompute(matrix * mtxA, matrix * mtxB, matrix * mtxC, int place []) {
 /*   printf("SERVER: Compute: row = %d\n",place[0]);
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxC, place[0], place[0] + 1, 0, mtxA->cols, 0, mtxB->cols);
    place[0] += 1;
*/}

void finish(int trash [], int tagFinilize, int nprocs, MPI_Request req []) {
    // Send termination message to all processes
    //printf("SERVER: ENTER FINISH LOOP!!!! \n");
    int i, mpi_error;
    for (i = 1; i < nprocs; i++) {
        mpi_error = MPI_Isend(trash, 1, MPI_INT, i, tagFinilize, MPI_COMM_WORLD, req + i);
    }
    
    
    // Wait for all messages to go through to avoid seg fault
    MPI_Waitall(nprocs -1, req + 1, MPI_STATUS_IGNORE);
   // printf("SERVER: MADE IT PAST !FINAL! WAIT ALL");
    
}

void handleMasterFinishShort(int trash [], int tagFinilize, int nprocs, MPI_Request req []) {
    finish(trash, tagFinilize, nprocs, req);
}

void handleMasterFinishLong(matrix * mtxC, int nprocs, int trash [], int place [], int hasData, int tagC, int tagFinilize,
                            MPI_Status status, MPI_Request req []) {
    int IHasData [hasData], mpi_error, i;
   // printf("HasData %d \n",hasData);
    
    for (i = 1; i < nprocs; i++) {
        if (place[i] != 0) {
            IHasData[i] = i;
     //       printf("IHASDATA: %d, i = %d\n",IHasData[i],i);
        }
    }
    
    i = 0;
    //printf("Master Entering Recive Data Loop \n");
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
    //printf("MASTER FINISHED RECIVE LOOP \n");
    finish(trash, tagFinilize, nprocs, req);
}



void handleSlaveInit(matrix * mtxA, matrix * mtxC, int m, int * trash, int serverRank, int tagInit, int tagA, int myrank, MPI_Status status, MPI_Request req []) {
    //printf("Rank %d: Send Initialize\n",myrank);
    int mpi_error = MPI_Isend(trash, 1, MPI_INT, serverRank, tagInit, MPI_COMM_WORLD, req);
    MPI_Wait(req, MPI_STATUS_IGNORE);
    int count;
    MPI_Probe(serverRank, tagA, MPI_COMM_WORLD, & status);
    mpi_error = MPI_Get_count(&status, MPI_DOUBLE, & count);
    mpi_error = MPI_Recv(mtxA -> data[0], count, MPI_DOUBLE, serverRank, tagA, MPI_COMM_WORLD, status);
    int rows = count / m;
    mtxA -> rows = rows;
    mtxC -> rows = rows;
}

void handleSlaveBody(matrix * mtxA, matrix * mtxB, matrix * mtxC, int serverRank, int tagA, int tagC, int m, int myrank, MPI_Request req[], MPI_Status status) {
    int count, mpi_error;
    mpi_error = MPI_Get_count( &status, MPI_DOUBLE, &count);
    
    // Recive matrix A
    mpi_error = MPI_Irecv(mtxA -> data[0], count, MPI_DOUBLE, serverRank, tagA, MPI_COMM_WORLD, req);
    // Found Error!!
    zeroMatrix(mtxC);
    //printf("SLAVE: %d, count = %d, m = %d count/m\n",myrank,count,m);
    int rows = count / m;
    mtxA -> rows = rows;
    mtxC -> rows = rows;
   // printf("Slave: %d, rows of A: %d \n",myrank, rows);
    
    MPI_Wait(req, MPI_STATUS_IGNORE);

    // Compute Product
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxC, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
    
    // Send Back
    /*
    printf("Slave: %d  rows: %d, cols: %d, MTXC \n",myrank, mtxC  -> rows, mtxC -> cols);
    printMatrix(mtxC);
    */
    
    mpi_error = MPI_Isend(mtxC -> data[0], mtxC -> rows * mtxC -> cols, MPI_DOUBLE, serverRank, tagC, MPI_COMM_WORLD, req);
    MPI_Wait(req, MPI_STATUS_IGNORE);
    
}


void handleSlaveFinish(int * trash, int serverRank, int tagFinilize, int myrank, MPI_Request req []) {
    //printf("SLAVE: %d,  Started Slave Finish\n", myrank);
    int mpi_error;
    mpi_error = MPI_Irecv(trash, 1, MPI_INT, serverRank, tagFinilize, MPI_COMM_WORLD, req);
    MPI_Wait(req, MPI_STATUS_IGNORE);
  //  printf("Rank %d: Finilize recived, Exiting MPI \n", myrank);
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
    //printf("Created Test Matrix\n");
    
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




