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
 * Registers a new student by collecting their information and adding them
 * to the dynamically allocated students array using realloc().
 * Validates for duplicate IDs and valid year of study.
 */
void registerStudent(Student **students, int *numStudents) {
    int id, year;
    Student *temp;
    Student *newStudent;

    printf("\n--- Register New Student ---\n");

    /* Get and validate student ID */
    printf("Enter Student ID: ");
    if (scanf("%d", &id) != 1) {
        printf("Error: Invalid input for Student ID.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    /* Check for duplicate ID */
    if (isDuplicateID(*students, *numStudents, id)) {
        printf("Error: Student ID %d already exists.\n", id);
        return;
    }

    /* Expand the students array using realloc */
    temp = (Student *)realloc(*students, (size_t)(*numStudents + 1) * sizeof(Student));
    if (temp == NULL) {
        printf("Error: Memory allocation failed. Cannot register student.\n");
        return;
    }
    *students = temp;

    /* Pointer to the new student slot */
    newStudent = *students + *numStudents;

    /* Initialize the new student */
    newStudent->studentID = id;
    newStudent->numCourses = 0;
    newStudent->courses = NULL;

    /* Get first name */
    printf("Enter First Name: ");
    fgets(newStudent->firstName, MAX_NAME_LEN, stdin);
    newStudent->firstName[strcspn(newStudent->firstName, "\n")] = '\0';

    /* Get last name */
    printf("Enter Last Name: ");
    fgets(newStudent->lastName, MAX_NAME_LEN, stdin);
    newStudent->lastName[strcspn(newStudent->lastName, "\n")] = '\0';

    /* Get programme */
    printf("Enter Programme: ");
    fgets(newStudent->programme, MAX_PROGRAMME_LEN, stdin);
    newStudent->programme[strcspn(newStudent->programme, "\n")] = '\0';

    /* Get and validate year of study */
    printf("Enter Year of Study (%d-%d): ", MIN_YEAR, MAX_YEAR);
    if (scanf("%d", &year) != 1 || year < MIN_YEAR || year > MAX_YEAR) {
        printf("Error: Year of study must be between %d and %d.\n", MIN_YEAR, MAX_YEAR);
        clearInputBuffer();
        /* Remove the partially added student by not incrementing count */
        return;
    }
    clearInputBuffer();
    newStudent->yearOfStudy = year;

    (*numStudents)++;
    printf("Student '%s %s' (ID: %d) registered successfully.\n",
           newStudent->firstName, newStudent->lastName, newStudent->studentID);
}

/*
 * displayAllStudents
 * ------------------
 * Displays a formatted table of all registered students with summary info.
 */
void displayAllStudents(Student *students, int numStudents) {
    int i;

    printf("\n--- All Registered Students ---\n");
    if (numStudents == 0) {
        printf("No students registered.\n");
        return;
    }

    printf("+----------+-----------------+-----------------+----------------------+------+----------+---------+\n");
    printf("| %-8s | %-15s | %-15s | %-20s | %-4s | %-8s | %-7s |\n",
           "ID", "First Name", "Last Name", "Programme", "Year", "Courses", "Average");
    printf("+----------+-----------------+-----------------+----------------------+------+----------+---------+\n");

    for (i = 0; i < numStudents; i++) {
        displayStudentSummary(students + i);
    }

    printf("+----------+-----------------+-----------------+----------------------+------+----------+---------+\n");
    printf("Total students: %d\n", numStudents);
}

/*
 * viewStudentDetails
 * ------------------
 * Prompts for a student ID and displays their full record including courses.
 */
void viewStudentDetails(Student *students, int numStudents) {
    int id, index;

    printf("\n--- View Student Details ---\n");
    if (numStudents == 0) {
        printf("No students registered.\n");
        return;
    }

    printf("Enter Student ID: ");
    if (scanf("%d", &id) != 1) {
        printf("Error: Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    index = searchByID(students, numStudents, id);
    if (index == -1) {
        printf("Error: Student with ID %d not found.\n", id);
        return;
    }

    displayStudentFull(students + index);
}

/*
 * updateStudentInfo
 * -----------------
 * Updates a student's personal information (name, programme, year).
 * Provides a sub-menu for selecting which field to update.
 */
void updateStudentInfo(Student *students, int numStudents) {
    int id, index, choice, year;
    Student *s;

    printf("\n--- Update Student Information ---\n");
    if (numStudents == 0) {
        printf("No students registered.\n");
        return;
    }

    printf("Enter Student ID: ");
    if (scanf("%d", &id) != 1) {
        printf("Error: Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    index = searchByID(students, numStudents, id);
    if (index == -1) {
        printf("Error: Student with ID %d not found.\n", id);
        return;
    }

    s = students + index;
    printf("Current info: %s %s, Programme: %s, Year: %d\n",
           s->firstName, s->lastName, s->programme, s->yearOfStudy);

    printf("\nWhat would you like to update?\n");
    printf("1. First Name\n");
    printf("2. Last Name\n");
    printf("3. Programme\n");
    printf("4. Year of Study\n");
    printf("Choice: ");
    if (scanf("%d", &choice) != 1) {
        printf("Error: Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    switch (choice) {
        case 1:
            printf("Enter new First Name: ");
            fgets(s->firstName, MAX_NAME_LEN, stdin);
            s->firstName[strcspn(s->firstName, "\n")] = '\0';
            printf("First name updated.\n");
            break;
        case 2:
            printf("Enter new Last Name: ");
            fgets(s->lastName, MAX_NAME_LEN, stdin);
            s->lastName[strcspn(s->lastName, "\n")] = '\0';
            printf("Last name updated.\n");
            break;
        case 3:
            printf("Enter new Programme: ");
            fgets(s->programme, MAX_PROGRAMME_LEN, stdin);
            s->programme[strcspn(s->programme, "\n")] = '\0';
            printf("Programme updated.\n");
            break;
        case 4:
            printf("Enter new Year of Study (%d-%d): ", MIN_YEAR, MAX_YEAR);
            if (scanf("%d", &year) != 1 || year < MIN_YEAR || year > MAX_YEAR) {
                printf("Error: Year must be between %d and %d.\n", MIN_YEAR, MAX_YEAR);
                clearInputBuffer();
                return;
            }
            clearInputBuffer();
            s->yearOfStudy = year;
            printf("Year of study updated.\n");
            break;
        default:
            printf("Invalid choice.\n");
            break;
    }
}

/*
 * deleteStudent
 * -------------
 * Deletes a student record by ID. Frees the student's course memory,
 * shifts remaining elements, and uses realloc() to shrink the array.
 */
void deleteStudent(Student **students, int *numStudents) {
    int id, index, i;
    Student *temp;

    printf("\n--- Delete Student Record ---\n");
    if (*numStudents == 0) {
        printf("No students registered.\n");
        return;
    }

    printf("Enter Student ID to delete: ");
    if (scanf("%d", &id) != 1) {
        printf("Error: Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    index = searchByID(*students, *numStudents, id);
    if (index == -1) {
        printf("Error: Student with ID %d not found.\n", id);
        return;
    }

    printf("Deleting student: %s %s (ID: %d)\n",
           (*students + index)->firstName,
           (*students + index)->lastName,
           (*students + index)->studentID);

    /* Free the student's courses array */
    if ((*students + index)->courses != NULL) {
        free((*students + index)->courses);
        (*students + index)->courses = NULL;
    }

    /* Shift remaining students left */
    for (i = index; i < *numStudents - 1; i++) {
        *(*students + i) = *(*students + i + 1);
    }

    (*numStudents)--;

    /* Shrink the array or free if empty */
    if (*numStudents == 0) {
        free(*students);
        *students = NULL;
    } else {
        temp = (Student *)realloc(*students, (size_t)(*numStudents) * sizeof(Student));
        if (temp != NULL) {
            *students = temp;
        }
        /* If realloc fails here, the old pointer is still valid */
    }

    printf("Student deleted successfully.\n");
}

/* ============================================================================
 *                          COURSE OPERATIONS
 * ============================================================================ */
