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
 * ============================================================================ */

/**
 * SalesProcessor - Function pointer type for sales data processing operations.
 *
 * Functions matching this signature take the transaction array and count,
 * and return a float result (e.g., total revenue, average value, etc.).
 *
 * Usage: Assigned at runtime based on user menu selection (menu option 5).
 */
typedef float (*SalesProcessor)(Transaction *transactions, int count);

/**
 * TransactionFilter - Callback function pointer type for filtering transactions.
 *
 * Functions matching this signature evaluate a single transaction against
 * criteria and return 1 (match) or 0 (no match).
 *
 * Parameters:
 *   t        - Pointer to the transaction to evaluate
 *   criteria - Generic pointer to filter criteria (cast inside callback)
 *
 * Usage: Passed to applyFilter() to filter transactions by various criteria.
 */
typedef int (*TransactionFilter)(Transaction *t, void *criteria);

/**
 * CompareFunc - Comparison callback function pointer for sorting.
 *
 * Functions matching this signature compare two transactions and return:
 *   negative if a < b, zero if equal, positive if a > b
 */
typedef int (*CompareFunc)(const Transaction *a, const Transaction *b);

/* ============================================================================
