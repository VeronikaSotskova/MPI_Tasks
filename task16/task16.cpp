#include "task16.h"

int ProcNum = 0; // ���-�� ����ᮢ
int ProcRank = 0; // ࠭�
int GridSize; // ࠧ��� ��⪨
int GridCoords[2]; // ���न���� ������ � ��⪥
MPI_Comm GridComm; // ����㭨���� �⪨
MPI_Comm ColComm; // ����㭨���� �⮫�殢
MPI_Comm RowComm; // ����㭨���� ��ப

void task_16(int size_of_arr, int argc, char **argv) {
    double *pAMatrix; // ��ࢠ� �����
    double *pBMatrix; // ���� �����
    double *pCMatrix;// १����
    int Size = size_of_arr; // ࠧ��� ������
    int BlockSize; // ࠧ��� ����筮�� ����� �� ������
    double *pAblock; // ��砫�� ���� ������ �
    double *pBblock; // ��砫�� ���� ������ �
    double *pCblock; // १������騩 ����
    double *pMatrixAblock; //�ᯮ������ �� ���뫪�� ������ �� ��ப�� ��⪨ ����ᮢ

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    GridSize = sqrt((double) ProcNum);
    if (ProcNum != GridSize * GridSize) {
        if (ProcRank == 0) {
            printf("Number of processes must be a perfect square \n");
        }
    } else {
        if (ProcRank == 0)
            printf("Parallel matrix multiplication program\n");
// ᮧ����� �����୮� �⪨
        CreateGridCommunicators();
// �뤥����� ����� � ���樠������ �����
        ProcessInitialization(pAMatrix, pBMatrix, pCMatrix, pAblock, pBblock,
                              pCblock, pMatrixAblock, size_of_arr, BlockSize);
// ��।������ ��室��� ������
        DataDistribution(pAMatrix, pBMatrix, pMatrixAblock, pBblock, Size,
                         BlockSize);
// ��⮤ ����
        ParallelResultCalculation(pAblock, pMatrixAblock, pBblock,

                                  pCblock, BlockSize);
// ᡮ� १����
        ResultCollection(pCMatrix, pCblock, Size, BlockSize);
// �뢮� १����
        TestResult(pAMatrix, pBMatrix, pCMatrix, Size);

    }
    MPI_Finalize();
}

// �㭪�� ᮧ���� ����㭨���� � ���� ��㬥୮� �����⭮� ��⪨, ��।���� ���न���� ������� ����� � �⮩ ��⪥
void CreateGridCommunicators() {
    int DimSize[2]; // �᫮ �����஢ � ������ ����७�� �⪨
    int Periodic[2]; // =1, �᫨ ࠧ��୮��� ������ ���� ������筠
    int Subdims[2]; // =1, �᫨ ࠧ��୮��� ������ ���� 䨪�஢����
    DimSize[0] = GridSize;
    DimSize[1] = GridSize;
    Periodic[0] = 0;
    Periodic[1] = 0;
/*2 - �᫮ ����७��
DimSize - -	 ���ᨢ, � ���஬ �������� �᫮ ����ᮢ ����� ������� ����७��
Periodic - �����᪨� ���ᨢ ࠧ��� ndims ��� ������� �࠭���� �᫮���
1 - �����᪠� ��६�����, 㪠�뢠��, �ந������� ��७㬥��� ����ᮢ (true) ��� ��� (false)
GridComm - ���� ����㭨����*/
    MPI_Cart_create(MPI_COMM_WORLD, 2, DimSize, Periodic, 1, &GridComm);
//��।������ ���न��� ������� ������ � ��⪥
    MPI_Cart_coords(GridComm, ProcRank, 2, GridCoords);
// ᮧ����� ����㭨���� ��� ��ப
    Subdims[0] = 0; // 䨪��� ࠧ��୮��
    Subdims[1] = 1; // ����稥 ������ ࠧ��୮�� � ����⪥
    MPI_Cart_sub(GridComm, Subdims, &RowComm);
// ᮧ����� ����㭨���� ��� �⮫�殢
    Subdims[0] = 1;
    Subdims[1] = 0;
    MPI_Cart_sub(GridComm, Subdims, &ColComm);
}

// ���������� ��室��� ���ᨢ�� ࠭����묨 ���祭�ﬨ
void RandomDataInitialization(double *pAMatrix, double *pBMatrix, int Size) {
    for (int i = 0; i < Size; i++)
        for (int j = 0; j < Size; j++) {
            pAMatrix[i * Size + j] = rand() % 10;
            pBMatrix[i * Size + j] = rand() % 10;
        }
}

// �뤥����� ����� � ���樠������ �����
void ProcessInitialization(double *&pAMatrix, double *&pBMatrix,
                           double *&pCMatrix, double *&pAblock, double *&pBblock, double *&pCblock,
                           double *&pTemporaryAblock, int Size, int &BlockSize) {

    MPI_Bcast(&Size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    BlockSize = Size / GridSize;
    pAblock = new double[BlockSize * BlockSize];
    pBblock = new double[BlockSize * BlockSize];
    pCblock = new double[BlockSize * BlockSize];
    //�ᯮ������ �� ���뫪�� ������ �� ��ப�� ��⪨ ����ᮢ
    pTemporaryAblock = new double[BlockSize * BlockSize];
    for (int i = 0; i < BlockSize * BlockSize; i++) {
        pCblock[i] = 0;
    }
    if (ProcRank == 0) {
        pAMatrix = new double[Size * Size];
        pBMatrix = new double[Size * Size];
        pCMatrix = new double[Size * Size];
        RandomDataInitialization(pAMatrix, pBMatrix, Size);
    }
}

// �㭪�� ࠧ������� ������ �� ���
void CheckerboardMatrixScatter(double *pMatrix, double *pMatrixBlock, int Size, int BlockSize) {
    double *pMatrixRow = new double[BlockSize * Size];
/*����� ������ ��ਧ��⠫�묨 ����ᠬ� ����� �����ࠬ� �㫥���� �⮫�� ��⪨*/
    if (GridCoords[1] == 0) {
/*pMatrix - ���� ��砫� ࠧ��饭�� ������ ��।��塞�� ������
BlockSize*Size - �᫮ ����⮢, ���뫠���� ������� ������
pMatrixRow - ���� �ਥ��
BlockSize*Size - �᫮ ����砥��� ����⮢
0 - ����� ������ ��ࠢ�⥫�
colComm - ����㭨����*/
        MPI_Scatter(pMatrix, BlockSize * Size, MPI_DOUBLE, pMatrixRow, BlockSize * Size, MPI_DOUBLE, 0, ColComm);
    }
/*��।������ ������ ��ப� ��ਧ��⠫쭮� ������ ������ ����� ��ப �����୮� ��⪨*/
    for (int i = 0; i < BlockSize; i++) {
        MPI_Scatter(&pMatrixRow[i * Size], BlockSize, MPI_DOUBLE, &(pMatrixBlock[i * BlockSize]), BlockSize, MPI_DOUBLE,
                    0, RowComm);
    }
    delete[] pMatrixRow;
}

// �㭪�� ��।������ ������ ����� �����ࠬ�
void
DataDistribution(double *pAMatrix, double *pBMatrix, double *pMatrixAblock, double *pBblock, int Size, int BlockSize) {
    CheckerboardMatrixScatter(pAMatrix, pMatrixAblock, Size, BlockSize);
    CheckerboardMatrixScatter(pBMatrix, pBblock, Size, BlockSize);
}

// ᮡ�ࠥ� ����� � १����
void ResultCollection(double *pCMatrix, double *pCblock, int Size, int BlockSize) {
    double *pResultRow = new double[Size * BlockSize];
    for (int i = 0; i < BlockSize; i++) {
/*�㭪�� MPI_Gather �ந������ ᡮ�� ������ ������, ���뫠���� �ᥬ� ����ᠬ� ��㯯�, � ���� ���ᨢ ����� � ����஬ root
&pCblock[i*BlockSize] - ���� ��砫� ࠧ��饭�� ���뫠���� ������
BlockSize - �᫮ ���뫠���� ����⮢
&pResultRow[i*Size] - ���� ��砫� ���� �ਥ��
BlockSize - �᫮ ����⮢, ����砥��� �� ������� �����
0 - ����� ������ �����⥤��*/
        MPI_Gather(&pCblock[i * BlockSize], BlockSize, MPI_DOUBLE, &pResultRow[i * Size], BlockSize, MPI_DOUBLE, 0,
                   RowComm);
    }
    if (GridCoords[1] == 0) {
/*�㭪�� MPI_Gather �ந������ ᡮ�� ������ ������, ���뫠���� �ᥬ� ����ᠬ� ��㯯�, � ���� ���ᨢ ����� � ����஬ root*/
        MPI_Gather(pResultRow, BlockSize * Size, MPI_DOUBLE, pCMatrix, BlockSize * Size, MPI_DOUBLE, 0, ColComm);
    }
    delete[] pResultRow;
}

/*� ��砫� ������ ���樨 iter �����⬠ ��� ������ ��ப� ����᭮� ��⪨ �롨ࠥ��� �����,
 ����� �㤥� ���뫠�� ᢮� ���� ������ �*/
void ABlockCommunication(int iter, double *pAblock, double *pMatrixAblock,

                         int BlockSize) {
// ��।��塞 ����騩 ������
    int Pivot = (GridCoords[0] + iter) % GridSize;
// �����㥬 ��।������ ���� � ����
    if (GridCoords[1] == Pivot) {
        for (int i = 0; i < BlockSize * BlockSize; i++)
            pAblock[i] = pMatrixAblock[i];
    }
// �᪨�뢠�� ���� ����� ��ப�
    MPI_Bcast(pAblock, BlockSize * BlockSize, MPI_DOUBLE, Pivot, RowComm);
}

// ��������� ������� ������
void BlockMultiplication(double *pAblock, double *pBblock,
                         double *pCblock, int BlockSize) {
// ���᫥��� �ந�������� ������� ������
    for (int i = 0; i < BlockSize; i++) {
        for (int j = 0; j < BlockSize; j++) {
            double temp = 0;
            for (int k = 0; k < BlockSize; k++)
                temp += pAblock[i * BlockSize + k] * pBblock[k * BlockSize + j];
            pCblock[i * BlockSize + j] += temp;
        }
    }
}

// 横���᪨� ᤢ�� ����� �⮫��
void BblockCommunication(double *pBblock, int BlockSize) {
    MPI_Status Status;
    int NextProc = GridCoords[0] + 1;
    if (GridCoords[0] == GridSize - 1) NextProc = 0;
    int PrevProc = GridCoords[0] - 1;
    if (GridCoords[0] == 0) PrevProc = GridSize - 1;
    /*����� ����묨 ������ ⨯� � ����饭��� ���뫠���� ������ �� �ਭ������
    pBblock - ���� ��砫� �ᯮ������� ���뫠����� � �ਭ�������� ᮮ�饭�
    BlockSize*BlockSize - �᫮ ��।������� ����⮢
    NextProc - ����� �����-�����⥫�
    0 - �����䨪��� ���뫠����� ᮮ�饭��
    PrevProc - ����� ������ ��ࠢ�⥫�
    0 - �����䨪��� �ਭ�������� ᮮ�饭��
    */
    MPI_Sendrecv_replace(pBblock, BlockSize * BlockSize, MPI_DOUBLE,
                         NextProc, 0, PrevProc, 0, ColComm, &Status);
}

void ParallelResultCalculation(double *pAblock, double *pMatrixAblock,
                               double *pBblock, double *pCblock, int BlockSize) {
    for (int iter = 0; iter < GridSize; iter++) {
// ���뫪� ����� ������ � �� ��ப� �����୮� ��⪨
        ABlockCommunication(iter, pAblock, pMatrixAblock, BlockSize);
// 㬭������
        BlockMultiplication(pAblock, pBblock, pCblock, BlockSize);
// 横���᪨� ᤢ�� ������ ������ � ����� �⮫�� �����୮� ��⪨
        BblockCommunication(pBblock, BlockSize);
    }
}

// �뢮� १����
void TestResult(double *pAMatrix, double *pBMatrix, double *pCMatrix, int size) {
    if (ProcRank == 0) {
        printf("AMatrix\n");
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++)
                printf("%10.0f ", pAMatrix[i * size + j]);
            printf("\n");
        }
        printf("\nBMatrix\n");
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++)
                printf("%10.0f ", pBMatrix[i * size + j]);
            printf("\n");
        }
        printf("\nCMatrix\n");
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++)
                printf("%10.0f ", pCMatrix[i * size + j]);
            printf("\n");
        }
    }
}