#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <math.h>

// 0 is noise, 1 is cluster 0, etc
unsigned char *clusterLUT = NULL;
unsigned short *invocationSetLUT = NULL;
unsigned int clusterCount = 0;
unsigned long *clusterExecutionTimes = NULL;
unsigned long *noiseSetExecutionTimes = NULL;
unsigned int totalInvocationCount = 0;
unsigned int noiseSetCount = 0;
unsigned int invocationSetCount = 0;

void readClusterfile(const char *path) {
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *contents = malloc(fsize);
    size_t readSize = fread(contents, fsize, 1, f);
    fprintf(stderr, "Read size: %ld", readSize);
    fclose(f);

    fprintf(stderr, "\tParsing JSON...\n");
    cJSON *doc = cJSON_ParseWithLength(contents, fsize);

    free(contents);

    cJSON *jInvocationSets = cJSON_GetObjectItem(doc, "invocation_sets");
    assert(cJSON_IsArray(jInvocationSets));

    unsigned long **invocationSets = malloc(cJSON_GetArraySize(jInvocationSets) * sizeof(unsigned long *));

    fprintf(stderr, "\tParsing invocation set information...\n");
    cJSON *set;
    cJSON_ArrayForEach(set, jInvocationSets) {
        totalInvocationCount += cJSON_GetArraySize(set);
    }

    invocationSetLUT = malloc(totalInvocationCount * sizeof(unsigned short));
    invocationSetCount = cJSON_GetArraySize(jInvocationSets);

    unsigned short id = 0;
    cJSON_ArrayForEach(set, jInvocationSets) {
        unsigned int invocationCount = cJSON_GetArraySize(set);
        invocationSets[id] = malloc((invocationCount + 1) * sizeof(unsigned long));
        invocationSets[id][0] = invocationCount;

        unsigned int i = 1;
        cJSON *invok;
        cJSON_ArrayForEach(invok, set) {
            unsigned long invocation = (unsigned long) cJSON_GetNumberValue(invok);
            invocationSets[id][i] = invocation;
            invocationSetLUT[invocation] = id;
            i++;
        }

        id++;
    }

    cJSON *jClusters = cJSON_GetObjectItem(doc, "clusters");
    assert(cJSON_IsArray(jClusters));
    assert(cJSON_GetArraySize(jClusters) < UCHAR_MAX);

    clusterLUT = malloc(totalInvocationCount * sizeof(unsigned char));
    memset(clusterLUT, 0, totalInvocationCount * sizeof(unsigned char));

    fprintf(stderr, "\tBuilding cluster LUT...\n");
    id = 1;
    cJSON *cluster;
    cJSON_ArrayForEach(cluster, jClusters) {
        cJSON *set;
        cJSON_ArrayForEach(set, cluster) {
            unsigned short setID = (int) cJSON_GetNumberValue(set);

            unsigned long *invocations = invocationSets[setID];
            for (unsigned int i = 1; i <= invocations[0]; i++) {
                clusterLUT[invocations[i]] = id;
            }
        }
        id++;
    }

    clusterCount = id;

    cJSON *jNoiseInvocations = cJSON_GetObjectItem(doc, "noise_invocations");
    assert(cJSON_IsArray(jNoiseInvocations));
    noiseSetCount = cJSON_GetArraySize(jNoiseInvocations);

    for (unsigned int i = 0; i < cJSON_GetArraySize(jInvocationSets); i++) {
        free(invocationSets[i]);
    }

    free(invocationSets);
    cJSON_Delete(doc);
}

unsigned long long getExecutionTimes(const char *path) {
    FILE *fp = fopen(path, "r");
    ssize_t read;
    size_t len = 0;
    char *line = NULL;
    read = getline(&line, &len, fp); // Ignore headers

    clusterExecutionTimes = malloc(clusterCount * sizeof(unsigned long));
    memset(clusterExecutionTimes, 0, clusterCount * sizeof(unsigned long));

    if (noiseSetCount > 0) {
        noiseSetExecutionTimes = malloc(invocationSetCount * sizeof(unsigned long));
        memset(noiseSetExecutionTimes, 0, invocationSetCount * sizeof(unsigned long));
    }

    unsigned int id = 0;
    unsigned long long totalTime = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        char *numStart = strchr(line, ',') + 1;
        char *numEnd = strchr(numStart, ',');
        *numEnd  = '\0';
        unsigned long time = atoi(numStart);
        *numEnd = ',';

        unsigned short cluster = clusterLUT[id];
        clusterExecutionTimes[cluster] += time;
        totalTime += time;

        if (__builtin_expect(cluster == 0, 0)) {
            unsigned int set = invocationSetLUT[id];
            noiseSetExecutionTimes[set] += time;
        }

        id++;
    }

    free(line);
    fclose(fp);

    return totalTime;
}

float getInvocationWeight(unsigned int invocation, unsigned long long totalTime) {
    unsigned short cluster = clusterLUT[invocation];
    unsigned long long selfTime;

    if (cluster > 0) {
        selfTime = clusterExecutionTimes[cluster];
    } else {
        unsigned int set = invocationSetLUT[invocation];
        selfTime = noiseSetExecutionTimes[set];
    }

    return selfTime / (double) totalTime;
}

float getFunctionWeight(const char *path, const char *function) {
    FILE *fp = fopen(path, "r");
    ssize_t read;
    size_t len = 0;
    char *line = NULL;
    read = getline(&line, &len, fp); // Ignore headers

    float weight = NAN;
    while ((read = getline(&line, &len, fp)) != -1) {
        strtok(line, "\t");
        char *name = strtok(NULL, "\t");

        if (strcmp(name, function) == 0) {
            strtok(NULL, "\t");
            strtok(NULL, "\t");
            char *percentage = strtok(NULL, "\t");
            *strchr(percentage, '%') = '\0';
            weight = atof(percentage) / 100.0;
            break;
        }
    }

    free(line);
    fclose(fp);

    return weight;
}

float getWeight(unsigned int invocation, const char *samplingPath, const char *function, unsigned long long totalTime) {
    return getInvocationWeight(invocation, totalTime) * getFunctionWeight(samplingPath, function);
}

int main(int argc, char **argv) {
    if (argc < 6) {
        fprintf(stderr,
                "Usage:\n"
                "\tmicroweight <cluster_info> <performance_trace> <sampling_results> <function_name> <invocation_index>...\n"
                "\n"
                "\tcluster_info: JSON file generated via the cti_cluster script from the microbenchmark's original function\n"
                "\tperformance_trace: Performance trace csv file generated via perf-invok of the microbenchmark's original function\n"
                "\tsampling_results: Results of the sampling performed via ChopStiX. This corresponds to the output of 'chop list functions'\n"
                "\tfunction_name: Name of the microbenchmark's original function\n"
                "\tinvoication_index: Space-separated list of invocation indices from which to calculate the weight\n"
                "\n"
                "This program calculates the representativeness of an invocation to a function within the execution time of the whole benchmark.\n"
        );
        return -1;
    }

    const char *clusterPath = argv[1];
    const char *profilePath = argv[2];
    const char *samplingPath = argv[3];
    const char *function = argv[4];

    fprintf(stderr, "Reading clustering information...\n");
    readClusterfile(clusterPath);

    fprintf(stderr, "Cluster count: %d\nNoise invocation set count: %d\n", clusterCount - 1, noiseSetCount);

    fprintf(stderr, "Reading profiling information...\n");
    unsigned long long totalTime = getExecutionTimes(profilePath);

    fprintf(stderr, "Cluster execution times:\n");
    for (unsigned int i = 0; i < clusterCount; i++) {
        fprintf(stderr, "Cluster %d: %lu\n", i, clusterExecutionTimes[i]);
    }

    for (unsigned int i = 5; i < argc; i++) {
        const unsigned int invocation = atoi(argv[i]);

        printf("%f\n", getWeight(invocation, samplingPath, function, totalTime));
    }

    return 0;
}
