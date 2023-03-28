#include "headers/utils.h"

void read_multivector(double* vec, int rows, int cols) {
    int i,j;
    double fl = 0;

    FILE *f = fopen("matrix.txt", "r");
    for(i=0; i<rows; i++){
        for(j=0; j<cols; j++){
            fscanf(f, "%lf", &fl);
            vec[i*cols+j] = fl;
        }
    }
    fclose(f);
}

void save_result(double* y, int rows, int cols) {
    int i, j;

    FILE *f = fopen("result.txt", "w");
    for (i=0; i<rows; i++){
        for (j=0; j<cols; j++) {
            fprintf(f, "%lf ", y[i*cols+j]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

void print_matrix(double* mat, int rows, int cols, char* msg){
    fprintf(stdout, "%s", msg);
    for (int i=0; i < rows; i++) {
        for (int j=0; j < cols; j++) {
            fprintf(stdout, "\t%.16g", mat[i*cols+j]);
        }
        fprintf(stdout, "\n");
    }
}

void print_csr(CSR* csr){
    int m = csr->M, n = csr->N, nz = csr->NZ, *ja = csr->JA, *irp = csr->IRP;
    double *as = csr->AS;

    fprintf(stdout, "CSR:\n");
    fprintf(stdout, "\tM: %d, N: %d, NZ: %d\n", m, n, nz);
    fprintf(stdout, "\tRow pointers: [");

    int i;
    for (i = 0; i < m-1; i++) {
        fprintf(stdout, "%d, ", irp[i]);
    }
    fprintf(stdout, "%d]\n", irp[m-1]);

    fprintf(stdout, "Value (Column):\n");
    for (i = 0; i < nz-1; i++) {
        fprintf(stdout, "\t%.16g (%d)", as[i], ja[i]);
        if (i%3 == 2) fprintf(stdout, "\n");
    }
    fprintf(stdout, "\t%.16g (%d)\n", as[nz-1], ja[nz-1]);
}

void print_ell(ELL* ell) {
    int m = ell->M, n = ell->N, maxnz = ell->MAXNZ, *ja = ell->JA;
    double *as = ell->AS;

    fprintf(stdout, "ELL:\n");
    fprintf(stdout, "\tM: %d, N: %d, MAXNZ: %d\n", m, n, maxnz);
    fprintf(stdout, "Value (Column):\n\t");

    int row, idx;
    for (int i = 0; i < m; i++) {
        row = i * maxnz;
        for (int j = 0; j < maxnz; j++) {
            idx = row + j;
            fprintf(stdout, "%.16g (%d)\t\t", as[idx], ja[idx]);
        }
        fprintf(stdout, "\n\t");
    }
}