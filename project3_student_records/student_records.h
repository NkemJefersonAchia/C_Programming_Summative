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
