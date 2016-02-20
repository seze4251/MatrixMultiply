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
    
    // Server Rank Sends out A
    if (myrank == serverRank) {
        // length - how many rows to send
        length = scatterSize;
        
        for (i = 1; i < nprocs; i++) {
            if (i == nprocs - 1) {
                length = scatterSize + rem;
            }
            mpi_error = MPI_Isend(mtxA -> data[ scatterSize + scatterSize * (i -1)], length*m, MPI_DOUBLE, i, tagA, \
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
    
//    MPI_Waitall(nprocs, req, MPI_STATUS_IGNORE);
 /* 		MPI_Wait(req, MPI_STATUS_IGNORE);
  		printf("made it1 \n");

         MPI_Wait(req +1, MPI_STATUS_IGNORE);
        printf("made it2 \n");

         MPI_Wait(req+2, MPI_STATUS_IGNORE);
         printf("made it3 \n");
 
          MPI_Wait(req +3, MPI_STATUS_IGNORE);
        printf("made it4 \n");
*/
		if ( myrank != serverRank) {
			MPI_Wait(req, MPI_STATUS_IGNORE);
			MPI_Wait(req + myrank, MPI_STATUS_IGNORE);
		}

	printf("MADE IT HERE ON PROCESS: %d\n", myrank);			
    // Perform Matrix Multplicaiton
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxC, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
    printf("Process Number:%d  Error Code: %d\n", myrank, err); //Debug
    
    if (myrank == serverRank) {

	for ( i = 1; i < nprocs; i++) {
	printf("I work on server i = %d",i);
		MPI_Request_free(req+i);
	}
        // Resize rows of A and C
        mtxA -> rows = n;
        mtxC -> rows = n;
        length = scatterSize;
        
        for (i = 1; i < nprocs; i++) {
            if (i == nprocs -1) {
                length = scatterSize + rem;
            }
            mpi_error =  MPI_Irecv(mtxC -> data [scatterSize + scatterSize * (i-1)], length * p, MPI_DOUBLE, i, i+tagC, \
			MPI_COMM_WORLD, req + i);
        }
        
    } else {
		//MPI_Request_free(req + myrank);
		//printf("All otheres work process %d \n", myrank);
		if (myrank == nprocs -1) {
            MPI_Isend(mtxC -> data[0], (scatterSize + rem) * p, MPI_DOUBLE, serverRank, myrank+tagC, MPI_COMM_WORLD, req+myrank);
            
        } else {
            MPI_Isend(mtxC -> data[0], scatterSize * p, MPI_DOUBLE, serverRank, myrank+tagC, MPI_COMM_WORLD, req + myrank);
            
        }
        printf("MADE IT TO SETHS POINT ON ALL OTHER PROCESSES\n");
    }
    
    
 // MPI_Waitall(nprocs, req, MPI_STATUS_IGNORE);
  /*         MPI_Wait(req, MPI_STATUS_IGNORE);
           printf("made it1 \n");
   
            MPI_Wait(req +1, MPI_STATUS_IGNORE);
          printf("made it2 \n");
   
            MPI_Wait(req+2, MPI_STATUS_IGNORE);
            printf("made it3 \n");
       
             MPI_Wait(req +3, MPI_STATUS_IGNORE);
           printf("made it4 \n");
*/

    if (myrank == serverRank) {
        
        // Print to File
        /*FILE * file = fopen("OutputParallel","a");
         fprintf(file, " %d \t\t %lu /n",m,total_t);
         fclose(file);
         */
        
        matrix * mtxTest = newMatrix(n, p);
      //  matrixProductCacheObliv(mtxA, mtxB, mtxTest, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
        
        // Test Correctness  DEBUG
        /*
         printf("Matrix Test: \n");
         printMatrix(mtxTest);
         printf("Matrix C: \n");
         printMatrix(mtxC);
         */
		printf("Howdy!!\n");
//  		MPI_Waitall(nprocs - 1, req + 1, MPI_STATUS_IGNORE);      
        printf("howdy from proc 0 2nd time \n");
	/*	if (subtractMatrix(mtxC, mtxTest)) {
            printf("\n Matrix Product Cache Obliv incorrect \n");
        } else {
			printf(" \n Matrix Product Correct!!!!!!!! \n");
		}
    */    deleteMatrix(mtxTest);
    }
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
    MPI_Finalize();
    return 0;
}



