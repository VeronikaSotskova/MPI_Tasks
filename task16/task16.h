#ifndef TASK1_TASK16_H
#define TASK1_TASK16_H

#include "../main.h"

void TestResult(double *pAMatrix, double *pBMatrix, double *pCMatrix, int size);

void CreateGridCommunicators();

void RandomDataInitialization(double *pAMatrix, double *pBMatrix, int Size);

void ProcessInitialization(double *&pAMatrix, double *&pBMatrix,
                           double *&pCMatrix, double *&pAblock, double *&pBblock, double *&pCblock,
                           double *&pTemporaryAblock, int Size, int &BlockSize);

void CheckerboardMatrixScatter(double *pMatrix, double *pMatrixBlock, int Size, int BlockSize);

void
DataDistribution(double *pAMatrix, double *pBMatrix, double *pMatrixAblock, double *pBblock, int Size, int BlockSize);

void ResultCollection(double *pCMatrix, double *pCblock, int Size, int BlockSize);

void ABlockCommunication(int iter, double *pAblock, double *pMatrixAblock, int BlockSize);

void BlockMultiplication(double *pAblock, double *pBblock,
                         double *pCblock, int BlockSize);

void BblockCommunication(double *pBblock, int BlockSize);

void ParallelResultCalculation(double *pAblock, double *pMatrixAblock,
                               double *pBblock, double *pCblock, int BlockSize);

void task_16(int size_of_arr, int argc, char **argv);


#endif //TASK1_TASK16_H
