#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define ICAT_VERSION "v0.1.0"

void usage(int status);
void streamcopy(FILE *fin, FILE *fout);

struct icat {
    bool number;
};

int main(int argc, char *argv[]) {
    char *file;

    struct icat icat = {false};

    static struct option const long_options[] = {
        {"number", no_argument, nullptr, 'n'},
        {"version", no_argument, nullptr, 'v'},
        {"help", no_argument, nullptr, 'h'}};

    /* Parse command line options. */
    int c;
    while ((c = getopt_long(argc, argv, "nvh", long_options, nullptr)) != -1) {
        switch (c) {
        case 'n':
            icat.number = true;
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
    const char *msg = "icat: a cat like command;\n"
                      "  usage: icat {file}\n";
    printf("icat version: %s\n\n", ICAT_VERSION);
    printf("%s", msg);
    if (status == EXIT_FAILURE) {
        exit(EXIT_FAILURE);
    }
}

void streamcopy(FILE *fin, FILE *fout) {
    int c;
    while ((c = getc(fin)) != EOF) {
        putc(c, fout);
    }
}
