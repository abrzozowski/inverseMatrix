#include "lu.h"

#include <math.h>

#define DEBUG_VERBOSE_INVERSE 1

SquareMatrix* getL(SquareMatrix *mat)
{
    int dim = mat->dimension;
    SquareMatrix *mat_ret = createIdentityMatrix(dim);
    double **m_src = mat->matrix;
    double **m_dest = mat_ret->matrix;
    int i, j;

    for (i = 1; i < dim; ++i) {
        for (j = 0; j < i; ++j) {
            m_dest[i][j] = m_src[i][j];
        }
    }

    return mat_ret;
}
SquareMatrix* getU(SquareMatrix *mat)
{
    int dim = mat->dimension;
    SquareMatrix *mat_ret = createMatrix(dim);
    double **m_src = mat->matrix;
    double **m_dest = mat_ret->matrix;

    int i, j;

    for (i = 0; i < dim; ++i) {
        for (j = i; j < dim; ++j) {
            m_dest[i][j] = m_src[i][j];
        }
    }

    return mat_ret;
}

SquareMatrix* getLInverse(SquareMatrix *mat_L)
{
    int dim = mat_L->dimension;
    SquareMatrix *mat_ret = createMatrix(dim);
    SquareMatrix *mat_I = createIdentityMatrix(dim);

    double **m_L = mat_L->matrix;
    double **m_I = mat_I->matrix;
    double **m = mat_ret->matrix;

    int i, k, j;
    double sum;

    for(k = 0; k < dim; ++k)
    {
        for(i = k; i < dim; ++i)
        {
            sum = 0.0;
            for(j = k; j < i; ++j) sum +=  m_L[i][j] * m[j][k];
            m[i][k] = (m_I[i][k] - sum) / m_L[i][i];
        }
    }

    freeMatrix(mat_I);

    return mat_ret;
}
SquareMatrix* getUInverse(SquareMatrix *mat_U)
{
    int dim = mat_U->dimension;
    SquareMatrix *mat_ret = createMatrix(dim);
    SquareMatrix *mat_I = createIdentityMatrix(dim);

    double **m_U = mat_U->matrix;
    double **m_I = mat_I->matrix;
    double **m = mat_ret->matrix;

    int i, j, k;
    double sum;

    for(k = 0; k < dim; ++k)
    {
        for(i = k; i >= 0; --i)
        {
            sum = 0.0;
            for(j = i+1; j <= k; ++j) sum +=  m_U[i][j] * m[j][k];
            m[i][k] = (m_I[i][k] - sum) / m_U[i][i];
        }
    }

    freeMatrix(mat_I);

    return mat_ret;
}

int doolittle(SquareMatrix *mat_A, SquareMatrix *mat_LU)
{
    assert(mat_A->dimension == mat_LU->dimension);

    double **matrixA = mat_A->matrix;
    double **matrixLU = mat_LU->matrix;

    int ret = 0;

    int i, j, k;
    double sum;

    int dim = mat_A->dimension;

    for(j = 0; j < dim; ++j)
    {
        for(i = 0; i <= j; ++i)
        {
            sum = 0.0;
            for(k = 0; k < i; ++k) sum +=  matrixLU[i][k] * matrixLU[k][j];
            matrixLU[i][j] = matrixA[i][j] - sum;
        }
        for(i = j + 1; i < dim; ++i)
        {
            sum = 0.0;
            for(k = 0; k < j; ++k) sum += matrixLU[i][k] * matrixLU[k][j];
            matrixLU[i][j] = (matrixA[i][j] - sum) / matrixLU[j][j];
        }
    }

    return ret;
}
int cholesky(SquareMatrix *A, SquareMatrix *mat_LU)
{
    assert(A->dimension == mat_LU->dimension);

    double **matrixA = A->matrix;
    double **matrixLU = mat_LU->matrix;

    int ret = 0;

    int i, j, k;
    double sum;

    int dim = A->dimension;

    for(k = 0; k < dim; ++k){
        sum = 0.0;
        for(j = 0; j < k; ++j) sum += matrixLU[k][j] * matrixLU[k][j];
        matrixLU[k][k] = sqrt(matrixA[k][k] - sum);
        for(i = k+1; i < dim; ++i){
            sum = 0.0;
            for(j = 0; j < k; ++j) sum+= matrixLU[i][j] * matrixLU[k][j];
            matrixLU[i][k] = (matrixA[i][k] - sum) / matrixLU[k][k];
        }
    }

    return ret;
}
int choleskyRow(SquareMatrix *A, SquareMatrix *mat_LU)
{
    assert(A->dimension == mat_LU->dimension);

    double **matrixA = A->matrix;
    double **matrixLU = mat_LU->matrix;

    int ret = 0;

    int i, j, k;
    double sum;

    int dim = A->dimension;

    for(i = 0; i < dim; ++i){
        for(j = 0; j < dim; ++j){
            sum = 0.0;
            for(k = 0; k < j; ++k) sum += matrixLU[i][k] * matrixLU[j][k];
            matrixLU[i][j] = (matrixA[i][j] - sum) / matrixLU[j][j];
        }
        sum = 0.0;
        for(k = 0; k < i; ++k) sum += matrixLU[i][k] * matrixLU[i][k];
        matrixLU[i][i] = sqrt(matrixA[i][i] - sum);
    }

    return ret;
}

SquareMatrix* getInverseMatrixFromLU(SquareMatrix *mat_LU)
{
    int dim = mat_LU->dimension;
    SquareMatrix *mat_ret = createMatrix(dim);
    SquareMatrix *mat_I = createIdentityMatrix(dim);
    double **m_LU = mat_LU->matrix;
    double **m_ret = mat_ret->matrix;
    double **m_I = mat_I->matrix;

    int i, j, k;

    double sum, y[dim];

    for(k = 0; k < dim; k++) {
        for(i = 0; i < dim; i++) {
            sum = 0.0;
            for(j = 0; j <= i-1; j++) {
                sum += m_LU[i][j] * y[j];
            }
            y[i] = (m_I[i][k] - sum);
        }
        for(i = dim-1; i >= 0; i--) {
            sum = 0.0;
            for(j = i+1; j < dim; j++) {
                sum += m_LU[i][j] * m_ret[j][k];
            }
            m_ret[i][k] = (y[i] - sum) / m_LU[i][i];
        }
    }

    freeMatrix(mat_I);

    return mat_ret;
}

SquareMatrix* inverse(SquareMatrix *mat_A)
{
    int dim = mat_A->dimension;
    SquareMatrix *LU = createMatrix(dim);
    SquareMatrix *mat_ret;

    if(isSymmetric(mat_A)) {
        cholesky(mat_A, LU);

        SquareMatrix *L_1 =  getLInverse(LU);
        SquareMatrix *L_1_T =  transpose(L_1);

        mat_ret =  multiply(L_1_T, L_1);

#if DEBUG_VERBOSE_INVERSE
        printMatrixWithName(LU, "LU cholesky");
        printMatrixWithName(L_1, "L^(-1)");
        printMatrixWithName(L_1_T, "(L^(-1))^T");
#endif

        freeMatrix(L_1);
        freeMatrix(L_1_T);
    }
    else {
        doolittle(mat_A, LU);

        SquareMatrix *L = getL(LU);
        SquareMatrix *U = getU(LU);
        SquareMatrix *L_1 = getLInverse(L);
        SquareMatrix *U_1 = getUInverse(U);

        mat_ret = multiply(U_1, L_1);

#if DEBUG_VERBOSE_INVERSE
        printMatrixWithName(LU, "LU doolitle");
        printMatrixWithName(L, "L");
        printMatrixWithName(U, "U");
        printMatrixWithName(L_1, "L^(-1)");
        printMatrixWithName(U_1, "U^(-1)");
#endif

        freeMatrix(L);
        freeMatrix(U);
        freeMatrix(L_1);
        freeMatrix(U_1);
    }

#if DEBUG_VERBOSE_INVERSE
    printMatrixWithName(mat_ret, "A^(-1)");
#endif
    freeMatrix(LU);

    return mat_ret;
}
