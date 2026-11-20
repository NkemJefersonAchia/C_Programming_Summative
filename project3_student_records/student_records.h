/*
 * ============================================================================
 * Project:     Student Academic Records and Performance Analytics System
 * File:        student_records.h
 * Description: Header file containing structure definitions, constants,
 *              macros, and function prototypes for the Student Academic
 *              Records and Performance Analytics System.
 * Author:      Nkem Jeferson Achia
 * Course:      C Programming
 * Date:        2026
 * ============================================================================
 */

#ifndef STUDENT_RECORDS_H
#define STUDENT_RECORDS_H

/* ========================== INCLUDES ========================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ========================== CONSTANTS ========================== */
#define MAX_CODE_LEN      20    /* Maximum length of a course code           */
#define MAX_COURSE_NAME   100   /* Maximum length of a course name           */
#define MAX_NAME_LEN      50    /* Maximum length of first/last name         */
#define MAX_PROGRAMME_LEN 100   /* Maximum length of programme name          */
#define DATA_FILE         "students_data.txt"  /* Default data file name     */
#define PASS_THRESHOLD    50.0  /* Pass mark percentage threshold            */
#define MIN_YEAR          1     /* Minimum year of study                     */
#define MAX_YEAR          7     /* Maximum year of study                     */

/* ========================== STRUCTURES ========================== */

/*
 * CourseRecord
 * -----------
 * Represents a single course taken by a student, including the course
 * code, name, marks obtained, and maximum possible marks.
 */
typedef struct {
    char courseCode[MAX_CODE_LEN];
    char courseName[MAX_COURSE_NAME];
    float marksObtained;
    float maxMarks;
} CourseRecord;

/*
 * Student
 * -------
 * Represents a student record containing personal information,
 * programme of study, and a dynamically allocated array of courses.
 */
typedef struct {
    int studentID;
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    char programme[MAX_PROGRAMME_LEN];
    int yearOfStudy;
    int numCourses;
    CourseRecord *courses;  /* dynamically allocated array of courses */
} Student;

/* ========================== FUNCTION PROTOTYPES ========================== */

/* --- Menu and UI --- */
void displayMenu(void);

/* --- Student CRUD Operations --- */
void registerStudent(Student **students, int *numStudents);
void displayAllStudents(Student *students, int numStudents);
void viewStudentDetails(Student *students, int numStudents);
void updateStudentInfo(Student *students, int numStudents);
void deleteStudent(Student **students, int *numStudents);

/* --- Course Operations --- */
void addCourseToStudent(Student *students, int numStudents);
void updateCourseMarks(Student *students, int numStudents);
void displayAcademicPerformance(Student *students, int numStudents);

/* --- Searching --- */
void searchStudents(Student *students, int numStudents);
int searchByID(Student *students, int numStudents, int targetID);
void searchByName(Student *students, int numStudents, const char *name);
void searchByProgramme(Student *students, int numStudents, const char *programme);

/* --- Sorting --- */
void sortStudents(Student *students, int numStudents);
void sortByID(Student *students, int numStudents);
void sortByName(Student *students, int numStudents);
void sortByAverage(Student *students, int numStudents);
void sortByProgramme(Student *students, int numStudents);

/* --- Academic Report --- */
void generateAcademicReport(Student *students, int numStudents);

/* --- File Handling --- */
void saveRecordsToFile(Student *students, int numStudents);
void loadRecordsFromFile(Student **students, int *numStudents);

/* --- Utility Functions --- */
float calculateStudentAverage(Student *student);
void clearInputBuffer(void);
int isDuplicateID(Student *students, int numStudents, int id);
void strToLower(char *dest, const char *src);
void freeAllMemory(Student *students, int numStudents);
void displayStudentSummary(Student *student);
void displayStudentFull(Student *student);

#endif /* STUDENT_RECORDS_H */
