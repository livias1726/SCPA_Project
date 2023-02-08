#include <unistd.h>
#include "utils/utils.h"

#define NUM_RUNS 5
#define NUM_STORAGE 2
#define NUM_K 7
#define PROGRAM "cmake-build-debug/openmp/omp_spmv " //"cmake-build-debug/serial/spmv "
#define PATH "resources\\files\\"

#define get_gflops(t, k, nz) ((2*k*nz)/t)

void tokenize_output (char* output, double* start, double* end, int* nz) {
    char* tok = strtok(output, " ");
    char* tokens[3];
    int count = -1;

    while (tok != NULL && count < 3) {
        count++;
        tokens[count] = tok;
        tok = strtok(NULL, " ");
    }

    if (count == 2) {
        *start = strtod(tokens[0], NULL);
        *end = strtod(tokens[1], NULL);
        *nz = (int)strtol(tokens[2], NULL, 10);
    }
}

double elapsed_nanoseconds (double start, double end) {
    double secs, nanos = modf(start, &secs);
    double sece, nanoe = modf(end, &sece);

    double sec = sece - secs;
    double nano = nanoe - nanos;

    if (sec != 0) {
        nano += (sec* pow(10,9));
    }

    return nano;
}

int main(){

#ifndef PERFORMANCE
    fprintf(stderr, "Compilation needs PERFORMANCE flag");
    exit(-1);
#endif

    FILE *mat_file, *pipe, *out_file;
    int i, j, z, k, nz;
    double start, end, time, gflops = 0;
    char name[NAME_MAX], input[IO_MAX], output[IO_MAX], filepath[strlen(PATH) + NAME_MAX];
    char *ks[NUM_K] = {" 3", " 4", " 8", " 12", " 16", " 32", " 64"},
         *storage[NUM_STORAGE] = {" csr", " ellpack"};
    unsigned ptr1 = strlen(PROGRAM), ptr2, ptr3;

    // get list of matrix names
    if ((mat_file = fopen("matrices.txt", "r")) == NULL) {
        fprintf(stderr, "Cannot open matrices file (Error: %d)\n", errno);
        exit(-1);
    }
    strcpy(filepath, PATH); // initialize first half of filepath
    strcpy(input, PROGRAM); // add program name to input

    //create csv for results
    if ((out_file = fopen("gflops.csv", "w+")) == NULL) {
        fprintf(stderr, "Cannot open output file\n");
        exit(-1);
    }
    fprintf(out_file, "Matrix, Storage Format, K, GFLOPS\n"); //header

    // run
    while (fgets(name, NAME_MAX, mat_file)) {
        // add matrix name to input
        name[strlen(name)-1] = '\0';
        if (strcmp(name, "") == 0) {
            continue;
        }
        strcpy(&input[ptr1], name);
        // build path
        strcpy(&filepath[strlen(PATH)], name);
        ptr2 = ptr1 + strlen(name); // index of the copying point

        for (i=0; i<NUM_STORAGE; i++) {
            // add storage format to input
            strcpy(&input[ptr2], storage[i]);
            ptr3 = ptr2 + strlen(storage[i]); // index of the copying point

            for (j=0; j<NUM_K; j++) {
                // add k value to input
                strcpy(&input[ptr3], ks[j]);

                for (z=0; z<NUM_RUNS; z++) {
                    printf("Execution %d: [%s]\n", z, input);
                    // launch program
                    pipe = popen(input, "r");
                    if (pipe == NULL){
                        fprintf(stderr, "Cannot open pipe\n");
                        exit(-1);
                    }

                    while (fgets(output, PATH_MAX, pipe)){}

                    // close program
                    if (pclose(pipe) == -1) {
                        fprintf(stderr, "Cannot close pipe\n");
                        exit(-1);
                    }

                    tokenize_output(output, &start, &end, &nz);
                    time = elapsed_nanoseconds(start, end);
                    k = (int)strtol(ks[j], NULL, 10);
                    gflops += get_gflops(time, k, nz);
                }

                // retrieve average gflops
                gflops = gflops / NUM_RUNS;

                // save on csv
                fprintf(out_file, "%s,%s,%d,%f\n", name, storage[i], k, gflops);

                sleep(1);
            }

            gflops = 0;
        }
    }

    fclose(mat_file);
    fclose(out_file);

    return 0;
}
