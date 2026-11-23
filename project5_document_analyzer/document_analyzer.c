// Multi-threaded Document Search and Analysis System
// Author: Nkem Jeferson Achia

#define _GNU_SOURCE  // Required for getline()/strcasestr() on some platforms

#include "document_analyzer.h"

// ======================= Helper: case-insensitive strstr ===================

// caseInsensitiveStrstr: portable case-insensitive substring search.
static const char *caseInsensitiveStrstr(const char *haystack, const char *needle)
{
    if (!haystack || !needle) return NULL;
    if (*needle == '\0') return haystack;

    size_t needleLen = strlen(needle);

    for (; *haystack; haystack++) {
        if (strncasecmp(haystack, needle, needleLen) == 0) {
            return haystack;
        }
    }
    return NULL;
}

// ============================= initThreadArgs =============================

// initThreadArgs: initialise a ThreadArgs struct with default values.
// Sets all counters to zero, copies the filename and keyword, and allocates
// the initial array that stores matching line numbers.
void initThreadArgs(ThreadArgs *args, const char *filename,
                    const char *keyword, int threadID)
{
    memset(args, 0, sizeof(ThreadArgs));

    strncpy(args->inputFilename, filename, sizeof(args->inputFilename) - 1);
    args->inputFilename[sizeof(args->inputFilename) - 1] = '\0';

    strncpy(args->keyword, keyword, sizeof(args->keyword) - 1);
    args->keyword[sizeof(args->keyword) - 1] = '\0';

    args->threadID = threadID;

    args->totalLines     = 0;
    args->keywordCount   = 0;
    args->matchingLines  = 0;

    args->maxMatchingLines    = INITIAL_MATCH_CAP;
    args->matchingLineNumbers = (int *)malloc(sizeof(int) * args->maxMatchingLines);
    if (!args->matchingLineNumbers) {
        fprintf(stderr, "[Thread %d] Warning: could not allocate memory for "
                "matching line numbers.\n", threadID);
        args->maxMatchingLines = 0;
    }

    args->status = STATUS_PENDING;
    args->errorMessage[0] = '\0';
}

// =============================== searchLine ===============================

// searchLine: count case-insensitive occurrences of keyword in a line.
// Scans through the line, advancing past each found occurrence so that
// matches are never counted twice.
int searchLine(const char *line, const char *keyword)
{
    int count = 0;
    size_t keyLen = strlen(keyword);

    if (keyLen == 0) return 0;

    const char *pos = line;
    while ((pos = caseInsensitiveStrstr(pos, keyword)) != NULL) {
        count++;
        pos += keyLen;
    }

    return count;
}

// ============================== processFile ===============================

// processFile: thread entry point that searches a single document.
// Opens the file, reads each line (of any length), searches for the keyword,
// tracks the matching line numbers and then writes the report file.
void *processFile(void *arg)
{
    ThreadArgs *tArgs = (ThreadArgs *)arg;
    char  *line = NULL;      // Buffer allocated by getline(), any line length
    size_t lineCap = 0;
    int    lineNumber = 0;

    FILE *fp = fopen(tArgs->inputFilename, "r");
    if (!fp) {
        tArgs->status = STATUS_ERROR;
        snprintf(tArgs->errorMessage, sizeof(tArgs->errorMessage),
                 "Error: Cannot open file '%s'", tArgs->inputFilename);
        fprintf(stderr, "[Thread %d] %s\n", tArgs->threadID, tArgs->errorMessage);
        return NULL;
    }

    while (getline(&line, &lineCap, fp) != -1) {
        lineNumber++;

        int occurrences = searchLine(line, tArgs->keyword);

        if (occurrences > 0) {
            tArgs->keywordCount += occurrences;
            tArgs->matchingLines++;

            if (tArgs->matchingLineNumbers) {
                // Grow the array when capacity is reached
                if (tArgs->matchingLines > tArgs->maxMatchingLines) {
                    int newCap = tArgs->maxMatchingLines * 2;
                    int *temp = (int *)realloc(tArgs->matchingLineNumbers,
                                               sizeof(int) * newCap);
                    if (temp) {
                        tArgs->matchingLineNumbers = temp;
                        tArgs->maxMatchingLines = newCap;
                    } else {
                        fprintf(stderr, "[Thread %d] Warning: could not expand "
                                "matching lines array.\n", tArgs->threadID);
                    }
                }

                if (tArgs->matchingLines <= tArgs->maxMatchingLines) {
                    tArgs->matchingLineNumbers[tArgs->matchingLines - 1] = lineNumber;
                }
            }
        }
    }

    free(line);
    fclose(fp);
    tArgs->totalLines = lineNumber;

    // Set the status before writing the report so the report shows it correctly
    tArgs->status = STATUS_SUCCESS;

    if (generateReport(tArgs) != 0) {
        tArgs->status = STATUS_ERROR;
        snprintf(tArgs->errorMessage, sizeof(tArgs->errorMessage),
                 "Error: Failed to generate report for '%s'",
                 tArgs->inputFilename);
        fprintf(stderr, "[Thread %d] %s\n", tArgs->threadID, tArgs->errorMessage);
        return NULL;
    }

    printf("[Thread %d] Successfully processed %s\n",
           tArgs->threadID, tArgs->inputFilename);

    return NULL;
}

// ============================= generateReport =============================

// generateReport: write an analysis report file for one thread's results.
// Creates <inputFilename without extension>_report.txt, for example
// document1.txt -> document1_report.txt, containing a formatted summary.
int generateReport(const ThreadArgs *args)
{
    // Build the report filename by removing the file extension, if any
    char reportFilename[512];
    const char *lastSlash = strrchr(args->inputFilename, '/');
    const char *lastDot   = strrchr(args->inputFilename, '.');
    int baseLen = (int)strlen(args->inputFilename);

    // Only strip the dot if it belongs to the file name, not to a folder
    if (lastDot && (!lastSlash || lastDot > lastSlash) &&
        lastDot != args->inputFilename && lastDot != lastSlash + 1) {
        baseLen = (int)(lastDot - args->inputFilename);
    }
    snprintf(reportFilename, sizeof(reportFilename),
             "%.*s_report.txt", baseLen, args->inputFilename);

    FILE *fp = fopen(reportFilename, "w");
    if (!fp) {
        return -1;
    }

    fprintf(fp, "============================================================\n");
    fprintf(fp, "       DOCUMENT ANALYSIS REPORT\n");
    fprintf(fp, "============================================================\n\n");

    fprintf(fp, "Input File:            %s\n", args->inputFilename);
    fprintf(fp, "Search Keyword:        %s\n", args->keyword);
    fprintf(fp, "------------------------------------------------------------\n\n");

    fprintf(fp, "Total Lines Processed: %d\n", args->totalLines);
    fprintf(fp, "Total Keyword Occurrences: %d\n", args->keywordCount);
    fprintf(fp, "Number of Matching Lines:  %d\n", args->matchingLines);

    fprintf(fp, "\nMatching Line Numbers: ");
    if (args->matchingLines > 0 && args->matchingLineNumbers) {
        for (int i = 0; i < args->matchingLines; i++) {
            if (i > 0) fprintf(fp, ", ");
            fprintf(fp, "%d", args->matchingLineNumbers[i]);
        }
        fprintf(fp, "\n");
    } else {
        fprintf(fp, "None\n");
    }

    fprintf(fp, "\n------------------------------------------------------------\n");
    fprintf(fp, "Processing Status:     %s\n",
            (args->status == STATUS_SUCCESS) ? "Success" : "Failed");

    if (args->status == STATUS_ERROR && args->errorMessage[0] != '\0') {
        fprintf(fp, "Error Details:         %s\n", args->errorMessage);
    }

    fprintf(fp, "============================================================\n");

    fclose(fp);
    return 0;
}

// ============================ displaySummary ==============================

// displaySummary: print the final processing summary to stdout.
// Shows per-file results including occurrence count and matching lines,
// or the error message for files that failed.
void displaySummary(ThreadArgs *allArgs[], int numFiles)
{
    printf("\n===== PROCESSING SUMMARY =====\n\n");

    for (int i = 0; i < numFiles; i++) {
        if (allArgs[i]->status == STATUS_SUCCESS) {
            printf("%s: Completed - %d occurrences found in %d lines\n",
                   allArgs[i]->inputFilename,
                   allArgs[i]->keywordCount,
                   allArgs[i]->matchingLines);
        } else if (allArgs[i]->status == STATUS_ERROR) {
            printf("%s: Failed - %s\n",
                   allArgs[i]->inputFilename,
                   allArgs[i]->errorMessage);
        } else {
            printf("%s: Status unknown (pending)\n",
                   allArgs[i]->inputFilename);
        }
    }

    printf("\nAll processing tasks completed.\n");
    printf("Reports generated successfully.\n");
}

// =========================== cleanupThreadArgs ============================

// cleanupThreadArgs: free dynamically allocated memory in a ThreadArgs.
// Releases the matchingLineNumbers array and sets the pointer to NULL
// so it cannot be freed twice.
void cleanupThreadArgs(ThreadArgs *args)
{
    if (args && args->matchingLineNumbers) {
        free(args->matchingLineNumbers);
        args->matchingLineNumbers = NULL;
        args->maxMatchingLines = 0;
    }
}

// =============================== printUsage ===============================

// printUsage: print usage information to stderr.
// Shows correct command-line syntax and an example invocation.
void printUsage(const char *programName)
{
    fprintf(stderr, "\nUsage: %s \"keyword\" file1.txt [file2.txt ...]\n\n",
            programName);
    fprintf(stderr, "Arguments:\n");
    fprintf(stderr, "  keyword    The word to search for (case-insensitive)\n");
    fprintf(stderr, "  file(s)    One or more text files to search\n\n");
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "  %s \"IoT\" document1.txt document2.txt document3.txt\n\n",
            programName);
}

// ================================== main ==================================

// main: entry point for the multi-threaded document analyzer.
// Validates command-line arguments, initialises per-file ThreadArgs,
// creates one thread per file, joins them and prints the summary.
int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Error: Insufficient arguments.\n");
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *keyword  = argv[1];
    int         numFiles = argc - 2;

    if (numFiles > MAX_FILES) {
        fprintf(stderr, "Error: Too many files. Maximum supported: %d\n",
                MAX_FILES);
        return EXIT_FAILURE;
    }

    printf("\n===== MULTI-THREADED DOCUMENT ANALYZER =====\n\n");
    printf("Search Keyword: %s\n", keyword);
    printf("Number of Input Files: %d\n\n", numFiles);

    ThreadArgs *argsArray[MAX_FILES];
    pthread_t   threads[MAX_FILES];
    int         threadCreated[MAX_FILES];

    for (int i = 0; i < numFiles; i++) {
        argsArray[i] = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        if (!argsArray[i]) {
            fprintf(stderr, "Error: Memory allocation failed for thread %d.\n",
                    i + 1);
            for (int j = 0; j < i; j++) {
                cleanupThreadArgs(argsArray[j]);
                free(argsArray[j]);
            }
            return EXIT_FAILURE;
        }
        initThreadArgs(argsArray[i], argv[i + 2], keyword, i + 1);
        threadCreated[i] = 0;
    }

    for (int i = 0; i < numFiles; i++) {
        printf("[Thread %d] Processing %s ...\n",
               argsArray[i]->threadID, argsArray[i]->inputFilename);

        int rc = pthread_create(&threads[i], NULL, processFile, argsArray[i]);
        if (rc != 0) {
            fprintf(stderr, "[Thread %d] Error: Failed to create thread "
                    "(error code %d). Skipping file '%s'.\n",
                    argsArray[i]->threadID, rc, argsArray[i]->inputFilename);
            argsArray[i]->status = STATUS_ERROR;
            snprintf(argsArray[i]->errorMessage,
                     sizeof(argsArray[i]->errorMessage),
                     "Thread creation failed (error code %d)", rc);
            threadCreated[i] = 0;
        } else {
            threadCreated[i] = 1;
        }
    }

    printf("\n");

    for (int i = 0; i < numFiles; i++) {
        if (threadCreated[i]) {
            pthread_join(threads[i], NULL);
        }
    }

    displaySummary(argsArray, numFiles);

    for (int i = 0; i < numFiles; i++) {
        cleanupThreadArgs(argsArray[i]);
        free(argsArray[i]);
    }

    return EXIT_SUCCESS;
}
