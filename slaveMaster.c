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
            int check = scanf("%d %d %d",&n, &m, &p, &load);
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
    
    matrix * mtxA = newMatrix(n, m);
    matrix * mtxB = newMatrix(m, p);
    matrix * mtxC = newMatrix(n, p);
    
    if (myrank == serverRank) {
        randomizeMatrix(mtxA);
        randomizeMatrix(mtxB);
        starttime = MPI_Wtime();
    }

    
    // Broadcast B
    MPI_Request req [nprocs];
    mpi_error = MPI_Ibcast(mtxB -> data[0], m*p, MPI_DOUBLE, serverRank, MPI_COMM_WORLD, req);
    
    // Define Stuff for main loop
    // WildCards: MPI_ANY_TAG,  MPI_ANY_SOURCE, MPI_STATUS_IGNORE
    MPI_Status status;
    int flag = 0, tagA = 1, tagC = 2, tagInit = 3;
    
    // Get matrix B B
    if (myrank != serverRank) {
        MPI_Wait(req, MPI_STATUS_IGNORE);
    }
    
    // Matrix A, B and C are purposely built large enough that they can hold any size matrix
    // Need to declare new MPI datatype to send the matrix, and 2 ints that describe where it should get put in mtxC
    
    while (1) {
        if (myrank == serverRank) {
            // Probe message and put contents into status
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, & status)
            
            if (flag == 1) {
                int count;
                mpi_error = MPI_Get_count( &status, MPI_DOUBLE, count )
                
                if (status.MPI_TAG == tagInit ) {
                    // Initialize - For initializer tag, do not read message, just send data
                    mpi_error = MPI_Isend(mtxA -> data[ Proper location], load*m, MPI_Datatype, status.MPI_ANY_SOURCE, tagA, MPI_COMM_WORLD, \
                                          req + status.MPI_SOURCE);
                  
                } else if ( status.MPI_TAG == tagC ) {
                    // Normal Procedure
                    // Get the Message
                    mpi_error = MPI_Irecv(mtxC -> data[0], count, MPI_Datatype datatype, status.MPI_SOURCE,
                                          status.MPI_TAG, MPI_COMM_WORLD, req + status.MPI_SOURCE);
                    // Send the new Message
                    mpi_error = MPI_Isend(mtxA -> data[ Proper location], load*m, MPI_Datatype, status.MPI_ANY_SOURCE, tagA, MPI_COMM_WORLD, \
                                          req + status.MPI_SOURCE);
                    // Do something here :)
                    
                    MPI_Wait(req + status.MPI_SOURCE, MPI_STATUS_IGNORE);
                    // Put buffer into appropriate place in matrixC
                    
                } else {
                    // Some sort of Error LOG IT
                    
                }
                flag = 0;
            } else {
                // Compute 1 row of A and check back for more messages
            }
            
            // potentially test request objects to make sure all communicaiton went properly

            } else {
                // All Other Processes
                
                if (mtxA -> rows == n) {
                    //If mtxA rows equal A no information has been sent in
                    // Send an initialize message
                }
                
                MPI_Iprobe(serverRank, tagA, MPI_COMM_WORLD, &flag, & status)
                
                if (flag == 1) {
                    int count;
                    mpi_error = MPI_Get_count( &status, MPI_DOUBLE, count )
                    // Recive matrix A
                    
                    // Compute Product
                    
                    // Send Back
                    
                } else {
                    // Do something here
                }
                
            
            
        }
    }
    
    // This stuff is just a reference while I write the new program
    /*
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
			MPI_Request_free(req+myrank);            
        } else {
            MPI_Isend(mtxC -> data[0], scatterSize * p, MPI_DOUBLE, serverRank, tagC, MPI_COMM_WORLD, req + myrank);
            MPI_Request_free(req+myrank);
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
        /*
        matrix * mtxTest = newMatrix(n, p);
        printf("Created Test Matrix\n");
        
        matrixProductCacheObliv(mtxA, mtxB, mtxTest, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
        printf("Completed Test Multiplication \n");
        // Test Correctness  DEBUG
	    MPI_Waitall(nprocs - 1, req + 1, MPI_STATUS_IGNORE);

/*        printf("Matrix Test: \n");
        printMatrix(mtxTest);
        printf("Matrix C: \n");
        printMatrix(mtxC);
  */      
        //MPI_Waitall(nprocs - 1, req + 1, MPI_STATUS_IGNORE);
     /*   printf("Finshed Waitall in Server Process\n");
        if (subtractMatrix(mtxC, mtxTest)) {
            printf("\n Matrix Product Cache Obliv incorrect \n");
        } else {
            printf(" \n Matrix Product Correct!!!!!!!! \n");
        }
        deleteMatrix(mtxTest);
        printf("Deleted Test Matrix Server\n");
        
    }
*/
	// This Barrier gets my code working, without it I have a seg fault
	MPI_Barrier(MPI_COMM_WORLD);
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



