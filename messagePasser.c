//
//  messagePasser.c
//  messagePasser
//
//  Created by Seth on 2/27/16.
//  Copyright © 2016 Seth. All rights reserved.
//

#include "messagePasser.h"
#include <stdio.h>
#include <stdlib.h>

MPI_Request * immediateSend(const void * buf, int count, MPI_Datatype datatype, int dest, int tag,
                            MPI_Comm comm, MPI_Request *request){
    
    
    // Send
    int mpi_err = MPI_Isend(buf, count, datatype, dest, tag, comm, request);
    
    if (mpi_err == MPI_ERR_COMM) {
        printf("MPI_ERROR: Invalid Comunicator: tag: %d  dest: %d count: %d \n", tag, dest, count);
        return NULL;
        
    } else if(mpi_err == MPI_ERR_COUNT) {
        printf("MPI_ERROR: Invalid Count: tag: %d  dest: %d count %d \n", tag, dest, count);
        return NULL;
        
    } else if(mpi_err == MPI_ERR_TYPE) {
        printf("MPI_ERROR: Invalid Type: tag: %d  dest: %d count %d \n", tag, dest, count);
        return NULL;
        
    } else if(mpi_err == MPI_ERR_TAG ) {
        printf("MPI_ERROR: Invalid Tag: tag: %d  dest: %d count %d \n", tag, dest, count);
        return NULL;
        
    } else if(mpi_err == MPI_ERR_RANK ) {
        printf("MPI_ERROR: Invalid Rank: (Source or Dest) tag: %d  dest: %d count %d \n", tag, dest, count);
        return NULL;
        
    } else if(mpi_err == MPI_ERR_INTERN ) {
        printf("MPI_ERROR: Invalid Intern: MPI unable to get mem tag: %d  dest: %d count %d \n", tag, dest, count);
        return NULL;
    }
    
    return request;
}

MPI_Request * immediateRecev(const void * buf, int count, MPI_Datatype datatype, int dest, int tag,
                            MPI_Comm comm, MPI_Request *request){
    
    
    // Send
    int mpi_err = MPI_Isend(buf, count, datatype, dest, tag, comm, request);
    
    if (mpi_err == MPI_ERR_COMM) {
        printf("MPI_ERROR: Invalid Comunicator: tag: %d  dest: %d count: %d \n", tag, dest, count);
        return NULL;
        
    } else if(mpi_err == MPI_ERR_COUNT) {
        printf("MPI_ERROR: Invalid Count: tag: %d  dest: %d count %d \n", tag, dest, count);
        return NULL;
        
    } else if(mpi_err == MPI_ERR_TYPE) {
        printf("MPI_ERROR: Invalid Type: tag: %d  dest: %d count %d \n", tag, dest, count);
        return NULL;
        
    } else if(mpi_err == MPI_ERR_TAG ) {
        printf("MPI_ERROR: Invalid Tag: tag: %d  dest: %d count %d \n", tag, dest, count);
        return NULL;
        
    } else if(mpi_err == MPI_ERR_RANK ) {
        printf("MPI_ERROR: Invalid Rank: (Source or Dest) tag: %d  dest: %d count %d \n", tag, dest, count);
        return NULL;
        
    } else if(mpi_err == MPI_ERR_INTERN ) {
        printf("MPI_ERROR: Invalid Intern: MPI unable to get mem tag: %d  dest: %d count %d \n", tag, dest, count);
        return NULL;
    }
    
    return request;
}



int addChecksum( ){
    
}

int removeChecksum()



