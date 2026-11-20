/*
 * ============================================================================
 * Project:     Student Academic Records and Performance Analytics System
 * File:        student_records.c
 * Description: A comprehensive menu-driven C program for managing student
 *              academic records and generating performance analytics.
 *              Demonstrates dynamic memory management (malloc, calloc,
 *              realloc, free), pointers, structures, file I/O, and manual
 *              implementations of search and sort algorithms.
 * Author:      Nkem Jeferson Achia
 * Course:      C Programming
 * Date:        2026
 * ============================================================================
 */

#include "student_records.h"

/* ============================================================================
 *                              UTILITY FUNCTIONS
 * ============================================================================ */

/*
 * clearInputBuffer
 * ----------------
 * Clears any remaining characters from stdin to prevent input issues
 * after using scanf.
 */
void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/*
 * strToLower
 * ----------
 * Converts a string to lowercase and stores the result in dest.
 * Parameters:
 *   dest - destination buffer (must be large enough)
 *   src  - source string
 */
void strToLower(char *dest, const char *src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = (char)tolower((unsigned char)src[i]);
        i++;
    }
    dest[i] = '\0';
}

/*
 * isDuplicateID
 * -------------
 * Checks whether a student ID already exists in the array.
 * Returns: 1 if duplicate found, 0 otherwise.
 */
int isDuplicateID(Student *students, int numStudents, int id) {
    int i;
    for (i = 0; i < numStudents; i++) {
        if ((students + i)->studentID == id) {
            return 1;
        }
    }
    return 0;
}

/*
 * calculateStudentAverage
 * -----------------------
 * Calculates the overall average percentage for a student across all courses.
