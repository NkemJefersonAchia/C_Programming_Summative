# Project 5: Multi-threaded Document Search and Analysis System

## Description

A multi-threaded command-line tool written in C that concurrently searches multiple text documents for a user-specified keyword. The program uses **POSIX threads (pthreads)** to process each input file in parallel, performing case-insensitive keyword matching and generating per-file analysis reports.

## Features

- **Multi-threaded processing** — One thread per input file for concurrent analysis
- **Case-insensitive search** — Finds keywords regardless of capitalisation (e.g., `IoT`, `iot`, `IOT`)
- **Occurrence counting** — Counts every occurrence of the keyword, including multiple per line
- **Line tracking** — Records which line numbers contain the keyword
- **Per-file reports** — Each file gets a detailed `_report.txt` with search results
- **Terminal summary** — Consolidated results printed to stdout after all threads complete
- **Robust error handling** — Gracefully handles missing files, memory failures, and thread errors
- **Clean memory management** — All dynamic allocations are properly freed

## Project Structure

| File | Description |
|------|-------------|
| `document_analyzer.h` | Header with struct definitions and function prototypes |
| `document_analyzer.c` | Main implementation with all functions and `main()` |
| `sample_document1.txt` | Sample document about IoT applications and trends |
| `sample_document2.txt` | Sample document about cloud computing and IoT |
| `sample_document3.txt` | Sample document about embedded systems and IoT |
| `README.md` | This file |

## Key Concepts Demonstrated

- **POSIX Threads (`pthread_create`, `pthread_join`)** for concurrent file processing
- **Dynamic memory allocation** (`malloc`, `realloc`, `free`) for matching line storage
- **Structs and pointers** for passing data between main and worker threads
- **Modular function design** with clear separation of concerns
- **Case-insensitive string searching** using `strncasecmp()`

## Compilation

```bash
gcc -o document_analyzer document_analyzer.c -lpthread
```

Or equivalently:

```bash
gcc -o document_analyzer document_analyzer.c -pthread
```

## Usage

```bash
./document_analyzer "keyword" file1.txt [file2.txt ...]
```

### Examples

Search for "Cameroon" across the three sample documents:

```bash
./document_analyzer "IoT" sample_document1.txt sample_document2.txt sample_document3.txt
```

Search for a different keyword:

```bash
./document_analyzer "English" sample_document2.txt
```

Search a single file:

```bash
./document_analyzer "German" sample_document3.txt
```

## Sample Output

```
===== MULTI-THREADED DOCUMENT ANALYZER =====

Search Keyword: Cameroon
Number of Input Files: 3

[Thread 1] Processing sample_document1.txt ...
[Thread 2] Processing sample_document2.txt ...
[Thread 3] Processing sample_document3.txt ...

[Thread 1] Successfully processed sample_document1.txt
[Thread 2] Successfully processed sample_document2.txt
[Thread 3] Successfully processed sample_document3.txt

===== PROCESSING SUMMARY =====

sample_document1.txt: Completed - 4 occurrences found in 4 lines
sample_document2.txt: Completed - 9 occurrences found in 8 lines
sample_document3.txt: Completed - 4 occurrences found in 4 lines

All processing tasks completed.
Reports generated successfully.
```

## Error Handling

| Scenario | Behaviour |
|----------|-----------|
| No arguments / missing keyword | Prints usage information and exits |
| File does not exist | Thread reports error; other files still processed |
| Cannot create report file | Thread reports I/O error in status |
| Thread creation fails | Error logged; remaining files still processed |
| Keyword not found | Report generated with 0 matches |
| Memory allocation failure | Warning printed; processing continues where possible |

## Modular Functions

| Function | Purpose |
|----------|---------|
| `initThreadArgs()` | Initialise a ThreadArgs struct with defaults |
| `processFile()` | Thread entry point — searches a file for the keyword |
| `searchLine()` | Count keyword occurrences in a single line |
| `generateReport()` | Write a per-file report to `<filename>_report.txt` |
| `displaySummary()` | Print final results to the terminal |
| `cleanupThreadArgs()` | Free dynamically allocated memory |
| `printUsage()` | Display command-line usage information |

## Author

Nkem Jeferson Achia — C Programming, Trimester 6 Summative
