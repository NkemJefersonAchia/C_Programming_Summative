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

