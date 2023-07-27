#include "color.h"
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define ICAT_VERSION "v0.1.0"

void usage(int status);
void streamcopy(FILE *fin, FILE *fout);
char *readline(FILE *istream);

struct icat {
    bool show_numbers;
    bool show_tabs;
};

struct icat icat = {false, false};

int main(int argc, char *argv[]) {
    char *file;

    static struct option const long_options[] = {
        {"number", no_argument, nullptr, 'n'},
        {"show-tabs", no_argument, nullptr, 'T'},
        {"version", no_argument, nullptr, 'v'},
        {"help", no_argument, nullptr, 'h'}};

    /* Parse command line options. */
    int c;
    while ((c = getopt_long(argc, argv, "nTvh", long_options, nullptr)) != -1) {
        switch (c) {
        case 'n':
            icat.show_numbers = true;
            break;
        case 'T':
            icat.show_tabs = true;
            break;
        case 'h':
            usage(EXIT_SUCCESS);
        case 'v':
            printf("icat version: %s\n", ICAT_VERSION);
            exit(EXIT_SUCCESS);
        default:
            usage(EXIT_FAILURE);
        }
    }

    if (optind < argc) {
        while (optind < argc) {
            file = argv[optind++];
            FILE *fp = fopen(file, "r");
            streamcopy(fp, stdout);
            fclose(fp);
        }
    } else {
        streamcopy(stdin, stdout);
    }

    return EXIT_SUCCESS;
}

void usage(int status) {
    const char *usage = "Usage: icat [OPTION]... [FILE]...\n"
                        "Concatenate FILE(s) to standard output.\n"
                        "\n";
    printf("%s", usage);
    exit(status);
}

void streamcopy(FILE *fin, FILE *fout) {
    char *buffer;
    int n = 1;
    while ((buffer = readline(fin)) != nullptr) {
        if (icat.show_numbers) {
            printf(DARY_GRAY "%8d  " NONE, n++);
        }
        printf("%s\n", buffer);
    }
}

/* Read a line from FILE stream as a null terminated string.  If buffer is
   non NULL use at most buffer_size bytes and return a pointer to buffer.
   Otherwise return a newly malloced buffer. if EOF is read this function
   returns NULL.  */
char *readline(FILE *istream) {
    int ix = 0, buffer_size = 64;
    char *buffer;

    buffer = (char *)malloc(buffer_size);

    for (;; ++ix) {
        int ch;

        if (ix == buffer_size - 1) {
            if (!buffer_size)
                break;
            buffer_size += buffer_size;
            buffer = (char *)realloc(buffer, buffer_size);
        }

        ch = getc(istream);
        if (ch == EOF) {
            free(buffer);
            return nullptr;
        }
        if (ch == '\n')
            break;
        buffer[ix] = ch;
    }

    /* 0 terminate buffer. */
    buffer[ix] = '\0';

    return buffer;
}
