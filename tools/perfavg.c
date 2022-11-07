#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#define CALC_AVG(name, n) name ## Mean = ((n - 1) * name ## Mean + name) / (double) n
#define CALC_VAR(name, n) name ## Variance = ((n - 2) * name ## Variance + (n / (double) (n + 1) * (name ## Mean - name) * (name ## Mean - name))) / (double) (n - 1)
#define CALC_GEO(name, n) name ## Geo = ((n - 1) * name ## Geo + log(name)) / (double) n

void average(unsigned int count, FILE **files, FILE *means, FILE *deviations) {
    ssize_t read;
    size_t len = 0;
    char *line = NULL;

    // Skip headers
    for (unsigned int i = 0; i < count; i++) assert(getline(&line, &len, files[i]) > 0);

    while (true) {
        bool finished = false;

        // Variables used for calculating the arithmetic mean, required by the variance
        double cyclesMean = 0;
        double timeMean = 0;
        double instrMean = 0;
        double memInstrMean = 0;
        double missesMean = 0;
        double ipcMean = 0;
        double missrateMean = 0;

        // Variables used for calculating the geometric mean, required by the output
        double cyclesGeo = 0;
        double timeGeo = 0;
        double instrGeo = 0;
        double memInstrGeo = 0;
        double missesGeo = 0;
        double ipcGeo = 0;
        double missrateGeo = 0;

        // Variables used for calculating the std deviation, required by the output
        double cyclesVariance = 0;
        double timeVariance = 0;
        double instrVariance = 0;
        double memInstrVariance = 0;
        double missesVariance = 0;
        double ipcVariance = 0;
        double missrateVariance = 0;

        for (unsigned int i = 0; i < count; i++) {
            unsigned long cycles, time, instr, memInstr, misses;
            float ipc, missPercent;
            read = fscanf(files[i], "%lu, %lu, %lu, %lu, %lu, %f, %f\n", &cycles, &time, &instr, &memInstr, &misses, &ipc, &missPercent);

            if (read == 7) {
                unsigned int n = i + 1;

                double ipc = instr / cycles;
                double missrate = misses / memInstr;

                CALC_AVG(cycles, n);
                CALC_AVG(time, n);
                CALC_AVG(instr, n);
                CALC_AVG(memInstr, n);
                CALC_AVG(misses, n);
                CALC_AVG(memInstr, n);
                CALC_AVG(ipc, n);
                CALC_AVG(missrate, n);

                CALC_GEO(cycles, n);
                CALC_GEO(time, n);
                CALC_GEO(instr, n);
                CALC_GEO(memInstr, n);
                CALC_GEO(misses, n);
                CALC_GEO(ipc, n);
                CALC_GEO(missrate, n);

                if (n > 1) {
                    CALC_VAR(cycles, n);
                    CALC_VAR(time, n);
                    CALC_VAR(instr, n);
                    CALC_VAR(memInstr, n);
                    CALC_VAR(misses, n);
                    CALC_GEO(ipc, n);
                    CALC_GEO(missrate, n);
                }
            } else {
                assert(i == 0);
                finished = true;
                break;
            }

        }

        if (finished) {
            for (unsigned int i = 0; i < count; i++) {
                assert(getline(&line, &len, files[i]) == -1);
            }

            break;
        } else {
            fprintf(means, "%f, %f, %f, %f, %f, %f, %f\n", exp(cyclesGeo), exp(timeGeo), exp(instrGeo), exp(memInstrGeo), exp(missesGeo), exp(ipcGeo), exp(missrateGeo));

            fprintf(deviations, "%f, %f, %f, %f, %f, %f, %f\n", sqrt(cyclesVariance), sqrt(timeVariance), sqrt(instrVariance), sqrt(memInstrVariance), sqrt(missesVariance), sqrt(ipcVariance), sqrt(missrateVariance));
        }
    }

}

int main(int argc, char **argv) {
    assert(argc > 5);

    unsigned int count = argc - 3;
    FILE *means = fopen(argv[1], "w");
    FILE *deviations = fopen(argv[2], "w");
    FILE **fp = malloc(count * sizeof(FILE *));

    fprintf(means, "Cycles, Time Elapsed (us), Retired Instructions, Retired Memory Instructions, Data Cache Misses, Instructions Per Cycle, Miss Percentage\n");
    fprintf(deviations, "Cycles, Time Elapsed (us), Retired Instructions, Retired Memory Instructions, Data Cache Misses, Instructions Per Cycle, Miss Percentage\n");

    for (unsigned int i = 0; i < count; i++) fp[i] = fopen(argv[3 + i], "r");

    average(count, fp, means, deviations);

    return 0;
}
