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

/*
 * addCourseToStudent
 * ------------------
 * Adds a new course record to a specified student. Dynamically allocates
 * or expands the student's courses array using realloc().
 * Validates marks (0 to maxMarks).
 */
void addCourseToStudent(Student *students, int numStudents) {
    int id, index;
    float marks, maxMarks;
    Student *s;
    CourseRecord *temp;
    CourseRecord *newCourse;

    printf("\n--- Add Course to Student ---\n");
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

    /* Expand the courses array */
    temp = (CourseRecord *)realloc(s->courses, (size_t)(s->numCourses + 1) * sizeof(CourseRecord));
    if (temp == NULL) {
        printf("Error: Memory allocation failed. Cannot add course.\n");
        return;
    }
    s->courses = temp;

    newCourse = s->courses + s->numCourses;

    /* Get course code */
    printf("Enter Course Code: ");
    fgets(newCourse->courseCode, MAX_CODE_LEN, stdin);
    newCourse->courseCode[strcspn(newCourse->courseCode, "\n")] = '\0';

    /* Get course name */
    printf("Enter Course Name: ");
    fgets(newCourse->courseName, MAX_COURSE_NAME, stdin);
    newCourse->courseName[strcspn(newCourse->courseName, "\n")] = '\0';

    /* Get and validate max marks */
    printf("Enter Maximum Marks: ");
    if (scanf("%f", &maxMarks) != 1 || maxMarks <= 0) {
        printf("Error: Maximum marks must be a positive number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    newCourse->maxMarks = maxMarks;

    /* Get and validate marks obtained */
    printf("Enter Marks Obtained (0 to %.2f): ", maxMarks);
    if (scanf("%f", &marks) != 1 || marks < 0 || marks > maxMarks) {
        printf("Error: Marks must be between 0 and %.2f.\n", maxMarks);
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    newCourse->marksObtained = marks;

    s->numCourses++;
    printf("Course '%s' added to student %s %s.\n",
           newCourse->courseName, s->firstName, s->lastName);
}

/*
 * updateCourseMarks
 * -----------------
 * Updates the marks for an existing course of a specified student.
 * Validates that new marks are within [0, maxMarks].
 */
void updateCourseMarks(Student *students, int numStudents) {
    int id, index, i, courseChoice;
    float newMarks;
    Student *s;
    CourseRecord *c;

    printf("\n--- Update Course Marks ---\n");
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
    if (s->numCourses == 0) {
        printf("Student has no courses.\n");
        return;
    }

    /* Display courses */
    printf("Courses for %s %s:\n", s->firstName, s->lastName);
    for (i = 0; i < s->numCourses; i++) {
        c = s->courses + i;
        printf("  %d. %s (%s) — Marks: %.2f / %.2f\n",
               i + 1, c->courseName, c->courseCode,
               c->marksObtained, c->maxMarks);
    }

    printf("Select course number to update: ");
    if (scanf("%d", &courseChoice) != 1 || courseChoice < 1 || courseChoice > s->numCourses) {
        printf("Error: Invalid course selection.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    c = s->courses + (courseChoice - 1);

    printf("Current marks for '%s': %.2f / %.2f\n",
           c->courseName, c->marksObtained, c->maxMarks);
    printf("Enter new marks (0 to %.2f): ", c->maxMarks);
    if (scanf("%f", &newMarks) != 1 || newMarks < 0 || newMarks > c->maxMarks) {
        printf("Error: Marks must be between 0 and %.2f.\n", c->maxMarks);
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    c->marksObtained = newMarks;
    printf("Marks updated to %.2f for '%s'.\n", newMarks, c->courseName);
}

/*
 * displayAcademicPerformance
 * --------------------------
 * Displays the academic performance of a specific student, showing
 * each course's percentage and pass/fail status.
 */
void displayAcademicPerformance(Student *students, int numStudents) {
    int id, index, i;
    Student *s;

    printf("\n--- Academic Performance ---\n");
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
    printf("\nAcademic Performance for %s %s (ID: %d)\n",
           s->firstName, s->lastName, s->studentID);
    printf("Programme: %s, Year: %d\n\n", s->programme, s->yearOfStudy);

    if (s->numCourses == 0) {
        printf("No courses enrolled.\n");
        return;
    }

    printf("%-15s %-30s %8s %8s %8s %10s\n",
           "Code", "Course", "Marks", "Max", "Pct(%)", "Status");
    for (i = 0; i < 85; i++) printf("-");
    printf("\n");

    for (i = 0; i < s->numCourses; i++) {
        CourseRecord *c = s->courses + i;
        float pct = (c->maxMarks > 0) ? (c->marksObtained / c->maxMarks) * 100.0f : 0.0f;
        const char *status = (pct >= PASS_THRESHOLD) ? "PASS" : "FAIL";
        printf("%-15s %-30s %8.2f %8.2f %7.2f%% %10s\n",
               c->courseCode, c->courseName,
               c->marksObtained, c->maxMarks, pct, status);
    }

    printf("\nOverall Average: %.2f%%\n", calculateStudentAverage(s));
}

/* ============================================================================
 *                            SEARCHING
 * ============================================================================ */

/*
 * searchByID
 * ----------
 * Linear search for a student by ID.
 * Returns: index of the student if found, -1 otherwise.
 */
int searchByID(Student *students, int numStudents, int targetID) {
    int i;
    for (i = 0; i < numStudents; i++) {
        if ((students + i)->studentID == targetID) {
            return i;
        }
    }
    return -1;
}

/*
 * searchByName
 * ------------
 * Case-insensitive search for students whose first or last name
 * contains the given search string. Displays all matches.
 */
void searchByName(Student *students, int numStudents, const char *name) {
    int i, found = 0;
    char lowerSearch[MAX_NAME_LEN];
    char lowerFirst[MAX_NAME_LEN];
    char lowerLast[MAX_NAME_LEN];

    strToLower(lowerSearch, name);

    printf("\nSearch results for name '%s':\n", name);
    for (i = 0; i < numStudents; i++) {
        strToLower(lowerFirst, (students + i)->firstName);
        strToLower(lowerLast, (students + i)->lastName);

        if (strstr(lowerFirst, lowerSearch) != NULL ||
            strstr(lowerLast, lowerSearch) != NULL) {
            displayStudentSummary(students + i);
            found++;
        }
    }

    if (found == 0) {
        printf("No students found matching '%s'.\n", name);
    } else {
        printf("Found %d student(s).\n", found);
    }
}

/*
 * searchByProgramme
 * -----------------
 * Case-insensitive search for students in a given programme.
 * Displays all matches.
 */
void searchByProgramme(Student *students, int numStudents, const char *programme) {
    int i, found = 0;
    char lowerSearch[MAX_PROGRAMME_LEN];
    char lowerProg[MAX_PROGRAMME_LEN];

    strToLower(lowerSearch, programme);

    printf("\nSearch results for programme '%s':\n", programme);
    for (i = 0; i < numStudents; i++) {
        strToLower(lowerProg, (students + i)->programme);

        if (strstr(lowerProg, lowerSearch) != NULL) {
            displayStudentSummary(students + i);
            found++;
        }
    }

    if (found == 0) {
        printf("No students found in programme '%s'.\n", programme);
    } else {
        printf("Found %d student(s).\n", found);
    }
}

/*
 * searchStudents
 * --------------
 * Presents a sub-menu for choosing the search type, then performs
 * the appropriate search.
 */
void searchStudents(Student *students, int numStudents) {
    int choice, id, index;
    char searchTerm[MAX_PROGRAMME_LEN];

    printf("\n--- Search Students ---\n");
    if (numStudents == 0) {
        printf("No students registered.\n");
        return;
    }

    printf("Search by:\n");
    printf("1. Student ID\n");
    printf("2. Name (first or last)\n");
    printf("3. Programme\n");
    printf("Choice: ");
    if (scanf("%d", &choice) != 1) {
        printf("Error: Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    switch (choice) {
        case 1:
            printf("Enter Student ID: ");
            if (scanf("%d", &id) != 1) {
                printf("Error: Invalid input.\n");
                clearInputBuffer();
                return;
            }
            clearInputBuffer();
            index = searchByID(students, numStudents, id);
            if (index == -1) {
                printf("Student with ID %d not found.\n", id);
            } else {
                printf("Student found:\n");
                displayStudentFull(students + index);
            }
            break;
        case 2:
            printf("Enter name to search: ");
            fgets(searchTerm, MAX_NAME_LEN, stdin);
            searchTerm[strcspn(searchTerm, "\n")] = '\0';
            searchByName(students, numStudents, searchTerm);
            break;
        case 3:
            printf("Enter programme to search: ");
            fgets(searchTerm, MAX_PROGRAMME_LEN, stdin);
            searchTerm[strcspn(searchTerm, "\n")] = '\0';
            searchByProgramme(students, numStudents, searchTerm);
            break;
        default:
            printf("Invalid search option.\n");
            break;
    }
}

/* ============================================================================
 *                              SORTING
 *       (Manual implementations — NO qsort used)
 * ============================================================================ */

/*
 * sortByID
 * --------
 * Sorts students array by Student ID in ascending order using bubble sort.
 */
void sortByID(Student *students, int numStudents) {
    int i, j;
    Student temp;

    for (i = 0; i < numStudents - 1; i++) {
        for (j = 0; j < numStudents - 1 - i; j++) {
            if ((students + j)->studentID > (students + j + 1)->studentID) {
                temp = *(students + j);
                *(students + j) = *(students + j + 1);
                *(students + j + 1) = temp;
            }
        }
    }
    printf("Students sorted by ID (ascending).\n");
}

/*
 * sortByName
 * ----------
 * Sorts students array alphabetically by last name, then first name,
 * using bubble sort with strcmp().
 */
void sortByName(Student *students, int numStudents) {
    int i, j, cmp;
    Student temp;

    for (i = 0; i < numStudents - 1; i++) {
        for (j = 0; j < numStudents - 1 - i; j++) {
            cmp = strcmp((students + j)->lastName, (students + j + 1)->lastName);
            if (cmp == 0) {
                cmp = strcmp((students + j)->firstName, (students + j + 1)->firstName);
            }
            if (cmp > 0) {
                temp = *(students + j);
                *(students + j) = *(students + j + 1);
                *(students + j + 1) = temp;
            }
        }
    }
    printf("Students sorted by name (alphabetical).\n");
}

/*
 * sortByAverage
 * -------------
 * Sorts students array by overall academic average in descending order
 * using bubble sort.
 */
void sortByAverage(Student *students, int numStudents) {
    int i, j;
    Student temp;
    float avgJ, avgJ1;

    for (i = 0; i < numStudents - 1; i++) {
        for (j = 0; j < numStudents - 1 - i; j++) {
            avgJ = calculateStudentAverage(students + j);
            avgJ1 = calculateStudentAverage(students + j + 1);
            if (avgJ < avgJ1) {  /* Descending order */
                temp = *(students + j);
                *(students + j) = *(students + j + 1);
                *(students + j + 1) = temp;
            }
        }
    }
    printf("Students sorted by academic average (descending).\n");
}

/*
 * sortByProgramme
 * ---------------
 * Sorts students array alphabetically by programme using bubble sort.
 */
void sortByProgramme(Student *students, int numStudents) {
    int i, j;
    Student temp;

    for (i = 0; i < numStudents - 1; i++) {
        for (j = 0; j < numStudents - 1 - i; j++) {
            if (strcmp((students + j)->programme, (students + j + 1)->programme) > 0) {
                temp = *(students + j);
                *(students + j) = *(students + j + 1);
                *(students + j + 1) = temp;
            }
        }
    }
    printf("Students sorted by programme (alphabetical).\n");
}

/*
 * sortStudents
 * ------------
 * Presents a sub-menu for choosing the sort criteria, then performs
 * the appropriate sort using manual bubble sort.
 */
void sortStudents(Student *students, int numStudents) {
    int choice;

    printf("\n--- Sort Students ---\n");
    if (numStudents == 0) {
        printf("No students to sort.\n");
        return;
    }

    printf("Sort by:\n");
    printf("1. Student ID\n");
    printf("2. Name (alphabetical)\n");
    printf("3. Academic Average\n");
    printf("4. Programme\n");
    printf("Choice: ");
    if (scanf("%d", &choice) != 1) {
        printf("Error: Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    switch (choice) {
        case 1:
            sortByID(students, numStudents);
            break;
        case 2:
            sortByName(students, numStudents);
            break;
        case 3:
            sortByAverage(students, numStudents);
            break;
        case 4:
            sortByProgramme(students, numStudents);
            break;
        default:
            printf("Invalid sort option.\n");
            break;
    }
}

/* ============================================================================
 *                        ACADEMIC REPORT
 * ============================================================================ */

/*
 * generateAcademicReport
 * ----------------------
 * Generates a comprehensive academic report including:
 *   - Total registered students
 *   - Total courses across all students
 *   - Overall average academic performance
 *   - Student with highest/lowest average
 *   - Number passing/failing
 *   - Average performance per course
 *   - Number of students per programme
 */
void generateAcademicReport(Student *students, int numStudents) {
    int i, j, k;
    int totalCourses = 0;
    int totalPassing = 0, totalFailing = 0;
    float overallSum = 0.0f;
    float highestAvg = -1.0f, lowestAvg = 101.0f;
    int highestIdx = 0, lowestIdx = 0;
    float avg;

    /* For per-course averages: track unique course codes */
    char courseNames[500][MAX_CODE_LEN];
    float courseSums[500];
    int courseCounts[500];
    int uniqueCourses = 0;

    /* For per-programme counts */
    char progNames[100][MAX_PROGRAMME_LEN];
    int progCounts[100];
    int uniqueProgs = 0;

    printf("\n");
    printf("============================================================\n");
    printf("           COMPREHENSIVE ACADEMIC REPORT\n");
    printf("============================================================\n");

    if (numStudents == 0) {
        printf("No students registered. Report cannot be generated.\n");
        return;
    }

    /* Initialize arrays */
    memset(courseSums, 0, sizeof(courseSums));
    memset(courseCounts, 0, sizeof(courseCounts));
    memset(progCounts, 0, sizeof(progCounts));

    /* Compute statistics */
    for (i = 0; i < numStudents; i++) {
        Student *s = students + i;
        int studentPassing = 1; /* Assume passing until a course fails */

        totalCourses += s->numCourses;

        /* Calculate student average */
        avg = calculateStudentAverage(s);
        overallSum += avg;

        if (avg > highestAvg) {
            highestAvg = avg;
            highestIdx = i;
        }
        if (avg < lowestAvg) {
            lowestAvg = avg;
            lowestIdx = i;
        }

        /* Check pass/fail per course for this student */
        for (j = 0; j < s->numCourses; j++) {
            CourseRecord *c = s->courses + j;
            float pct = (c->maxMarks > 0) ? (c->marksObtained / c->maxMarks) * 100.0f : 0.0f;

            if (pct < PASS_THRESHOLD) {
                studentPassing = 0;
            }

            /* Track per-course average */
            {
                int found = 0;
                for (k = 0; k < uniqueCourses; k++) {
                    if (strcmp(courseNames[k], c->courseCode) == 0) {
                        courseSums[k] += pct;
                        courseCounts[k]++;
                        found = 1;
                        break;
                    }
                }
                if (!found && uniqueCourses < 500) {
                    strncpy(courseNames[uniqueCourses], c->courseCode, MAX_CODE_LEN - 1);
                    courseNames[uniqueCourses][MAX_CODE_LEN - 1] = '\0';
                    courseSums[uniqueCourses] = pct;
                    courseCounts[uniqueCourses] = 1;
                    uniqueCourses++;
                }
            }
        }

        if (studentPassing) {
            totalPassing++;
        } else {
            totalFailing++;
        }

        /* Track per-programme counts */
        {
            int found = 0;
            for (k = 0; k < uniqueProgs; k++) {
                if (strcmp(progNames[k], s->programme) == 0) {
                    progCounts[k]++;
                    found = 1;
                    break;
                }
            }
            if (!found && uniqueProgs < 100) {
                strncpy(progNames[uniqueProgs], s->programme, MAX_PROGRAMME_LEN - 1);
                progNames[uniqueProgs][MAX_PROGRAMME_LEN - 1] = '\0';
                progCounts[uniqueProgs] = 1;
                uniqueProgs++;
            }
        }
    }

    /* ---- Print Report ---- */
    printf("\n1. GENERAL STATISTICS\n");
    printf("   Total Registered Students : %d\n", numStudents);
    printf("   Total Courses (all students): %d\n", totalCourses);
    printf("   Overall Average Performance : %.2f%%\n",
           (numStudents > 0) ? overallSum / numStudents : 0.0f);

    printf("\n2. TOP / BOTTOM PERFORMERS\n");
    printf("   Highest Average: %s %s (ID: %d) — %.2f%%\n",
           (students + highestIdx)->firstName,
           (students + highestIdx)->lastName,
           (students + highestIdx)->studentID,
           highestAvg);
    printf("   Lowest Average : %s %s (ID: %d) — %.2f%%\n",
           (students + lowestIdx)->firstName,
           (students + lowestIdx)->lastName,
           (students + lowestIdx)->studentID,
           lowestAvg);

    printf("\n3. PASS / FAIL SUMMARY\n");
    printf("   Students Passing (all courses >= 50%%): %d\n", totalPassing);
    printf("   Students Failing (at least one < 50%%): %d\n", totalFailing);

    printf("\n4. AVERAGE PERFORMANCE PER COURSE\n");
    if (uniqueCourses == 0) {
        printf("   No courses recorded.\n");
    } else {
        printf("   %-15s %10s %10s\n", "Course Code", "Avg (%)", "Students");
        for (i = 0; i < 40; i++) printf("-");
        printf("\n");
        for (i = 0; i < uniqueCourses; i++) {
            printf("   %-15s %9.2f%% %10d\n",
                   courseNames[i],
                   (courseCounts[i] > 0) ? courseSums[i] / courseCounts[i] : 0.0f,
                   courseCounts[i]);
        }
    }

    printf("\n5. STUDENTS PER PROGRAMME\n");
    if (uniqueProgs == 0) {
        printf("   No programmes recorded.\n");
    } else {
        printf("   %-40s %10s\n", "Programme", "Students");
        for (i = 0; i < 55; i++) printf("-");
        printf("\n");
        for (i = 0; i < uniqueProgs; i++) {
            printf("   %-40s %10d\n", progNames[i], progCounts[i]);
        }
    }

    printf("\n============================================================\n");
    printf("                   END OF REPORT\n");
    printf("============================================================\n");
}

/* ============================================================================
 *                          FILE HANDLING
 * ============================================================================ */

/*
 * saveRecordsToFile
 * -----------------
 * Saves all student and course records to a text file (students_data.txt).
 * Format: structured plain text with delimiters for parsing.
 */
void saveRecordsToFile(Student *students, int numStudents) {
    int i, j;
    FILE *fp;

    printf("\n--- Save Records to File ---\n");

    fp = fopen(DATA_FILE, "w");
    if (fp == NULL) {
