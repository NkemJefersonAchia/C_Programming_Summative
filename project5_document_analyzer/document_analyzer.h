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

