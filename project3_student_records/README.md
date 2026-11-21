# Student Academic Records and Performance Analytics System

## Project Description

A comprehensive, menu-driven C program for managing student academic records and performing performance analytics. The system allows administrators to register students, manage course records, search and sort data, generate academic reports, and persist all records to a text file.

This project demonstrates proficiency in **C programming** concepts including structures, dynamic memory management (`malloc`, `calloc`, `realloc`, `free`), pointers, file I/O, manual search and sort algorithms, and modular code organization.

## Features

- **Student Registration** — Add new students with unique IDs, personal information, and programme of study
- **Display All Students** — View a summary table of all registered students
- **View Student Details** — See full details including all course records for a specific student
- **Update Student Information** — Modify student name, programme, or year of study
- **Delete Student Record** — Remove a student and free associated memory
- **Add Course** — Enroll a student in a new course with marks
- **Update Course Marks** — Modify marks for an existing course
- **Academic Performance Display** — View individual student percentage and pass/fail status per course
- **Search Students** — Search by Student ID, name (case-insensitive), or programme
- **Sort Students** — Sort by ID, name, academic average, or programme (manual bubble/insertion sort)
- **Generate Academic Report** — Comprehensive analytics including averages, pass/fail counts, per-course and per-programme statistics
- **File Persistence** — Save to and load from `students_data.txt`
- **Input Validation** — Duplicate ID prevention, marks range checking, year validation, error handling

## Compilation

```bash
gcc -o student_records student_records.c
```

Or with warnings enabled (recommended):

```bash
gcc -Wall -Wextra -o student_records student_records.c
```

## Usage

```bash
./student_records
```

On startup, the system automatically attempts to load existing records from `students_data.txt`. Use the interactive menu to manage records:

```
===== STUDENT ACADEMIC RECORDS SYSTEM =====
1.  Register New Student
2.  Display All Students
3.  View Student Details
4.  Update Student Information
5.  Delete Student Record
6.  Add Course to Student
7.  Update Course Marks
8.  Display Academic Performance
9.  Search Students
10. Sort Students
11. Generate Academic Report
12. Save Records to File
13. Load Records from File
0.  Exit
```

## File Format

Records are saved to `students_data.txt` in the following plain-text format:

```
STUDENT_COUNT: <n>
---
ID: <studentID>
FIRST_NAME: <firstName>
LAST_NAME: <lastName>
PROGRAMME: <programme>
YEAR: <yearOfStudy>
NUM_COURSES: <numCourses>
COURSE: <courseCode> | <courseName> | <marksObtained> | <maxMarks>
COURSE: <courseCode> | <courseName> | <marksObtained> | <maxMarks>
---
```

Each student block is delimited by `---` lines. Course entries appear one per line within the student block.

## Memory Management

- The global student array is allocated with `malloc()` and grown/shrunk with `realloc()`
- Each student's course array is allocated with `malloc()` and grown with `realloc()`
- All memory is properly freed before program exit
- Allocation failures are detected and reported with error messages

## Search Algorithms

All searches are implemented manually (no `bsearch()`):
- **By Student ID** — Linear search returning the index
- **By Name** — Case-insensitive substring match on first or last name
- **By Programme** — Case-insensitive substring match on programme field

## Sort Algorithms

All sorts are implemented manually (no `qsort()`):
- **By Student ID** — Bubble sort on integer IDs
- **By Name** — Bubble sort using `strcmp()` on last name, then first name
- **By Academic Average** — Bubble sort on computed average percentage (descending)
- **By Programme** — Bubble sort using `strcmp()` on programme field

## Author

Nkem Jeferson Achia — C Programming Course, 2026
