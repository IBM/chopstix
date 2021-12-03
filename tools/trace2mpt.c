//#define _GNU_SOURCE
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <errno.h>

#define PATH_LEN 512

const char MPT_HEADER_TEMPLATE[] =
"[MPT]\n\
mpt_version = 0.5\n\
\n\
[STATE]\n\
contents = %s%s\n\
\n\
[CODE]\n\
default_address = 0x%016lx\n\
instructions=\n\
";

typedef enum {
    SEGMENT_CODE, SEGMENT_DATA
} MemorySegmentType;

typedef struct {
    unsigned long start;
    unsigned long end;
    MemorySegmentType type;
} MemorySegment;

MemorySegment *findSegment(unsigned long addr, MemorySegment segments[],
                           unsigned int segment_count) {
    unsigned int mid;
    unsigned int start = 0;
    unsigned int end = segment_count - 1;

    while (start <= end) {
        mid = (start + end) / 2;
        MemorySegment *segment = &segments[mid];

        if (addr >= segment->start && addr < segment->end) {
            return segment;
        } else if (addr >= segment->end) {
            start = mid + 1;
        } else if (addr < segment->start) {
            end = mid - 1;
        }
    }

    return NULL;
}

void parseMemorySegments(char *data, size_t data_size,
                         MemorySegment **segments_out,
                         unsigned int *segment_count_out) {
    unsigned int segment_count = 1; // Take into account last entry
    for (unsigned int i = 0; i < data_size; i++) {
        if (data[i] == '\n') segment_count++;
    }
    MemorySegment *segments = malloc(sizeof(MemorySegment) * segment_count);

    char *line = strtok(data, "\n");
    unsigned int index = 0;
    while (line) {
        char perms[5];
        sscanf(line, "%lx-%lx %s", &segments[index].start,
               &segments[index].end, perms);
        segments[index].type = perms[2] == 'x' ? SEGMENT_CODE : SEGMENT_DATA;
        index++;
        line = strtok(NULL, "\n");
    }

    *segments_out = segments;
    *segment_count_out = segment_count;
}

#define READ_BUFFER_BS 8192

void readCompressedFile(const char *path, char **data, size_t *data_size) {
    gzFile fp = gzopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "chop-trace2mpt: Error: Unable to open %s\n", path);
        exit(EXIT_FAILURE);
    }
    size_t buffer_size = READ_BUFFER_BS;
    *data = malloc(buffer_size);
    size_t offset = 0;
    unsigned int unzipped_bytes;
    while ((unzipped_bytes = gzread(fp, *data + offset, READ_BUFFER_BS)) > 0) {
        offset += unzipped_bytes;
        if (unzipped_bytes == READ_BUFFER_BS) {
            buffer_size += READ_BUFFER_BS;
            *data = realloc(*data, buffer_size);
        }
    }
    gzclose(fp);
    *data_size = offset;
}

void readRegularFile(const char *path, char **data, size_t *data_size) {
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "chop-trace2mpt: Error: Unable to open %s\n", path);
        fprintf(stderr, "chop-trace2mpt: Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0L, SEEK_END);
    *data_size = ftell(fp);
    rewind(fp);
    *data = malloc(*data_size);
    size_t w = fread(*data, *data_size, 1, fp);
    if ((*data_size * w) != *data_size) {
        fprintf(stderr, "chop-trace2mpt: Error: Unable to read all contents of %s\n", path);
        fprintf(stderr, "chop-trace2mpt: Error: Expecting: %ld bytes, %ld readed\n", (*data_size * w),  *data_size);
        exit(EXIT_FAILURE);
    }
    fclose(fp);
}

void readFile(char *path, char **data, size_t *data_size, bool text) {
    const char *extension = strrchr(path, '.');
    bool compressed = false;

    if (strcmp(extension, ".gz") == 0) {
        compressed = true;
    } else if (access(path, R_OK) != 0) {
        strcat(path, ".gz");
        if (access(path, R_OK) == 0) {
            compressed = true;
        } else {
            fprintf(stderr, "chop-trace2mpt: Can't find file %s", path);
            abort();
        }
    }

    if (compressed) readCompressedFile(path, data, data_size);
    else readRegularFile(path, data, data_size);

    if (text) { // Add null terminator
        *data = realloc(*data, *data_size + 1);
        (*data)[*data_size] = '\0';
    }
}

void format_data(FILE *mps, const char *data, size_t data_size,
                 unsigned long address) {
    fprintf(mps, "M %016lx ", address);
    for (unsigned int i = 0; i < data_size; i++) fprintf(mps, "%02x", data[i]);
    fwrite("\n", 1, 1, mps);
}

void format_code(FILE *mpt, const char *data, size_t data_size,
                 unsigned long address) {
    if ((data_size % 16) != 0) {
        fprintf(stderr, "chop-trace2mpt: Data size not multiple of 16 bytes\n");
        exit(EXIT_FAILURE);
    }

    uint32_t *words = (uint32_t *) data;
    unsigned int word_groups = data_size / 16;
    for (unsigned int i = 0; i < word_groups; i++) {
        fprintf(mpt, "  0x%lx:\n", address + (i * 16));
        for (unsigned int j = 0; j < 4; j++) {
            fprintf(mpt, "    0x%08x\n", words[i*4 + j]);
        }
    }
}

void trace2mpt(const char *output_base, const char *trace_dir,
               unsigned int index, unsigned long long int max_address,
               bool compressed) {
    char path[PATH_LEN];
    char mpt_path[PATH_LEN];
    char mps_path[PATH_LEN];
    char *data;
    size_t data_size;

    // Read segment data
    sprintf(path, "%s/maps.%d", trace_dir, index);
    readFile(path, &data, &data_size, true);
    MemorySegment *segments;
    unsigned int segment_count;
    parseMemorySegments(data, data_size, &segments, &segment_count);
    free(data);

    printf("chop-trace2mpt: Read %d segments.\n", segment_count);

    sprintf(mpt_path, "%s#%d.mpt", output_base, index);
    FILE *mpt = fopen(mpt_path, "w");

    if (mpt == NULL) {
        fprintf(stderr, "chop-trace2mpt: Unable to write to '%s'\n", mpt_path);
        fprintf(stderr, "chop-trace2mpt: Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Read default address
    unsigned long default_address;
    sprintf(path, "%s/info.%d", trace_dir, index);
    readFile(path, &data, &data_size, true);
    sscanf(data, "%*s %lx", &default_address);
    free(data);

    printf("chop-trace2mpt: Default code address: %lx\n", default_address);

    sprintf(mps_path, "%s#%d.mps", output_base, index);
    FILE *mps = fopen(mps_path, "w");

    if (mps == NULL) {
        fprintf(stderr, "chop-trace2mpt: Unable to write to '%s'\n", mpt_path);
        fprintf(stderr, "chop-trace2mpt: Error: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    char *lastSlash = strrchr(mps_path, '/');

    char *mps_name = (lastSlash != NULL ? lastSlash + 1 : mps_path);
    fprintf(mpt, MPT_HEADER_TEMPLATE, mps_name, compressed ? ".gz" : "", default_address);

    // Write register contents
    sprintf(path, "%s/regs.%d", trace_dir, index);
    readFile(path, &data, &data_size, true);
    char *line = strtok(data, "\n");
    unsigned int register_count = 0;
    while (line) {
        fprintf(mps, "R %s\n", line);
        line = strtok(NULL, "\n");
        register_count++;
    }
    free(data);
    fprintf(mps, "R PC 0x%lx\n", default_address);
    printf("chop-trace2mpt: Read %d registers.\n", register_count);

    // Find memory pages
    unsigned int _num_alloced_pages = 64;
    char **pages = malloc(_num_alloced_pages * sizeof(char *));
    unsigned int num_pages = 0;
    DIR *dir;
    dir = opendir(trace_dir);
    if (dir == NULL) {
        fprintf(stderr, "chop-trace2mpt: Error: Unable to open %s\n", trace_dir);
        fprintf(stderr, "chop-trace2mpt: Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    char filter[16];
    sprintf(filter, "page.%d.", index);
    unsigned int filter_length = strlen(filter);
    struct dirent *entry;
    // TODO: Use scandir to get sorted/filteres results
    while ((entry = readdir(dir)) != NULL) {
        unsigned int name_length = strlen(entry->d_name);
        if (strncmp(filter, entry->d_name, filter_length) == 0) {
            if (num_pages == _num_alloced_pages) {
                _num_alloced_pages *= 2;
                pages = realloc(pages, _num_alloced_pages * sizeof(char *));
            }

            char *name = malloc(name_length + 1);
            memcpy(name, entry->d_name, name_length + 1);
            pages[num_pages++] = name;
        }
    }
    closedir(dir);

    // Process memory pages
    bool default_address_found = false;
    for (unsigned int i = 0; i < num_pages; i++) {
        printf("\rchop-trace2mpt: Processing page %d/%d", i + 1, num_pages);
        fflush(stdout);
        unsigned long address = strtol(pages[i] + filter_length, NULL, 16);

        if (max_address != 0 && address >= max_address) {
            if (default_address < address && default_address >= (address + data_size)) {
                printf("\rchop-trace2mpt: Processing page %d/%d skip\n", i + 1, num_pages);
                fflush(stdout);
                continue;
            }
        }

        sprintf(path, "%s/%s", trace_dir, pages[i]);
        readFile(path, &data, &data_size, false);
        MemorySegment *segment =
            findSegment(address, segments, segment_count);
        if (segment != NULL && segment->type == SEGMENT_CODE) {
            format_code(mpt, data, data_size, address);
            if (default_address >= address && default_address < (address + data_size)) {
                printf("%ld, %lx\n", data_size, address);
                default_address_found = true;
            }
        } else {
            format_data(mps, data, data_size, address);
        }
        free(data);
    }

    fclose(mpt);
    fclose(mps);

    if (!default_address_found) {
        fprintf(stderr, "\nchop-trace2mpt: Error: Default address 0x%lx not in code.\n", default_address);
        fprintf(stderr, "chop-trace2mpt: Error: Check tracing execution.\n");
        if (remove(mpt_path) != 0) {
            fprintf(stderr, "chop-trace2mpt: Error: Unable to remove %s\n", mpt_path);
            fprintf(stderr, "chop-trace2mpt: Error: %s\n", strerror(errno));
        }
        if (remove(mps_path) != 0) {
            fprintf(stderr, "chop-trace2mpt: Error: Unable to remove %s\n", mps_path);
            fprintf(stderr, "chop-trace2mpt: Error: %s\n", strerror(errno));
        }
        return;
    }

    if (compressed) {
        char cmd[PATH_LEN+12];
        snprintf(cmd, PATH_LEN+12, "gzip -f -9 %s", mpt_path);
        if (system(cmd) != 0 ) printf("\nchop-trace2mpt: Unable to compress");
        snprintf(cmd, PATH_LEN+12, "gzip -f -9 %s", mps_path);
        if (system(cmd) != 0 ) printf("\nchop-trace2mpt: Unable to compress");
    }

    printf("\nchop-trace2mpt: mpt: %s%s", mpt_path, compressed ? ".gz" : "");
    printf("\nchop-trace2mpt: mps: %s%s", mps_path, compressed ? ".gz" : "");
    printf("\nchop-trace2mpt: Trace %u done.\n", index);

}

void print_usage() {
    printf(
        "Usage: chop-trace2mpt [-i <id>] -o <out> [-h] [--trace-dir <dir>] [--gzip]\n"
        "Options:\n"
        "  -h,--help            Display this help and exit\n"
        "  -i,--id <id>         Trace id. If not provided all id in the directory will be processed.\n"
        "  -o,--output <out>    Output base path. i.e. (<out>.<id>.mpt <out>.<id>.mps will be generated)\n"
        "  --trace-dir <dir>    Path to trace directory (default: ./trace_data)\n"
        "  --max-address value  Pages above this address will not be dumped to the generated MPT\n"
        "  --gzip               Zip trace output files\n");
}

int main(int argc, const char **argv) {

    // TODO: Use getopt libc interface for argument parsing

    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        print_usage();
        return 0;
    }

    unsigned int id = 0;
    bool id_provided = false;
    const char *output_base = NULL;
    const char *trace_dir = "./trace_data";
    bool compressed = false;
    unsigned long long int max_address = 0;

    enum {
        EXPECTING_OPT, EXPECTING_ID, EXPECTING_OUTPUT_BASE, EXPECTING_TRACE_DIR,
        EXPECTING_ADDRESS
    } state = EXPECTING_OPT;

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            print_usage();
            return 0;
        }

        switch (state) {
            case EXPECTING_OPT:
                if (strcmp(arg, "-i") == 0) state = EXPECTING_ID;
                else if (strcmp(arg, "-o") == 0) state = EXPECTING_OUTPUT_BASE;
                else if (strcmp(arg, "--trace-dir") == 0) state = EXPECTING_TRACE_DIR;
                else if (strcmp(arg, "--max-address") == 0) state = EXPECTING_ADDRESS;
                else if (strcmp(arg, "--gzip") == 0) compressed = true;
                else {
                    fprintf(stderr, "chop-trace2mpt: Unknown option: %s\n", arg);
                    print_usage();
                    return 2;
                }
                break;
            case EXPECTING_ID:
                id = atoi(arg);
                id_provided = true;
                state = EXPECTING_OPT;
                break;
            case EXPECTING_ADDRESS:
                max_address = strtoll(arg, NULL, 0);
                state = EXPECTING_OPT;
                break;
            case EXPECTING_OUTPUT_BASE:
                output_base = arg;
                state = EXPECTING_OPT;
                break;
            case EXPECTING_TRACE_DIR:
                trace_dir = arg;
                state = EXPECTING_OPT;
                break;
        }
    }

    if (output_base == NULL) {
        fprintf(stderr, "chop-trace2mpt: Missing required option(s): -o\n");
        print_usage();
        return 2;
    }

    if (id_provided) {
        printf("chop-trace2mpt: Processing trace id: %d\n", id);
        trace2mpt(output_base, trace_dir, id, max_address, compressed);
    } else {
        struct dirent **namelist;
        int n;

        n = scandir(trace_dir, &namelist, NULL, alphasort);
        if (n == -1) {
            fprintf(stderr, "chop-trace2mpt: Unable to open dir '%s'\n", trace_dir);
            fprintf(stderr, "chop-trace2mpt: Error: %s\n",strerror(errno));
            exit(EXIT_FAILURE);
        }

        int count = 0;
        while (count < n) {
            if (strncmp("info.", namelist[count]->d_name, 5) == 0) {
               unsigned int index = strtoul(namelist[count]->d_name + 5, NULL, 10);
               printf("chop-trace2mpt: Processing trace id: %d\n", index);
               trace2mpt(output_base, trace_dir, index, max_address, compressed);
            }
            free(namelist[count]);
            ++count;
        }
        free(namelist);
        printf("chop-trace2mpt: Total traces processed: %d\n", count);
    }
    exit(EXIT_SUCCESS);
}
