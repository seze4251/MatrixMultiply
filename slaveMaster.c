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
    
    MPI_Status status;
    int i;
    int flag = 0, tagA = 1, tagC = 2, tagInit = 3, tagFinilize = -1;
    int place[nprocs], trash[1];
    place[0] = 0;  trash[0] = 100;
   
    
    
    // Get matrix B B
    if (myrank != serverRank) {
        MPI_Wait(req, MPI_STATUS_IGNORE);
    }
    
    // Matrix A, B and C are purposely built large enough that they can hold any size matrix
    // Need to declare new MPI datatype to send the matrix, and 2 ints that describe where it should get put in mtxC
    
    while (1) {
        if (myrank == serverRank) {
            
            // Test End Condition
            if (place[0] == n) {
                int end [nprocs]; // ARRAY END when the array is all 1s we quit this huge loop
                for (i = 1; i < nprocs; i++) {
                    mpi_error = MPI_Isend(trash, 1, MPI_INT, i, tagFinilize, MPI_COMM_WORLD, reqF);
                    mpi_error = MPI_Irecv(trash, 1, MPI_INT, i, tagFinilize, MPI_COMM_WORLD, reqF + i);
                    end[i] = 0; // Initialized to 0
                }
                
                MPI_Waitall(nprocs -1, reqF + 1, MPI_STATUS_IGNORE);
                
                // Ending Loop test to make sure I have recived all messages and everyone sent me there message saying that they are done
                // This loop will be CPU heavy but it is short and at the end
                
                int finished = 0;
                while (finished == 0) {
                    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, & status);
                    
                    if (flag == 1) {
                        
                        if (status.MPI_TAG == tagFinilize ) {
                            mpi_error = MPI_Irecv(trash, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, reqR + status.MPI_SOURCE);
                            MPI_Wait(reqR + status.MPI_SOURCE, MPI_STATUS_IGNORE);
                            end[status.MPI_SOURCE] = 1;
                            
                        } else if(status.MPI_TAG == tagC) {
                            int count;
                            mpi_error = MPI_Get_count(&status, MPI_DOUBLE, & count);
                            
                            mpi_error = MPI_Irecv(mtxC -> data[place[status.MPI_SOURCE]], count, MPI_DOUBLE, status.MPI_SOURCE,
                                                  status.MPI_TAG, MPI_COMM_WORLD, reqR + status.MPI_SOURCE);
                            MPI_Wait(reqR + status.MPI_SOURCE, MPI_STATUS_IGNORE);
                            MPI_Request_free(reqR);
                            
                        }
                    }else {
                        int boolean = 0;
                        for (i = 1; i < nprocs; i++) {
                            if (end[i] == 1) {
                                boolean = 1;
                            }
                        }
                        if (boolean == 0) {
                            finished == 1;
                        }
                    }
                }
                break; // Exit the outmost While Loop
                
            } else {
                
                // Non Blocking Probe
                MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, & status);
                
                if (flag == 1) {
                    
                    if (status.MPI_TAG == tagInit ) {
                        mpi_error = MPI_Irecv(trash, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, reqR + status.MPI_SOURCE);
                        
                        mpi_error = MPI_Isend(mtxA -> data[place[0]], load*m, MPI_DOUBLE, status.MPI_SOURCE, tagA, MPI_COMM_WORLD, \
                                              reqS + status.MPI_SOURCE);
                        
                        place[status.MPI_SOURCE] = place[0];
                        place[0] += load;
                        
                        //Free Request arrive Request
                        MPI_Request_free(reqR + status.MPI_SOURCE);
                        
                    } else if ( status.MPI_TAG == tagC ) {
                        int count;
                        mpi_error = MPI_Get_count(&status, MPI_DOUBLE, &count);
                        
                        mpi_error = MPI_Irecv(mtxC -> data[place[status.MPI_SOURCE]], count, MPI_DOUBLE, status.MPI_SOURCE,
                                              status.MPI_TAG, MPI_COMM_WORLD, reqR + status.MPI_SOURCE);
                        
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
                    int err = matrixProductCacheObliv(mtxA, mtxB, mtxC, place[0], place[0], 0, mtxA->cols, 0, mtxB->cols);
                    place[0] += 1;
                }
            }
        } else {
            // All Other Processes
            if (mtxA -> rows == 2 * load) {
                //Uninitialized
                mpi_error = MPI_Isend(trash, 1, MPI_INT, serverRank, tagInit, MPI_COMM_WORLD, reqF);
                MPI_Request_free(reqF);
            }
            
            MPI_Probe(serverRank, MPI_ANY_TAG, MPI_COMM_WORLD, & status); //Blocking probe to not waste CPU time
            
            if (status.MPI_TAG == tagA) {
                
                int count;
                mpi_error = MPI_Get_count( &status, MPI_DOUBLE, &count);
                
                // Recive matrix A
                mpi_error = MPI_Irecv(mtxA -> data[0], count, MPI_DOUBLE, serverRank, status.MPI_TAG, MPI_COMM_WORLD, reqF);
                
                int rows = count / m;
                mtxA -> rows = rows;
                mtxC -> rows = rows;
                MPI_Wait(reqF, MPI_STATUS_IGNORE);
                
                // Compute Product
                int err = matrixProductCacheObliv(mtxA, mtxB, mtxC, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
                
                // Send Back
                mpi_error = MPI_Isend(mtxC -> data[0], count, MPI_DOUBLE, serverRank, tagC, MPI_COMM_WORLD, reqF);
                MPI_Wait(req, MPI_STATUS_IGNORE);
            } else if (status.MPI_TAG == tagFinilize) {
                mpi_error = MPI_Isend(trash, 1, MPI_INT, serverRank, tagFinilize, MPI_COMM_WORLD, reqF);
                MPI_Wait(reqF, MPI_STATUS_IGNORE);
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



