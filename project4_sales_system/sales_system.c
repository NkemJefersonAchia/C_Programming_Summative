/*
 * ============================================================================
 * Project 4: Sales Transaction Processing and Analysis System
 * ============================================================================
 * File:        sales_system.c
 * Description: A comprehensive sales transaction management system in C that
 *              demonstrates dynamic memory allocation, function pointers,
 *              callback functions, manual searching/sorting, file I/O, and
 *              modular program design.
 *
 * Key Concepts Demonstrated:
 *   - Function Pointers (SalesProcessor): Used in menu option 5 to select
 *     and invoke data processing functions at runtime via a function pointer
 *     array.
 *   - Callback Functions (TransactionFilter): Used in menu option 6 to pass
 *     filter functions as arguments to applyFilter(), enabling flexible
 *     and reusable filtering logic.
 *   - Comparison Callbacks (CompareFunc): Used in sorting to decouple the
 *     sort algorithm from the comparison logic.
 *   - Dynamic Memory: malloc/realloc/free with capacity tracking.
 *
 * Author:      Nkem Jeferson Achia
 * Date:        2026-11-21
 * Course:      C Programming - Trimester 6
 * ============================================================================
 */

#include "sales_system.h"

/* ============================================================================
 * Utility Functions
 * ============================================================================ */

/**
 * clearInputBuffer - Discards remaining characters in stdin up to newline.
 * Prevents leftover characters from affecting subsequent reads.
 */
void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/**
 * toLowerStr - Copies src to dest, converting all characters to lowercase.
 * Used for case-insensitive string comparisons.
 *
 * @param dest  Destination buffer (must be large enough)
 * @param src   Source string
 */
void toLowerStr(char *dest, const char *src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = (char)tolower((unsigned char)src[i]);
        i++;
    }
    dest[i] = '\0';
}

/**
 * getValidInt - Prompts the user and reads a valid integer.
 *
 * @param prompt  The prompt message to display
 * @param value   Pointer to store the validated integer
 * @return        1 on success, 0 on invalid input
 */
int getValidInt(const char *prompt, int *value) {
    printf("%s", prompt);
    if (scanf("%d", value) != 1) {
        clearInputBuffer();
        printf("[Error] Invalid input. Please enter a valid integer.\n");
        return 0;
    }
    clearInputBuffer();
    return 1;
}

/**
 * getValidFloat - Prompts the user and reads a valid float.
 *
 * @param prompt  The prompt message to display
 * @param value   Pointer to store the validated float
 * @return        1 on success, 0 on invalid input
 */
int getValidFloat(const char *prompt, float *value) {
    printf("%s", prompt);
    if (scanf("%f", value) != 1) {
        clearInputBuffer();
        printf("[Error] Invalid input. Please enter a valid number.\n");
        return 0;
    }
    clearInputBuffer();
    return 1;
}

/**
 * displayMenu - Prints the main menu options to stdout.
 */
void displayMenu(void) {
    printf("\n===== SALES TRANSACTION MANAGEMENT SYSTEM =====\n");
    printf("1.  Add New Transaction\n");
    printf("2.  Display All Transactions\n");
    printf("3.  Update Transaction\n");
    printf("4.  Delete Transaction\n");
    printf("5.  Process Sales Data\n");
    printf("6.  Filter Transactions (Callbacks)\n");
    printf("7.  Search Transactions\n");
    printf("8.  Sort Transactions\n");
    printf("9.  Generate Sales Report\n");
    printf("10. Save Transactions\n");
    printf("11. Load Transactions\n");
    printf("0.  Exit\n");
    printf("================================================\n");
}

/* ============================================================================
 * Dynamic Memory Management
 * ============================================================================ */

/**
 * initTransactions - Allocates initial memory for the transaction array
 * using malloc. Sets capacity to INITIAL_CAPACITY and count to 0.
 *
 * @param transactions  Pointer to the transaction array pointer
 * @param capacity      Pointer to store allocated capacity
 * @param count         Pointer to store transaction count (set to 0)
 * @return              1 on success, 0 on allocation failure
 */
int initTransactions(Transaction **transactions, int *capacity, int *count) {
    *capacity = INITIAL_CAPACITY;
    *count = 0;

    /* Allocate initial array using malloc */
    *transactions = (Transaction *)malloc(sizeof(Transaction) * (*capacity));

    if (*transactions == NULL) {
        printf("[Error] Memory allocation failed! Cannot initialize transactions.\n");
        return 0;
    }

    printf("[Info] Transaction system initialized (capacity: %d).\n", *capacity);
    return 1;
}

/**
 * ensureCapacity - Checks if the array is full and doubles capacity using
 * realloc when needed. Preserves existing data.
 *
 * @param transactions  Pointer to the transaction array pointer
 * @param capacity      Pointer to current capacity (updated if resized)
 * @param count         Current number of transactions
 * @return              1 on success, 0 on allocation failure
 */
int ensureCapacity(Transaction **transactions, int *capacity, int count) {
    if (count < *capacity) {
        return 1;  /* Still have room */
    }

    /* Double the capacity */
    int newCapacity = (*capacity) * 2;
    Transaction *temp = (Transaction *)realloc(*transactions,
                                                sizeof(Transaction) * newCapacity);

    if (temp == NULL) {
        printf("[Error] Memory reallocation failed! Cannot expand array.\n");
        return 0;
    }

    *transactions = temp;
    *capacity = newCapacity;
    printf("[Info] Array capacity expanded to %d.\n", newCapacity);
    return 1;
}

/**
 * freeTransactions - Frees the dynamically allocated transaction array
 * and resets count and capacity to 0.
 *
 * @param transactions  Pointer to the transaction array pointer
 * @param count         Pointer to count (reset to 0)
 * @param capacity      Pointer to capacity (reset to 0)
 */
void freeTransactions(Transaction **transactions, int *count, int *capacity) {
    if (*transactions != NULL) {
        free(*transactions);
        *transactions = NULL;
    }
    *count = 0;
    *capacity = 0;
}

/* ============================================================================
 * CRUD Operations
 * ============================================================================ */

/**
 * displayTransaction - Displays a single transaction in a formatted row.
 *
 * @param t  Pointer to the transaction to display
 */
void displayTransaction(const Transaction *t) {
    printf("| %-5d | %-20s | %-8d | $%-9.2f | $%-11.2f | %-10s |\n",
           t->transactionID, t->productName, t->quantitySold,
           t->unitPrice, t->totalAmount, t->transactionDate);
}

/**
 * addTransaction - Prompts user for transaction details and adds a new
 * transaction to the array. Validates:
 *   - Unique transaction ID (no duplicates)
 *   - Positive quantity sold
 *   - Positive unit price
 * Automatically calculates totalAmount = quantitySold * unitPrice.
 *
 * @param transactions  Pointer to the transaction array pointer
 * @param count         Pointer to current transaction count
 * @param capacity      Pointer to current array capacity
 */
void addTransaction(Transaction **transactions, int *count, int *capacity) {
    Transaction newTrans;

    printf("\n--- Add New Transaction ---\n");

    /* Get and validate Transaction ID */
    if (!getValidInt("Enter Transaction ID: ", &newTrans.transactionID)) return;

    if (newTrans.transactionID <= 0) {
        printf("[Error] Transaction ID must be a positive integer.\n");
        return;
    }

    /* Check for duplicate ID */
    if (searchByID(*transactions, *count, newTrans.transactionID) != -1) {
        printf("[Error] Transaction ID %d already exists. Duplicate IDs not allowed.\n",
               newTrans.transactionID);
        return;
    }

    /* Get product name */
    printf("Enter Product Name: ");
    if (fgets(newTrans.productName, MAX_PRODUCT_NAME, stdin) == NULL) {
        printf("[Error] Failed to read product name.\n");
        return;
    }
    /* Remove trailing newline */
    newTrans.productName[strcspn(newTrans.productName, "\n")] = '\0';

    if (strlen(newTrans.productName) == 0) {
        printf("[Error] Product name cannot be empty.\n");
        return;
    }

    /* Get and validate quantity */
    if (!getValidInt("Enter Quantity Sold: ", &newTrans.quantitySold)) return;

    if (newTrans.quantitySold <= 0) {
        printf("[Error] Quantity sold must be a positive integer.\n");
        return;
    }

