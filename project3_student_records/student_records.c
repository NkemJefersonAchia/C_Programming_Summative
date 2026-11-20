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
 * Returns: average percentage (0-100), or 0.0 if student has no courses.
 */
float calculateStudentAverage(Student *student) {
    int i;
    float totalPercentage = 0.0f;

    if (student->numCourses == 0) {
        return 0.0f;
    }

    for (i = 0; i < student->numCourses; i++) {
        CourseRecord *course = student->courses + i;
        if (course->maxMarks > 0) {
            totalPercentage += (course->marksObtained / course->maxMarks) * 100.0f;
        }
    }

    return totalPercentage / student->numCourses;
}

/*
 * displayStudentSummary
 * ---------------------
 * Prints a one-line summary of a student (for table display).
 */
void displayStudentSummary(Student *student) {
    float avg = calculateStudentAverage(student);
    printf("| %-8d | %-15s | %-15s | %-20s | %-4d | %-8d | %6.2f%% |\n",
           student->studentID,
           student->firstName,
           student->lastName,
           student->programme,
           student->yearOfStudy,
           student->numCourses,
           avg);
}

/*
 * displayStudentFull
 * ------------------
 * Prints full details of a student including all course records.
 */
void displayStudentFull(Student *student) {
    int i;
    float avg;

    printf("\n--- Student Details ---\n");
    printf("Student ID  : %d\n", student->studentID);
    printf("Name        : %s %s\n", student->firstName, student->lastName);
    printf("Programme   : %s\n", student->programme);
    printf("Year of Study: %d\n", student->yearOfStudy);
    printf("Courses     : %d\n", student->numCourses);

    if (student->numCourses > 0) {
        printf("\n  %-15s %-30s %10s %10s %8s\n",
               "Code", "Course Name", "Marks", "Max", "Pct(%)");
        printf("  ");
        for (i = 0; i < 78; i++) printf("-");
        printf("\n");

        for (i = 0; i < student->numCourses; i++) {
            CourseRecord *c = student->courses + i;
            float pct = (c->maxMarks > 0) ? (c->marksObtained / c->maxMarks) * 100.0f : 0.0f;
            printf("  %-15s %-30s %10.2f %10.2f %7.2f%%\n",
                   c->courseCode, c->courseName,
                   c->marksObtained, c->maxMarks, pct);
        }
    }

    avg = calculateStudentAverage(student);
    printf("\n  Overall Average: %.2f%%\n", avg);
    printf("  Status         : %s\n", (avg >= PASS_THRESHOLD) ? "PASSING" : "FAILING");
    printf("-----------------------\n");
}

/*
 * freeAllMemory
 * -------------
 * Frees all dynamically allocated memory: each student's courses array,
 * then the students array itself.
 */
void freeAllMemory(Student *students, int numStudents) {
    int i;
    if (students == NULL) return;

    for (i = 0; i < numStudents; i++) {
        if ((students + i)->courses != NULL) {
            free((students + i)->courses);
            (students + i)->courses = NULL;
        }
    }
    free(students);
}

/* ============================================================================
 *                            MENU AND UI
 * ============================================================================ */

/*
 * displayMenu
 * -----------
 * Displays the main menu options to the user.
 */
void displayMenu(void) {
    printf("\n===== STUDENT ACADEMIC RECORDS SYSTEM =====\n");
    printf("1.  Register New Student\n");
    printf("2.  Display All Students\n");
    printf("3.  View Student Details\n");
    printf("4.  Update Student Information\n");
    printf("5.  Delete Student Record\n");
    printf("6.  Add Course to Student\n");
    printf("7.  Update Course Marks\n");
    printf("8.  Display Academic Performance\n");
    printf("9.  Search Students\n");
    printf("10. Sort Students\n");
    printf("11. Generate Academic Report\n");
    printf("12. Save Records to File\n");
    printf("13. Load Records from File\n");
    printf("0.  Exit\n");
    printf("============================================\n");
    printf("Enter your choice: ");
}

/* ============================================================================
 *                       STUDENT CRUD OPERATIONS
 * ============================================================================ */

/*
 * registerStudent
 * ---------------
