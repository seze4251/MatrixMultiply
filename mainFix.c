//
//  mainFix.c
//  MatrixFix
//
//  Created by Seth on 2/2/16.
//  Copyright © 2016 Seth. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrixFix.h"
#include <stdbool.h>

void testCacheObliv(int n, int m, int p, bool output, FILE * file);
void testFix3(int n, int m, int p, bool output, FILE * file);
void testFix2(int n, int m, int p, bool output, FILE * file);
void testFix1(int n, int m, int p, bool output, FILE * file);
void testBaseFun(int n, int m, int p, bool output, FILE * file);
void testFUNCTIONS( int n, int m, int p, bool output, FILE * file);


int main(int argc, const char * argv[]) {
    // Initialize
    matrixInit();
    int n, m, p;
    bool output;
    
    // Test Input
    if (argc != 2) {
        printf("Must take input of integer for matrix size\n");
		return -1;
    }
    int test = atoi(argv[1]);
    printf("n = %d\n",test);
    // Set up Output File
    FILE * fileA = fopen("outputTestBase.txt", "a");
    FILE * fileB = fopen("outputTest1.txt", "a");
    FILE * fileC = fopen("outputTest2.txt", "a");
    FILE * fileD = fopen("outputTest3.txt", "a");
    FILE * fileE = fopen("outputTestCache.txt", "a");
    // Speed Test of square Matrix
    
    output = false;
   	n = m = p = test;
    
    testBaseFun(n, m, p, output, fileA);
    testFix1(n, m, p, output, fileB);
       testFix2(n, m, p, output, fileC);
       testFix3(n, m, p, output, fileD);
    testCacheObliv(n, m, p, output, fileE);
    
    fclose(fileA);
    fclose(fileB);
    fclose(fileC);
    fclose(fileD);
    fclose(fileE);
    
    /*
     testFUNCTIONS(n, m, p, output, File file, int test);
     testBaseFun(n, m, p, output, File file, int test);
     testFix1(n, m, p, output, File file, int test);
     testFix2(n, m, p, output, File file, int test);
     testFix3(n, m, p, output, File file, int test);
     testCacheObliv(n, m, p, output, File file, int test);
     testCacheOblivNonBlocking(n, m, p, output, File file, int test);
     testCacheOblivBlocking(n, m, p, output, File file, int test);
     testCacheOblivOpenMP(n, m, p, output, File file, int test);
     */
    
    return 0;
}

void testFUNCTIONS( int n, int m, int p, bool output, FILE * file) {
    matrix * mtxA, * mtxB, * mtxC, * mtxTest;
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    mtxC = newMatrix(n, p);
    mtxTest = newMatrix(n, p);
    
    if (!mtxA || !mtxB || !mtxC) {
        printf("One of the matrixes is Null");
    }
    
    
    if (output) {
        //Print Matrixes - expect 0s
        printf("Expected all zeros, size %d by %d", n, m);
        printMatrix(mtxA);
        printf("\n\n");
        
        //Const matrix and print it
        const int value = 5;
        
        constMatrix(mtxA, value);
        constMatrix(mtxB, value -3);
        
        // Print Matrix to visually inspect
        printf("Matrix A:\n");
        printMatrix(mtxA);
        printf("\n\n");
        
        printf("Matrix B:\n");
        printMatrix(mtxB);
        printf("\n\n");
    }
    
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
    deleteMatrix(mtxTest);
    
}

void testBaseFun(int n, int m, int p, bool output, FILE * file) {
    clock_t start_t, end_t, total_t;
    matrix * mtxA, * mtxB, * mtxC;
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    mtxC = newMatrix(n, p);
    randomizeMatrix(mtxA);
    randomizeMatrix(mtxB);
    
    start_t = clock();
    
    int err = matrixProduct(mtxA, mtxB, mtxC);
    
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    fprintf(file, "%d \t\t %lu\n\n",m, total_t);
    
    if ( output ) {
        printf("Error code: %d\n",err);
        printf("Matrix A:\n");
        printMatrix(mtxA);
        printf("\n\n");
        printf("Matrix B:\n");
        printMatrix(mtxB);
        printf("\n\n");
        printf("mtx C \n");
        printMatrix(mtxC);
    }
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxC);
}

void testFix1(int n, int m, int p, bool output, FILE * file) {
    clock_t start_t, end_t, total_t;
    matrix * mtxA, * mtxB, * mtxC, * mtxTest;
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    
    mtxTest = newMatrix(n, p);
    randomizeMatrix(mtxA);
    randomizeMatrix(mtxB);
    
    start_t = clock();
    
    int err = matrixProductFix1(mtxA, mtxB, mtxTest);
    
    end_t = clock();
    
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    fprintf(file, "%d \t\t %lu\n\n",m, total_t);
    
    if ( output ) {
        printf("Error code: %d\n",err);
        mtxC = newMatrix(n, p);
        matrixProduct(mtxA, mtxB, mtxC);
        if(subtractMatrix(mtxC, mtxTest)) {
            printf("\n Matrix Product Fix 1 incorrect \n");
        }
        printf("Matrix A:\n");
        printMatrix(mtxA);
        printf("\n\n");
        printf("Matrix B:\n");
        printMatrix(mtxB);
        printf("\n\n");
        printf("mtx C \n");
        printMatrix(mtxC);
        printf("mtxTest\n");
        printMatrix(mtxTest);
        deleteMatrix(mtxC);
    }
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxTest);
}



void testFix2(int n, int m, int p, bool output, FILE * file) {
    clock_t start_t, end_t, total_t;
    matrix * mtxA, * mtxB, * mtxC, * mtxTest;
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    
    randomizeMatrix(mtxA);
    randomizeMatrix(mtxB);
    mtxTest = newMatrix(n, p);
    
    
    
    start_t = clock();
    
    int err = matrixProductFix2(mtxA, mtxB, mtxTest);
    
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    fprintf(file, "%d \t\t %lu\n\n",m, total_t);
    
    if ( output ) {
        printf("Error code: %d\n",err);
        mtxC = newMatrix(n, p);
        matrixProduct(mtxA, mtxB, mtxC);
        
        if(subtractMatrix(mtxC, mtxTest)) {
            printf("\n Matrix Product Fix 2 incorrect \n");
        }
        
        printf("Matrix A:\n");
        printMatrix(mtxA);
        printf("\n\n");
        printf("Matrix B:\n");
        printMatrix(mtxB);
        printf("\n\n");
        printf("mtx C \n");
        printMatrix(mtxC);
        printf("mtxTest\n");
        printMatrix(mtxTest);
        deleteMatrix(mtxC);
    }
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxTest);
}

void testFix3(int n, int m, int p, bool output, FILE * file) {
    clock_t start_t, end_t, total_t;
    matrix * mtxA, * mtxB, * mtxC, * mtxTest;
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    
    mtxTest = newMatrix(n, p);
    randomizeMatrix(mtxA);
    randomizeMatrix(mtxB);
    
    
    
    
    start_t = clock();
    
    int err = matrixProductFix3(mtxA, mtxB, mtxTest);
    
    
    
    end_t = clock();
    
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    fprintf(file, "%d \t\t %lu\n\n",m, total_t);
    
    if ( output ) {
        printf("Error code: %d\n",err);
        mtxC = newMatrix(n, p);
        matrixProduct(mtxA, mtxB, mtxC);
        if(subtractMatrix(mtxC, mtxTest)) {
            printf("\n Matrix Product Fix 3 incorrect \n");
        }
        
        printf("Matrix A:\n");
        printMatrix(mtxA);
        printf("\n\n");
        printf("Matrix B:\n");
        printMatrix(mtxB);
        printf("\n\n");
        printf("mtx C \n");
        printMatrix(mtxC);
        printf("mtxTest\n");
        printMatrix(mtxTest);
        deleteMatrix(mtxC);
    }
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxTest);
}

void testCacheObliv(int n, int m, int p, bool output, FILE * file) {
    // Declare Clock
    clock_t start_t, end_t, total_t;
    // Declare and Init Matrixies
    matrix * mtxA, * mtxB, * mtxC, * mtxTest;
    mtxA = newMatrix(n, m);
    mtxB = newMatrix(m, p);
    mtxTest = newMatrix(n, p);
    randomizeMatrix(mtxA);
    randomizeMatrix(mtxB);
    
    
    // Perform Operation
    start_t = clock();
    int err = matrixProductCacheObliv(mtxA, mtxB, mtxTest, 0, mtxA->rows, 0, mtxA->cols, 0, mtxB->cols);
    end_t = clock();
    
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    fprintf(file, "%d \t\t %lu\n\n",m, total_t);
    
    if ( output ) {
        printf("Error code: %d\n",err);
        mtxC = newMatrix(n, p);
        matrixProduct(mtxA, mtxB, mtxC);
        // Test Correctness
        if(subtractMatrix(mtxC, mtxTest)) {
            printf("\n Matrix Product Cache Obliv incorrect \n");
        }
        
        printf("Matrix A:\n");
        printMatrix(mtxA);
        printf("\n\n");
        printf("Matrix B:\n");
        printMatrix(mtxB);
        printf("\n\n");
        printf("mtx C \n");
        printMatrix(mtxC);
        printf("mtxTest\n");
        printMatrix(mtxTest);
        deleteMatrix(mtxC);
    }
    
    deleteMatrix(mtxA);
    deleteMatrix(mtxB);
    deleteMatrix(mtxTest);
}

