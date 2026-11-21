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
 * Dynamic Memory Management Functions
 * ============================================================================ */

/**
 * initTransactions - Allocates initial memory for the transaction array.
 * @param transactions  Pointer to the transaction array pointer
 * @param capacity      Pointer to store the current capacity
 * @param count         Pointer to store the current count
 * @return              1 on success, 0 on failure
 */
int initTransactions(Transaction **transactions, int *capacity, int *count);

/**
 * ensureCapacity - Doubles the array capacity using realloc when full.
 * @param transactions  Pointer to the transaction array pointer
 * @param capacity      Pointer to the current capacity (updated on success)
 * @param count         Current number of transactions
 * @return              1 on success, 0 on allocation failure
 */
int ensureCapacity(Transaction **transactions, int *capacity, int count);

/**
 * freeTransactions - Frees the dynamically allocated transaction array.
 * @param transactions  Pointer to the transaction array pointer
 * @param count         Pointer to count (reset to 0)
 * @param capacity      Pointer to capacity (reset to 0)
 */
void freeTransactions(Transaction **transactions, int *count, int *capacity);

/* ============================================================================
 * CRUD Operations
 * ============================================================================ */

/**
 * addTransaction - Adds a new transaction with input validation.
 * Prevents duplicate IDs, rejects negative quantities/prices.
 */
void addTransaction(Transaction **transactions, int *count, int *capacity);

/**
 * displayAllTransactions - Displays all transactions in a formatted table.
 */
void displayAllTransactions(Transaction *transactions, int count);

/**
 * displayTransaction - Displays a single transaction's details.
 */
void displayTransaction(const Transaction *t);

/**
 * updateTransaction - Updates an existing transaction by ID.
 */
void updateTransaction(Transaction *transactions, int count);

/**
 * deleteTransaction - Deletes a transaction by ID, shifting remaining elements.
 */
void deleteTransaction(Transaction *transactions, int *count);

/* ============================================================================
 * Sales Processing Functions (match SalesProcessor signature)
 * ============================================================================ */

/**
 * calculateTotalRevenue - Computes the sum of all totalAmounts.
 * @return Sum of totalAmount for all transactions
 */
float calculateTotalRevenue(Transaction *transactions, int count);

/**
 * calculateAverageValue - Computes the average totalAmount.
 * @return Average totalAmount (handles division by zero)
 */
float calculateAverageValue(Transaction *transactions, int count);

/**
 * findHighestTransaction - Finds the highest totalAmount.
 * @return The highest totalAmount value
 */
float findHighestTransaction(Transaction *transactions, int count);

/**
 * findLowestTransaction - Finds the lowest totalAmount.
 * @return The lowest totalAmount value
 */
float findLowestTransaction(Transaction *transactions, int count);

/**
 * calculateTotalUnitsSold - Computes the sum of all quantities sold.
 * @return Total units sold (as float to match SalesProcessor signature)
 */
float calculateTotalUnitsSold(Transaction *transactions, int count);

/**
 * processSalesData - Menu option 5: lets user pick a processing operation,
 * assigns the matching function to a SalesProcessor pointer, and calls it.
 */
void processSalesData(Transaction *transactions, int count);

/* ============================================================================
 * Callback / Filter Functions (match TransactionFilter signature)
 * ============================================================================ */

/**
 * filterByMinAmount - Returns 1 if transaction totalAmount >= threshold.
 * @param t        Pointer to transaction
 * @param criteria Pointer to float threshold
 */
int filterByMinAmount(Transaction *t, void *criteria);

/**
 * filterByProduct - Returns 1 if transaction product name matches.
 * @param t        Pointer to transaction
 * @param criteria Pointer to string (product name)
 */
int filterByProduct(Transaction *t, void *criteria);

/**
 * countByCriteria - Counts transactions matching a filter callback.
 * @return Number of matching transactions
 */
int countByCriteria(Transaction *transactions, int count,
                    TransactionFilter filter, void *criteria);

/**
 * applyFilter - Applies a callback filter to all transactions and displays matches.
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 * @param filter        Callback function to test each transaction
 * @param criteria      Generic criteria pointer passed to callback
 */
void applyFilter(Transaction *transactions, int count,
                 TransactionFilter filter, void *criteria);

/**
 * filterTransactionsMenu - Menu option 6: lets user choose filter type,
 * then passes the appropriate callback to applyFilter.
 */
void filterTransactionsMenu(Transaction *transactions, int count);

/* ============================================================================
 * Search Functions (manual implementation, NO bsearch)
 * ============================================================================ */

/**
 * searchByID - Linear search for a transaction by its ID.
 * @return Index of the transaction, or -1 if not found
 */
int searchByID(Transaction *transactions, int count, int id);

/**
 * searchByProductName - Case-insensitive search by product name.
 */
void searchByProductName(Transaction *transactions, int count, const char *name);

/**
 * searchByAmountRange - Searches for transactions within a price range.
 */
void searchByAmountRange(Transaction *transactions, int count,
                         float minAmount, float maxAmount);

/**
 * searchTransactionsMenu - Menu option 7: lets user choose search criteria.
 */
void searchTransactionsMenu(Transaction *transactions, int count);

/* ============================================================================
 * Sorting Functions (manual implementation, NO qsort)
 * ============================================================================ */

/**
 * compareByID - Comparison callback: compares by transaction ID.
 */
int compareByID(const Transaction *a, const Transaction *b);

/**
 * compareByProductName - Comparison callback: compares by product name.
 */
int compareByProductName(const Transaction *a, const Transaction *b);

/**
 * compareByAmountAsc - Comparison callback: compares by totalAmount ascending.
 */
int compareByAmountAsc(const Transaction *a, const Transaction *b);

/**
 * compareByAmountDesc - Comparison callback: compares by totalAmount descending.
 */
int compareByAmountDesc(const Transaction *a, const Transaction *b);

/**
 * bubbleSort - Manual bubble sort using a CompareFunc callback.
 */
void bubbleSort(Transaction *transactions, int count, CompareFunc cmp);

/**
 * sortTransactionsMenu - Menu option 8: lets user choose sort criteria.
 */
void sortTransactionsMenu(Transaction *transactions, int count);

/* ============================================================================
 * Sales Report
 * ============================================================================ */

/**
 * generateSalesReport - Menu option 9: generates a comprehensive sales report
 * showing total transactions, total revenue, average value, highest/lowest
 * transaction, total quantity, and revenue by product.
 */
void generateSalesReport(Transaction *transactions, int count);

/* ============================================================================
 * File Handling
 * ============================================================================ */

/**
 * saveTransactions - Saves all transactions to a text file.
 * @return 1 on success, 0 on failure
 */
int saveTransactions(Transaction *transactions, int count, const char *filename);

/**
 * loadTransactions - Loads transactions from a text file.
 * @return Number of transactions loaded, or -1 on error
 */
int loadTransactions(Transaction **transactions, int *count, int *capacity,
                     const char *filename);

/* ============================================================================
 * Utility Functions
 * ============================================================================ */

/**
 * displayMenu - Prints the main menu.
 */
void displayMenu(void);

/**
 * clearInputBuffer - Clears remaining characters from stdin.
 */
void clearInputBuffer(void);

/**
 * toLowerStr - Converts a string to lowercase (for case-insensitive compare).
 */
void toLowerStr(char *dest, const char *src);

/**
 * getValidInt - Reads and validates an integer input.
 * @return 1 if valid input obtained, 0 otherwise
 */
int getValidInt(const char *prompt, int *value);

/**
 * getValidFloat - Reads and validates a float input.
 * @return 1 if valid input obtained, 0 otherwise
 */
int getValidFloat(const char *prompt, float *value);

#endif /* SALES_SYSTEM_H */
