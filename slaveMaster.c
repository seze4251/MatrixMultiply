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
    int buff[4], n, m, p, load;
    
    if (myrank == serverRank) {
        while (1) {
            printf("Please enter 3 ints to dimensionlize the Matrix followed by a int for load distribution (4 total) \n");
            int check = scanf("%d %d %d %d",&n, &m, &p, &load);
            if (n > 0 && m > 0 && p > 0) {
                break;
            } else {
                printf("Please Enter Three Numbers that Are greater than 0 \n");
            }
        }
        buff[0] = n; buff[1] = m; buff[2] = p; load = buff[4];
    }
    mpi_error = MPI_Bcast(buff, 4, MPI_INT, serverRank, MPI_COMM_WORLD); // MPI Bcast blocks until Everyone is here
    n = buff[0]; m = buff[1]; p = buff[2]; load = buff[4];
    matrix * mtxA, * mtxC;
    matrix * mtxB = newMatrix(m, p);
    
    if (myrank == serverRank) {
        mtxA = newMatrix(n, m);
        mtxC = newMatrix(n, p);
        randomizeMatrix(mtxA);
        randomizeMatrix(mtxB);
        starttime = MPI_Wtime();
    } else {
        mtxA = newMatrix(2 * load, m);
        mtxC = newMatrix(2 * load, p);
    }
    
    
    // Broadcast B
    MPI_Request reqS [nprocs];
    MPI_Request reqR [nprocs];
    MPI_Request reqF [nprocs];
    
    
    
    mpi_error = MPI_Ibcast(mtxB -> data[0], m*p, MPI_DOUBLE, serverRank, MPI_COMM_WORLD, reqF);
    
    // Define Stuff for main loop
    // WildCards: MPI_ANY_TAG,  MPI_ANY_SOURCE, MPI_STATUS_IGNORE
    
    // Status Variable
    MPI_Status status;
    // Loop Counter
    int i = 0;
    // Flag for Non blocking probe and Positive message tags
    int flag = 0, tagA = 1, tagC = 2, tagInit = 3, tagFinilize = 53;
    // Location holder Place for Master and Trash array to send quick messages between processes based on tag
    int place[nprocs], trash[1];
    trash[0] = 100;
    // Counter for Messages sent
    
    //
    if (myrank == serverRank) {
        for (i = 0; i < nprocs; i++) {
            place[i] = 0;
        }
    }
    
    
    
    
    // Get matrix B B
    if (myrank != serverRank) {
        MPI_Wait(reqF, MPI_STATUS_IGNORE);
    }
    
    // Matrix A, B and C are purposely built large enough that they can hold any size matrix
    // Need to declare new MPI datatype to send the matrix, and 2 ints that describe where it should get put in mtxC
    
    while (1) {
        if (myrank == serverRank) {
            
            // Test End Condition
            if (place[0] == n) {
                printf("SERVER: Starting Ending Loop");
                // When Terminate is not equal to 0 exit
                int terminate = 0;
                int hasData = 0;
                
                for (i = 1; i < nprocs; i++) {
                    if (place[i] != 0) {
                        hasdata += 1;
                    }
                }
                
                // Case 1: Master did all the work (small matrix)
                if (!hasData) {
                    break;
                }
                
                // Case 2 and 3: Some or all of the processes still have data
                // Find out which Processes have data
                // IHasData will be an array containing the ranks of slaves with data
                int IHasData [hasData];
                for (i = 1; i < nprocs; i++) {
                    if (place[i] != 0) {
                        IHasData[i] += i;
                    }
                }
                
                i = 0;
                while (i < hasData ) {
                    MPI_Probe(MPI_ANY_SOURCE, tagC, MPI_COMM_WORLD, & status);
                    int count;
                    mpi_error = MPI_Get_count(&status, MPI_DOUBLE, &count);
                    
                    mpi_error = MPI_Irecv(mtxC -> data[place[status.MPI_SOURCE]], count, MPI_DOUBLE, status.MPI_SOURCE,
                                          tagC, MPI_COMM_WORLD, reqR);
                    //Wait for Data to go in
                    MPI_Wait(reqR, MPI_STATUS_IGNORE);
                    
                }
                
                // Send termination message to all processes
                
                i = 0;
                while (i < hasData ) {
                    mpi_error = MPI_Isend(trash, 1, MPI_INT, IHasData[i], tagFinilize, MPI_COMM_WORLD, reqS + i);
                }
                
                // Wait for all messages to go through to avoid seg fault
                MPI_Waitall(hasData, reqS, MPI_STATUS_IGNORE);
                
            } else {
                
                // Non Blocking Probe
                MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, & status);
                flag = 1;
                printf("SERVER: Flag Value =  %d \n", flag);
                //   printf("Server: Recv Message from rank %d", status.MPI_SOURCE);
                if (flag == 1) {
                    
                    if (status.MPI_TAG == tagInit ) {
                        printf("SERVER: Initilize Message from Process %d \n",status.MPI_SOURCE);
                        mpi_error = MPI_Irecv(trash, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, reqR + status.MPI_SOURCE);
                        
                        mpi_error = MPI_Isend(mtxA -> data[place[0]], load*m, MPI_DOUBLE, status.MPI_SOURCE, tagA, MPI_COMM_WORLD, \
                                              reqS + status.MPI_SOURCE);
                        
                        place[status.MPI_SOURCE] = place[0];
                        place[0] += load;
                        
                        //Free Request arrive Request
                        MPI_Request_free(reqR + status.MPI_SOURCE);
                        printf("Server: Made it past Free\n");
                        
                    } else if ( status.MPI_TAG == tagC ) {
                        printf("SERVER: Data message from Process %d \n", status.MPI_SOURCE);
                        int count;
                        mpi_error = MPI_Get_count(&status, MPI_DOUBLE, &count);
                        
                        mpi_error = MPI_Irecv(mtxC -> data[place[status.MPI_SOURCE]], count, MPI_DOUBLE, status.MPI_SOURCE,
                                              status.MPI_TAG, MPI_COMM_WORLD, reqR + status.MPI_SOURCE);
                        
                        if (place[0] + load >= n) {
                            load = n - place[0];
                        }
                        
                        
                        // Send the new Message
                        mpi_error = MPI_Isend(mtxA -> data[place[0]], load*m, MPI_DOUBLE, status.MPI_SOURCE, tagA, MPI_COMM_WORLD, \
                                              reqS + status.MPI_SOURCE);
                        
                        place[status.MPI_SOURCE] = place[0];
                        place[0] += load;
                        
                        MPI_Wait(reqR + status.MPI_SOURCE, MPI_STATUS_IGNORE);
                        MPI_Request_free(reqR);
                        
                    } else {
                        // Some sort of Error LOG IT
                        printf("Error: Log not implemented yet, Message Tag inncorrect \n");
                        
                    }
                    
                } else if( flag == 0) {
                    // Compute 1 row of A and check back for more messages
                    printf("SERVER: Compute: row = %d\n",place[0]);
                    int err = matrixProductCacheObliv(mtxA, mtxB, mtxC, place[0], place[0], 0, mtxA->cols, 0, mtxB->cols);
                    place[0] += 1;
                }
            }
        } else {
            // All Other Processes
            if (i == 0) {
                //initialize
                i = 1;
                printf("Rank %d: Send Initialize\n",myrank);
                mpi_error = MPI_Isend(trash, 1, MPI_INT, serverRank, tagInit, MPI_COMM_WORLD, reqF);
                MPI_Wait(reqF, MPI_STATUS_IGNORE);
                printf("Rank %d: Finish Initialize \n", myrank);
            }
            
            MPI_Probe(serverRank, MPI_ANY_TAG, MPI_COMM_WORLD, &status); //Blocking probe to not waste CPU time
            printf("Rank %d: Finsihed Probe\n", myrank);
            
            if (status.MPI_TAG == tagA) {
                printf("Rank %d: Recive Data\n",myrank);
                
                int count;
                mpi_error = MPI_Get_count( &status, MPI_DOUBLE, &count);
                
                // Recive matrix A
                mpi_error = MPI_Irecv(mtxA -> data[0], count, MPI_DOUBLE, serverRank, tagA, MPI_COMM_WORLD, reqF);
                
                int rows = count / m;
                mtxA -> rows = rows;
                mtxC -> rows = rows;
                
                MPI_Wait(reqF, MPI_STATUS_IGNORE);
                printf("Rank %d: Print Matrix A\n",myrank);
                printMatrix(mtxA);
                // Compute Product
                int err = matrixProductCacheObliv(mtxA, mtxB, mtxC, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
                
                // Send Back
                mpi_error = MPI_Isend(mtxC -> data[0], count, MPI_DOUBLE, serverRank, tagC, MPI_COMM_WORLD, reqF);
                MPI_Wait(reqF, MPI_STATUS_IGNORE);
            } else if (status.MPI_TAG == tagFinilize) {
                mpi_error = MPI_Irecv(trash, 1, MPI_INT, serverRank, tagFinilize, MPI_COMM_WORLD, reqF);
                MPI_Wait(reqF, MPI_STATUS_IGNORE);
                printf("Rank %d: Finilize recived, Exiting MPI \n", myrank);
            }
        }
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
        // Test Correctness  DEBUG
        /*printf("Matrix Test: \n");
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



