//
//  matrixDoublePointer.h
//  MatrixDoublePointer
//
//  Created by Seth on 1/24/16.
//  Copyright © 2016 Seth. All rights reserved.
//

#ifndef matrixDoublePointer_h
#define matrixDoublePointer_h

#include <stdio.h>
void matrixInit();
double ** newMatrix(int rows, int cols);
void randomizeMatrix(int size, double ** mtx);
void deleteMatrix(double ** mtx);
void printMatrix(double ** mtx, int rows, int cols);
double ** matrixProduct(double ** mtxA, double ** mtxB, double ** mtxC, int length);
double ** matrixProduct2(double ** mtxA, double ** mtxB, double ** mtxC, int length);

#endif /* matrixDoublePointer_h */
