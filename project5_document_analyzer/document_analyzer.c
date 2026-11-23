/**
 * =============================================================================
 * Project 5: Multi-threaded Document Search and Analysis System
 * =============================================================================
 *
 * File:        document_analyzer.c
 * Description: Main implementation of the multi-threaded document analyzer.
 *              Uses POSIX threads (pthreads) to concurrently search multiple
 *              text documents for a user-specified keyword. Each thread
 *              processes one file independently, counts keyword occurrences
 *              (case-insensitive), and generates a per-file report.
 *
 * Author:      Nkem Jeferson Achia
 * Date:        2026-11-21
 * Course:      C Programming — Trimester 6 Summative
 *
 * Compilation: gcc -o document_analyzer document_analyzer.c -lpthread
 * Usage:       ./document_analyzer "keyword" file1.txt file2.txt ...
 * =============================================================================
 */

#define _GNU_SOURCE  /* Required for strcasestr() on some platforms */

#include "document_analyzer.h"

/* ======================= Helper: case-insensitive strstr =================== */

/**
 * caseInsensitiveStrstr — Portable case-insensitive substring search.
 *
 * Falls back to a manual implementation when strcasestr() is unavailable.
 *
 * @param haystack  The string to search within.
 * @param needle    The substring to find.
 * @return          Pointer to the first occurrence, or NULL if not found.
 */
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

/* ============================= initThreadArgs ============================= */

/**
 * initThreadArgs — Initialise a ThreadArgs struct with default values.
 *
 * Sets all counters to zero, copies the filename and keyword, allocates
 * initial storage for matching line numbers, and marks status as pending.
 */
void initThreadArgs(ThreadArgs *args, const char *filename,
                    const char *keyword, int threadID)
{
    /* Zero out the entire struct for a clean slate */
    memset(args, 0, sizeof(ThreadArgs));

    /* Copy input parameters safely */
    strncpy(args->inputFilename, filename, sizeof(args->inputFilename) - 1);
    args->inputFilename[sizeof(args->inputFilename) - 1] = '\0';

    strncpy(args->keyword, keyword, sizeof(args->keyword) - 1);
    args->keyword[sizeof(args->keyword) - 1] = '\0';

    args->threadID = threadID;

    /* Initialise counters */
    args->totalLines     = 0;
    args->keywordCount   = 0;
    args->matchingLines  = 0;

    /* Allocate initial capacity for matching line numbers */
    args->maxMatchingLines    = INITIAL_MATCH_CAP;
    args->matchingLineNumbers = (int *)malloc(sizeof(int) * args->maxMatchingLines);
    if (!args->matchingLineNumbers) {
        fprintf(stderr, "[Thread %d] Warning: could not allocate memory for "
                "matching line numbers.\n", threadID);
        args->maxMatchingLines = 0;
    }

    /* Mark as pending */
    args->status = STATUS_PENDING;
    args->errorMessage[0] = '\0';
}

/* =============================== searchLine =============================== */

/**
 * searchLine — Count case-insensitive occurrences of keyword in a line.
 *
 * Scans through the line, advancing past each found occurrence to count
 * all non-overlapping matches.
 */
int searchLine(const char *line, const char *keyword)
{
    int count = 0;
    size_t keyLen = strlen(keyword);

    if (keyLen == 0) return 0;

    const char *pos = line;
    while ((pos = caseInsensitiveStrstr(pos, keyword)) != NULL) {
        count++;
        pos += keyLen;  /* Move past this occurrence */
    }

    return count;
}

/* ============================== processFile =============================== */

/**
 * processFile — Thread entry point that searches a single document.
 *
 * Opens the file, reads each line, searches for the keyword, tracks
 * occurrences and matching line numbers, then generates a report.
 * All results are stored in the thread's ThreadArgs struct.
 */
void *processFile(void *arg)
{
    ThreadArgs *tArgs = (ThreadArgs *)arg;
    char lineBuffer[MAX_LINE_LENGTH];
    int lineNumber = 0;

    /* Attempt to open the input file */
    FILE *fp = fopen(tArgs->inputFilename, "r");
    if (!fp) {
        tArgs->status = STATUS_ERROR;
        snprintf(tArgs->errorMessage, sizeof(tArgs->errorMessage),
                 "Error: Cannot open file '%s'", tArgs->inputFilename);
        fprintf(stderr, "[Thread %d] %s\n", tArgs->threadID, tArgs->errorMessage);
        return NULL;
    }

    /* Read the file line by line */
    while (fgets(lineBuffer, sizeof(lineBuffer), fp) != NULL) {
        lineNumber++;

        /* Search this line for keyword occurrences */
        int occurrences = searchLine(lineBuffer, tArgs->keyword);

        if (occurrences > 0) {
            tArgs->keywordCount += occurrences;
            tArgs->matchingLines++;

            /* Store the matching line number, expanding array if needed */
            if (tArgs->matchingLineNumbers) {
                /* Grow the array if capacity is reached */
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

                /* Store line number (matchingLines is already incremented) */
                if (tArgs->matchingLines <= tArgs->maxMatchingLines) {
                    tArgs->matchingLineNumbers[tArgs->matchingLines - 1] = lineNumber;
                }
            }
        }
    }

    tArgs->totalLines = lineNumber;
    fclose(fp);

    /* Generate the report file for this document */
    if (generateReport(tArgs) != 0) {
        tArgs->status = STATUS_ERROR;
        snprintf(tArgs->errorMessage, sizeof(tArgs->errorMessage),
                 "Error: Failed to generate report for '%s'",
                 tArgs->inputFilename);
        fprintf(stderr, "[Thread %d] %s\n", tArgs->threadID, tArgs->errorMessage);
        return NULL;
    }

    /* Mark as successfully completed */
    tArgs->status = STATUS_SUCCESS;
    printf("[Thread %d] Successfully processed %s\n",
           tArgs->threadID, tArgs->inputFilename);

    return NULL;
}

/* ============================= generateReport ============================= */

/**
 * generateReport — Write an analysis report file for one thread's results.
 *
 * Creates <inputFilename>_report.txt containing a formatted summary of
 * the search results for the processed document.
 */
int generateReport(const ThreadArgs *args)
{
    /* Build the report filename: <inputFilename>_report.txt */
    char reportFilename[512];
    snprintf(reportFilename, sizeof(reportFilename),
             "%s_report.txt", args->inputFilename);

    FILE *fp = fopen(reportFilename, "w");
    if (!fp) {
        return -1;
    }

    /* Write report header */
    fprintf(fp, "============================================================\n");
    fprintf(fp, "       DOCUMENT ANALYSIS REPORT\n");
    fprintf(fp, "============================================================\n\n");

    /* File and keyword info */
    fprintf(fp, "Input File:            %s\n", args->inputFilename);
    fprintf(fp, "Search Keyword:        %s\n", args->keyword);
    fprintf(fp, "------------------------------------------------------------\n\n");

    /* Results summary */
    fprintf(fp, "Total Lines Processed: %d\n", args->totalLines);
    fprintf(fp, "Total Keyword Occurrences: %d\n", args->keywordCount);
    fprintf(fp, "Number of Matching Lines:  %d\n", args->matchingLines);

    /* List matching line numbers */
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

    /* Processing status */
    fprintf(fp, "\n------------------------------------------------------------\n");
    fprintf(fp, "Processing Status:     %s\n",
            (args->status == STATUS_ERROR) ? "Failed" : "Success");

    if (args->status == STATUS_ERROR && args->errorMessage[0] != '\0') {
        fprintf(fp, "Error Details:         %s\n", args->errorMessage);
    }

    fprintf(fp, "============================================================\n");

    fclose(fp);
    return 0;
}

/* ============================ displaySummary ============================== */

/**
 * displaySummary — Print the final processing summary to stdout.
 *
 * Shows per-file results including occurrence count and matching lines,
 * or an error message for files that failed.
 */
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

/* =========================== cleanupThreadArgs ============================ */

/**
 * cleanupThreadArgs — Free dynamically allocated memory in a ThreadArgs.
 *
 * Releases the matchingLineNumbers array and sets the pointer to NULL
 * to prevent double-free errors.
 */
void cleanupThreadArgs(ThreadArgs *args)
{
    if (args && args->matchingLineNumbers) {
        free(args->matchingLineNumbers);
        args->matchingLineNumbers = NULL;
        args->maxMatchingLines = 0;
    }
}

/* =============================== printUsage =============================== */

/**
 * printUsage — Print usage information to stderr.
 *
 * Shows correct command-line syntax and an example invocation.
 */
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

/* ================================== main ================================== */

/**
 * main — Entry point for the multi-threaded document analyzer.
 *
 * Validates command-line arguments, initialises per-file ThreadArgs,
 * spawns one POSIX thread per file, joins all threads, prints the
 * summary, and cleans up allocated resources.
 */
int main(int argc, char *argv[])
{
    /* ---- Validate command-line arguments ---- */
    if (argc < 3) {
        fprintf(stderr, "Error: Insufficient arguments.\n");
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *keyword  = argv[1];
    int         numFiles = argc - 2;  /* Remaining args are file paths */

    /* Enforce maximum file limit */
    if (numFiles > MAX_FILES) {
        fprintf(stderr, "Error: Too many files. Maximum supported: %d\n",
                MAX_FILES);
        return EXIT_FAILURE;
    }

    /* ---- Display banner ---- */
    printf("\n===== MULTI-THREADED DOCUMENT ANALYZER =====\n\n");
    printf("Search Keyword: %s\n", keyword);
    printf("Number of Input Files: %d\n\n", numFiles);

    /* ---- Allocate ThreadArgs and pthread_t arrays ---- */
    ThreadArgs *argsArray[MAX_FILES];
    pthread_t   threads[MAX_FILES];
    int         threadCreated[MAX_FILES];  /* Track which threads were created */

    /* Initialise each ThreadArgs struct */
    for (int i = 0; i < numFiles; i++) {
        argsArray[i] = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        if (!argsArray[i]) {
            fprintf(stderr, "Error: Memory allocation failed for thread %d.\n",
                    i + 1);
            /* Clean up previously allocated structs */
            for (int j = 0; j < i; j++) {
                cleanupThreadArgs(argsArray[j]);
                free(argsArray[j]);
            }
            return EXIT_FAILURE;
        }
        initThreadArgs(argsArray[i], argv[i + 2], keyword, i + 1);
        threadCreated[i] = 0;  /* Not yet created */
    }

    /* ---- Create one thread per input file ---- */
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

    /* ---- Join all successfully created threads ---- */
    for (int i = 0; i < numFiles; i++) {
        if (threadCreated[i]) {
            pthread_join(threads[i], NULL);
        }
    }

    /* ---- Display final summary ---- */
    displaySummary(argsArray, numFiles);

    /* ---- Clean up all allocated resources ---- */
    for (int i = 0; i < numFiles; i++) {
        cleanupThreadArgs(argsArray[i]);
        free(argsArray[i]);
    }

    return EXIT_SUCCESS;
}
