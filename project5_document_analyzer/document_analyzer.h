// Multi-threaded Document Search and Analysis System
// Author: Nkem Jeferson Achia

#ifndef DOCUMENT_ANALYZER_H
#define DOCUMENT_ANALYZER_H

// ========================= Standard Library Includes ======================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>   // strncasecmp()
#include <ctype.h>
#include <pthread.h>

// ============================== Constants =================================
#define MAX_LINE_LENGTH  1024   // Initial line buffer hint (lines of any length are supported)
#define MAX_FILES        100    // Maximum number of input files supported
#define INITIAL_MATCH_CAP 256   // Initial capacity for matching line numbers

// ========================= Status Code Constants ==========================
#define STATUS_PENDING   0     // Thread has not yet started processing
#define STATUS_SUCCESS   1     // Thread completed processing successfully
#define STATUS_ERROR    -1     // Thread encountered an error during processing

// ========================= Data Structures ================================

// ThreadArgs: holds all data a worker thread needs to process one file.
// Each thread receives a pointer to its own ThreadArgs instance, so no
// data is shared between threads and no synchronization is required.
typedef struct {
    char inputFilename[256];       // Path to the document to be searched
    char keyword[100];             // Keyword to search for (case-insensitive)
    int  threadID;                 // 1-based identifier for display purposes
    int  totalLines;               // Total lines read from the document
    int  keywordCount;             // Total keyword occurrences across all lines
    int  matchingLines;            // Number of lines with at least one match
    int *matchingLineNumbers;      // Dynamically allocated array of line nums
    int  maxMatchingLines;         // Current capacity of matchingLineNumbers
    int  status;                   // 0=pending, 1=success, -1=error
    char errorMessage[512];        // Descriptive error message (if any)
} ThreadArgs;

// ========================= Function Prototypes ============================

// Initialise a ThreadArgs struct with default values.
void initThreadArgs(ThreadArgs *args, const char *filename,
                    const char *keyword, int threadID);

// Thread entry point. Opens the file in ThreadArgs, reads it line by line,
// counts keyword occurrences, records matching lines and writes the report.
void *processFile(void *arg);

// Count case-insensitive occurrences of keyword in a line.
int searchLine(const char *line, const char *keyword);

// Write <inputFilename without extension>_report.txt for one thread's results.
int generateReport(const ThreadArgs *args);

// Print the final processing summary to the terminal.
void displaySummary(ThreadArgs *allArgs[], int numFiles);

// Free dynamically allocated memory in a ThreadArgs.
void cleanupThreadArgs(ThreadArgs *args);

// Print usage information to stderr.
void printUsage(const char *programName);

#endif // DOCUMENT_ANALYZER_H
