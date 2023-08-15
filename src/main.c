#include "color.h"
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef nullptr
#define nullptr NULL
#endif /* define nullptr */

#define ICAT_VERSION "v0.1.0"

void usage(int status);
void streamcopy(FILE *fin, FILE *fout);
char *readline(FILE *istream);

struct icat {
    bool show_numbers;
    bool num_nonblank;
    bool show_nonprinting;
    bool squeeze_blank;
    bool show_ends;
    bool show_tabs;
};

struct icat icat = {false, false, false, false, false, false};

int main(int argc, char *argv[]) {
    char *file;

    static struct option const long_options[] = {
        {"number", no_argument, nullptr, 'n'},
        {"number-nonblank", no_argument, nullptr, 'b'},
        {"squeeze-blank", no_argument, nullptr, 's'},
        {"show-nonprinting", no_argument, nullptr, 'v'},
        {"show-tabs", no_argument, nullptr, 'T'},
        {"show-ends", no_argument, nullptr, 'E'},
        {"version", no_argument, nullptr, 'v'},
        {"help", no_argument, nullptr, 'h'}};

    /* Parse command line options. */
    int c;
    while ((c = getopt_long(argc, argv, "nbsTEvh", long_options, nullptr)) !=
           -1) {
        switch (c) {
        case 'n':
            icat.show_numbers = true;
            break;
        case 'b':
            icat.num_nonblank = true;
            break;
        case 's':
            icat.squeeze_blank = true;
            break;
        case 'T':
            icat.show_tabs = true;
            break;
        case 'E':
            icat.show_ends = true;
            break;
        case 'h':
            usage(EXIT_SUCCESS);
        case 'v':
            icat.show_nonprinting = true;
            break;
        default:
            usage(EXIT_FAILURE);
        }
    }

    if (argc > 1) {
        while (optind < argc) {
            if (strcmp(argv[optind], "-") == 0) {
                streamcopy(stdin, stdout);
            } else {
                file = argv[optind];
                FILE *fp = fopen(file, "r");
                streamcopy(fp, stdout);
                fclose(fp);
            }
            optind++;
        }
    } else {
        streamcopy(stdin, stdout);
    }

    return EXIT_SUCCESS;
}

void usage(int status) {
    const char *usage =
        "Usage: icat [OPTION]... [FILE]...\n"
        "Concatenate FILE(s) to standard output.\n"
        "\n"
        "With no FILE, or when FILE is -, read standard input.\n"
        "\n"
        "  -A, --show-all          equivalent to -vET\n"
        "  -b, --number-nonblank   number nonempty output lines, overrides -n\n"
        "  -e                      equivalent to -vE\n"
        "  -E, --show-ends         display $ at end of each line\n"
        "  -n, --number            number all output lines\n"
        "  -s, --squeeze-blank     suppress repeated empty output lines\n"
        "  -t                      equivalent to -vT\n"
        "  -T, --show-tabs         display TAB characters as ^I\n"
        "  -u                      (ignored)\n"
        "  -v, --show-nonprinting  use ^ and M- notation, except for LFD and "
        "TAB\n"
        "      --help              display this help and exit\n"
        "      --version           output version information and exit\n"
        "\n"
        "Examples:\n"
        "  cat f -g  Output f's contents, then standard input, then g's "
        "contents.\n"
        "  cat       Copy standard input to standard output.\n"
        "\n"
        "Copyright: Xiang.C <iflygo@outlook.com>\n"
        "See more information, visit <https://github.com/colorsakura/icat>\n";
    printf("%s", usage);
    exit(status);
}

void streamcopy(FILE *fin, FILE *fout) {
    char *buffer;
    int n = 1;
    bool pre_line_blank = false;
    while ((buffer = readline(fin)) != nullptr) {
        if (icat.squeeze_blank && buffer[0] == '\0') {
            if (pre_line_blank) {
                continue;
            }
            pre_line_blank = true;
        } else {
            pre_line_blank = false;
        }
        if (icat.show_numbers || icat.num_nonblank) {
            if (!(icat.num_nonblank && buffer[0] == '\0'))
                printf(DARY_GRAY "%8d  " NONE, n++);
        }
        fputs(buffer, fout);
        if (icat.show_ends) {
            fputc('$', fout);
        }
        fputc('\n', fout);
    }
}

/* Read a line from FILE stream as a null terminated string.  If the line
   is '\n', return '\0'. if EOF is read this function returns nullptr.  */
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
        if (icat.show_nonprinting && ch > 0 && ch < 27) {
            buffer[ix] = '^';
            buffer[ix + 1] = ch + 40;
            continue;
        }
        buffer[ix] = ch;
    }

    /* 0 terminate buffer. */
    buffer[ix] = '\0';

    return buffer;
}
