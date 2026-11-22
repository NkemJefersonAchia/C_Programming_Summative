/**
 * =============================================================================
 * Project 5: Multi-threaded Document Search and Analysis System
 * =============================================================================
 *
 * File:        document_analyzer.h
 * Description: Header file containing struct definitions, constants, and
 *              function prototypes for the multi-threaded document analyzer.
 *              This system uses POSIX threads to concurrently search multiple
 *              documents for a user-specified keyword and generates per-file
 *              analysis reports.
 *
 * Author:      Nkem Jeferson Achia
 * Date:        2026-11-21
 * Course:      C Programming — Trimester 6 Summative
 *
 * Compilation: gcc -o document_analyzer document_analyzer.c -lpthread
 * =============================================================================
 */

#ifndef DOCUMENT_ANALYZER_H
#define DOCUMENT_ANALYZER_H

/* ========================= Standard Library Includes ====================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

/* ============================== Constants ================================= */
#define MAX_LINE_LENGTH  1024   /* Maximum length of a single line in a file   */
#define MAX_FILES        100    /* Maximum number of input files supported      */
#define INITIAL_MATCH_CAP 256   /* Initial capacity for matching line numbers  */

/* ========================= Status Code Constants ========================== */
#define STATUS_PENDING   0     /* Thread has not yet started processing        */
#define STATUS_SUCCESS   1     /* Thread completed processing successfully     */
#define STATUS_ERROR    -1     /* Thread encountered an error during processing*/

/* ========================= Data Structures ================================ */

/**
 * ThreadArgs — Holds all data a worker thread needs to process one file.
 *
 * Each thread receives a pointer to its own ThreadArgs instance. The struct
 * carries input parameters (filename, keyword, threadID), output counters,
 * dynamically allocated storage for matching line numbers, and an error
 * reporting field.
 */
typedef struct {
    char inputFilename[256];       /* Path to the document to be searched      */
    char keyword[100];             /* Keyword to search for (case-insensitive) */
    int  threadID;                 /* 1-based identifier for display purposes  */
    int  totalLines;               /* Total lines read from the document       */
    int  keywordCount;             /* Total keyword occurrences across all lines*/
    int  matchingLines;            /* Number of lines with at least one match  */
    int *matchingLineNumbers;      /* Dynamically allocated array of line nums */
    int  maxMatchingLines;         /* Current capacity of matchingLineNumbers  */
    int  status;                   /* 0=pending, 1=success, -1=error           */
    char errorMessage[256];        /* Descriptive error message (if any)       */
} ThreadArgs;

/* ========================= Function Prototypes ============================ */

/**
 * initThreadArgs — Initialise a ThreadArgs struct with default values.
 *
 * @param args      Pointer to the ThreadArgs struct to initialise.
 * @param filename  Path of the input file this thread will process.
 * @param keyword   The search keyword.
 * @param threadID  1-based thread identifier for display.
 */
void initThreadArgs(ThreadArgs *args, const char *filename,
                    const char *keyword, int threadID);

/**
 * processFile — Thread entry point. Searches a file for the keyword.
 *
 * Opens the file specified in ThreadArgs, reads it line-by-line, counts
 * keyword occurrences (case-insensitive), tracks matching line numbers,
 * generates a report file, and updates the status field.
 *
 * @param arg  Pointer to this thread's ThreadArgs (cast from void*).
 * @return     NULL (results are stored in the ThreadArgs struct).
 */
void *processFile(void *arg);

/**
 * searchLine — Count case-insensitive occurrences of keyword in a line.
 *
 * @param line     The text line to search.
 * @param keyword  The keyword to look for.
 * @return         Number of occurrences of keyword in line.
 */
int searchLine(const char *line, const char *keyword);

/**
 * generateReport — Write an analysis report file for one thread's results.
 *
 * Creates a file named <inputFilename>_report.txt containing the search
 * keyword, total lines, occurrences, matching line numbers, and status.
 *
 * @param args  Pointer to the ThreadArgs with completed search results.
 * @return      0 on success, -1 on file I/O error.
 */
int generateReport(const ThreadArgs *args);

/**
 * displaySummary — Print the final processing summary to the terminal.
 *
 * Shows per-file results (occurrences and matching lines) and an overall
 * completion message.
 *
 * @param allArgs   Array of ThreadArgs pointers (one per file).
 * @param numFiles  Number of files processed.
 */
void displaySummary(ThreadArgs *allArgs[], int numFiles);

/**
 * cleanupThreadArgs — Free dynamically allocated memory in a ThreadArgs.
 *
 * @param args  Pointer to the ThreadArgs whose resources should be freed.
 */
void cleanupThreadArgs(ThreadArgs *args);

/**
 * printUsage — Print usage information to stderr.
 *
 * @param programName  The name of the executable (argv[0]).
 */
void printUsage(const char *programName);

#endif /* DOCUMENT_ANALYZER_H */
