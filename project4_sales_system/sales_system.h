/*
 * ============================================================================
 * Project 4: Sales Transaction Processing and Analysis System
 * ============================================================================
 * File:        sales_system.h
 * Description: Header file containing struct definitions, function pointer
 *              typedefs, constants, and function prototypes for the Sales
 *              Transaction Processing and Analysis System.
 * Author:      Nkem Jeferson Achia
 * Date:        2026-11-21
 * Course:      C Programming - Trimester 6
 * ============================================================================
 */

#ifndef SALES_SYSTEM_H
#define SALES_SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ============================================================================
 * Constants
 * ============================================================================ */

#define INITIAL_CAPACITY  10       /* Initial size of the transaction array     */
#define MAX_PRODUCT_NAME  100      /* Maximum length of product name            */
#define MAX_DATE_LEN      20       /* Maximum length of date string (YYYY-MM-DD)*/
#define DATA_FILE         "transactions.txt"  /* Default save/load filename     */

/* ============================================================================
 * Structure Definitions
 * ============================================================================ */

/**
 * Transaction - Represents a single sales transaction
 *
 * Fields:
 *   transactionID   - Unique identifier for the transaction
 *   productName     - Name of the product sold
 *   quantitySold    - Number of units sold (must be positive)
 *   unitPrice       - Price per unit (must be positive)
 *   totalAmount     - Calculated as quantitySold * unitPrice
 *   transactionDate - Date of transaction in "YYYY-MM-DD" format
 */
typedef struct {
    int transactionID;
    char productName[MAX_PRODUCT_NAME];
    int quantitySold;
    float unitPrice;
    float totalAmount;              /* quantitySold * unitPrice */
    char transactionDate[MAX_DATE_LEN]; /* "YYYY-MM-DD" format */
} Transaction;

/* ============================================================================
 * Function Pointer Type Definitions
