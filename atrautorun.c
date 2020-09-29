// This directive is used to deactivate the safety warning for the use 
// of fopen under Microsoft Visual Studio.
#pragma warning(disable : 4996)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define ERL_WRONG_OPTIONS               1
#define ERL_MISSING_INPUT_DISK          2
#define ERL_MISSING_OUTPUT_DISK         3
#define ERL_MISSING_EXECUTABLE          4
#define ERL_INVALID_EXECUTABLE          5
#define ERL_CANNOT_OPEN_INPUT           6
#define ERL_CANNOT_OPEN_OUTPUT          7
#define ERL_EXECUTABLE_NOT_PRESENT      8
#define ERL_OUT_OF_MEMORY               9
#define ERL_CANNOT_READ_INPUT           10

// Filename of the disk image (input)
char* filename_disk_in = NULL;

// Filename of the disk image (output)
char* filename_disk_out = NULL;

// Filename of the file inside the input image
// to be renamed into "autorun.sys"
char* filename_executable = NULL;

// Execution verbose?
int verbose = 0;

// This function prints a short guide to the available options.
void usage_and_exit(int _level, int _argc, char* _argv[]) {

    printf("atrautorun - Utility to make autorunning ATR disk images\n");
    printf("Copyright(c) 2020 by Marco Spedaletti.\n");
    printf("Licensed under CC-BY-NC-SA\n\n");
    printf("Usage: %s [options]", _argv[0]);
    printf("\n");
    printf("options:\n");
    printf("\n");
    printf("[mandatory]\n");
    printf("\n");
    printf(" -i <filename>      input filename of source atr disk image\n");
    printf(" -o <filename>      output filename of modified atr disk image\n");
    printf(" -f <filename>      executable file to autorun\n");
    printf(" ");
    printf("[optional]\n");
    printf("\n");
    printf(" -v                 make execution verbose\n");
    printf(" ");

    exit(_level);

}

// This function allows to parse the options entered on the command line. 
// Options must start with a minus character ('-') and only the first letter 
// is considered.

void parse_options(int _argc, char* _argv[]) {

    // Used as index.
    int i;

    // We check for each option...
    for (i = 1; i < _argc; ++i) {

        // Parse it only if begins with '-'
        if (_argv[i][0] == '-') {

            switch (_argv[i][1]) {
            case 'i': // "-i <filename>"
                filename_disk_in = _argv[i + 1];
                ++i;
                break;
            case 'o': // "-o <filename>"
                filename_disk_out = _argv[i + 1];
                ++i;
                break;
            case 'f': // "-f <filename>"
                filename_executable = _argv[i + 1];
                ++i;
                break;
            case 'v': // "-v"
                verbose = 1;
                break;
            default:
                printf("Unknown option: %s", _argv[i]);
                usage_and_exit(ERL_WRONG_OPTIONS, _argc, _argv);
            }

        }
    }

}

// Main function
int main(int _argc, char* _argv[]) {

    FILE* diskIn = NULL;
    FILE* diskOut = NULL;
    unsigned char * disk = NULL;
    int diskSize = 0;
    unsigned char* pos = NULL;
    unsigned char realName[8+3];
    int i = 0, j = 0;
    int found = 0;

    parse_options(_argc, _argv);

    if (filename_disk_in == NULL) {
        printf("Missing input disk image filename.\n");
        usage_and_exit(ERL_MISSING_INPUT_DISK, _argc, _argv);
    }

    if (filename_disk_out == NULL) {
        printf("Missing output disk image filename.\n");
        usage_and_exit(ERL_MISSING_INPUT_DISK, _argc, _argv);
    }

    if (filename_executable == NULL) {
        printf("Missing executable filename.\n");
        usage_and_exit(ERL_MISSING_EXECUTABLE, _argc, _argv);
    }

    if (strlen(filename_executable) == 0 || strlen(filename_executable) > 11) {
        printf("Invalid executable filename %s.\n", filename_executable);
        usage_and_exit(ERL_INVALID_EXECUTABLE, _argc, _argv);
    }

    if (verbose) {
        printf("Input disk image : %s\n", filename_disk_in);
        printf("Output disk image: %s\n", filename_disk_out);
        printf("Replacing        : %s -> AUTORUN.SYS\n\n", filename_executable);
    }

    pos = filename_executable;
    while (*(pos)) {
        if (*pos == '.') {
            *pos = 0;
            memset(realName, 32, 8 + 3);
            strncpy(realName, filename_executable, 8);
            strncpy(realName + 8, pos+1, 3);
            *pos = '.';
            break;
        }
        ++pos;
    }

    for (i = 0; i < sizeof(realName); ++i) {
        if (realName[i] == 0) realName[i] = 32;
        realName[i] = toupper(realName[i]);
    }

    diskIn = fopen(filename_disk_in, "rb");

    if (diskIn == NULL) {
        printf("Cannot open input disk image %s.\n", filename_disk_in);
        usage_and_exit(ERL_CANNOT_OPEN_INPUT, _argc, _argv);
    }

    fseek(diskIn, 0, SEEK_END);
    diskSize = ftell(diskIn);
    fseek(diskIn, 0, SEEK_SET);

    disk = malloc(diskSize);

    if (disk == NULL) {
        printf("Out of memory during reading of image %s.\n", filename_disk_in);
        usage_and_exit(ERL_OUT_OF_MEMORY, _argc, _argv);
    }

    if (fread(disk, 1, diskSize, diskIn) != diskSize) {
        printf("Error during reading of image %s.\n", filename_disk_in);
        usage_and_exit(ERL_CANNOT_READ_INPUT, _argc, _argv);
    };

    for (i = 0; i < diskSize; ++i) {
        for (j = 0; j < 11; ++j) {
            if (disk[i+j] != realName[j]) {
                break;
            }
        }
        if (j == 11) {
            memcpy(&disk[i], "AUTORUN SYS", 11);
            if (verbose) {
                printf(" - replaced %s [%-11.11s] at 0x%04.4x\n", filename_executable, realName, i);
            }
            i += 11;
            found = 1;
        }
    }

    if (!found) {
        printf("Executable %s [%-11.11s] not present.\n", filename_executable, realName);
        usage_and_exit(ERL_CANNOT_READ_INPUT, _argc, _argv);
    }

    diskOut = fopen(filename_disk_out, "wb");

    if (diskOut == NULL) {
        printf("Cannot open output disk image %s.\n", filename_disk_out);
        usage_and_exit(ERL_CANNOT_OPEN_INPUT, _argc, _argv);
    }

    fwrite(disk, 1, diskSize, diskOut);

    fclose(diskIn);
    fclose(diskOut);

    return 0;

}