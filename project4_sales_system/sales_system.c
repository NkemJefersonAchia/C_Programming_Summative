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

    /* Get and validate unit price */
    if (!getValidFloat("Enter Unit Price: ", &newTrans.unitPrice)) return;

    if (newTrans.unitPrice <= 0.0f) {
        printf("[Error] Unit price must be a positive value.\n");
        return;
    }

    /* Calculate total amount */
    newTrans.totalAmount = (float)newTrans.quantitySold * newTrans.unitPrice;

    /* Get transaction date */
    printf("Enter Transaction Date (YYYY-MM-DD): ");
    if (fgets(newTrans.transactionDate, MAX_DATE_LEN, stdin) == NULL) {
        printf("[Error] Failed to read date.\n");
        return;
    }
    newTrans.transactionDate[strcspn(newTrans.transactionDate, "\n")] = '\0';

    if (strlen(newTrans.transactionDate) == 0) {
        printf("[Error] Transaction date cannot be empty.\n");
        return;
    }

    /* Ensure array has capacity */
    if (!ensureCapacity(transactions, capacity, *count)) {
        printf("[Error] Cannot add transaction — memory allocation failed.\n");
        return;
    }

    /* Add the transaction to the array */
    (*transactions)[*count] = newTrans;
    (*count)++;

    printf("[Success] Transaction ID %d added. Total Amount: $%.2f\n",
           newTrans.transactionID, newTrans.totalAmount);
}

/**
 * displayAllTransactions - Displays all transactions in a formatted table.
 * Shows a message if no transactions exist.
 *
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 */
void displayAllTransactions(Transaction *transactions, int count) {
    if (count == 0) {
        printf("\n[Info] No transactions to display.\n");
        return;
    }

    printf("\n--- All Transactions (%d) ---\n", count);
    printf("+-------+----------------------+----------+------------+--------------+------------+\n");
    printf("| ID    | Product              | Quantity | Unit Price | Total Amount | Date       |\n");
    printf("+-------+----------------------+----------+------------+--------------+------------+\n");

    for (int i = 0; i < count; i++) {
        displayTransaction(&transactions[i]);
    }

    printf("+-------+----------------------+----------+------------+--------------+------------+\n");
}

/**
 * updateTransaction - Finds a transaction by ID and allows the user to
 * update its fields. Recalculates totalAmount after changes.
 *
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 */
void updateTransaction(Transaction *transactions, int count) {
    int id, index;

    if (count == 0) {
        printf("\n[Info] No transactions to update.\n");
        return;
    }

    printf("\n--- Update Transaction ---\n");

    if (!getValidInt("Enter Transaction ID to update: ", &id)) return;

    index = searchByID(transactions, count, id);
    if (index == -1) {
        printf("[Error] Transaction ID %d not found.\n", id);
        return;
    }

    printf("Current details:\n");
    displayTransaction(&transactions[index]);

    /* Update product name */
    printf("\nEnter new Product Name (or press Enter to keep '%s'): ",
           transactions[index].productName);
    char buffer[MAX_PRODUCT_NAME];
    if (fgets(buffer, MAX_PRODUCT_NAME, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) > 0) {
            strcpy(transactions[index].productName, buffer);
        }
    }

    /* Update quantity */
    printf("Enter new Quantity Sold (or 0 to keep %d): ",
           transactions[index].quantitySold);
    int newQty;
    if (scanf("%d", &newQty) == 1 && newQty > 0) {
        transactions[index].quantitySold = newQty;
    } else if (newQty < 0) {
        printf("[Warning] Negative quantity ignored.\n");
    }
    clearInputBuffer();

    /* Update unit price */
    printf("Enter new Unit Price (or 0 to keep %.2f): ",
           transactions[index].unitPrice);
    float newPrice;
    if (scanf("%f", &newPrice) == 1 && newPrice > 0.0f) {
        transactions[index].unitPrice = newPrice;
    } else if (newPrice < 0.0f) {
        printf("[Warning] Negative price ignored.\n");
    }
    clearInputBuffer();

    /* Update date */
    printf("Enter new Date (YYYY-MM-DD) (or press Enter to keep '%s'): ",
           transactions[index].transactionDate);
    if (fgets(buffer, MAX_DATE_LEN, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) > 0) {
            strcpy(transactions[index].transactionDate, buffer);
        }
    }

    /* Recalculate total amount */
    transactions[index].totalAmount =
        (float)transactions[index].quantitySold * transactions[index].unitPrice;

    printf("[Success] Transaction ID %d updated. New Total: $%.2f\n",
           id, transactions[index].totalAmount);
}

/**
 * deleteTransaction - Deletes a transaction by ID. Shifts remaining
 * elements left to fill the gap. Decrements count.
 *
 * @param transactions  Array of transactions
 * @param count         Pointer to current transaction count (decremented)
 */
void deleteTransaction(Transaction *transactions, int *count) {
    int id, index;

    if (*count == 0) {
        printf("\n[Info] No transactions to delete.\n");
        return;
    }

    printf("\n--- Delete Transaction ---\n");

    if (!getValidInt("Enter Transaction ID to delete: ", &id)) return;

    index = searchByID(transactions, *count, id);
    if (index == -1) {
        printf("[Error] Transaction ID %d not found.\n", id);
        return;
    }

    /* Show the transaction being deleted */
    printf("Deleting transaction:\n");
    displayTransaction(&transactions[index]);

    /* Confirm deletion */
    printf("Are you sure? (y/n): ");
    char confirm;
    scanf(" %c", &confirm);
    clearInputBuffer();

    if (confirm != 'y' && confirm != 'Y') {
        printf("[Info] Deletion cancelled.\n");
        return;
    }

    /* Shift elements left to fill the gap */
    for (int i = index; i < *count - 1; i++) {
        transactions[i] = transactions[i + 1];
    }

    (*count)--;
    printf("[Success] Transaction ID %d deleted. Remaining: %d transactions.\n",
           id, *count);
}

/* ============================================================================
 * Sales Processing Functions (Function Pointer Demonstration)
 *
 * Each function below matches the SalesProcessor signature:
 *   float function_name(Transaction *transactions, int count);
 *
 * These are stored in an array of function pointers and selected at runtime
 * based on user choice — demonstrating the power of function pointers.
 * ============================================================================ */

/**
 * calculateTotalRevenue - Sums all totalAmount values across transactions.
 *
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 * @return              Total revenue (sum of all totalAmounts)
 */
float calculateTotalRevenue(Transaction *transactions, int count) {
    float total = 0.0f;
    for (int i = 0; i < count; i++) {
        total += transactions[i].totalAmount;
    }
    return total;
}

/**
 * calculateAverageValue - Computes the average totalAmount per transaction.
 * Handles division by zero by returning 0 when count is 0.
 *
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 * @return              Average totalAmount value
 */
float calculateAverageValue(Transaction *transactions, int count) {
    if (count == 0) return 0.0f;  /* Avoid division by zero */
    return calculateTotalRevenue(transactions, count) / (float)count;
}

/**
 * findHighestTransaction - Finds and returns the highest totalAmount.
 *
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 * @return              Highest totalAmount value (0 if no transactions)
 */
float findHighestTransaction(Transaction *transactions, int count) {
    if (count == 0) return 0.0f;

    float highest = transactions[0].totalAmount;
    for (int i = 1; i < count; i++) {
        if (transactions[i].totalAmount > highest) {
            highest = transactions[i].totalAmount;
        }
    }
    return highest;
}

/**
 * findLowestTransaction - Finds and returns the lowest totalAmount.
 *
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 * @return              Lowest totalAmount value (0 if no transactions)
 */
float findLowestTransaction(Transaction *transactions, int count) {
    if (count == 0) return 0.0f;

    float lowest = transactions[0].totalAmount;
    for (int i = 1; i < count; i++) {
        if (transactions[i].totalAmount < lowest) {
            lowest = transactions[i].totalAmount;
        }
    }
    return lowest;
}

/**
 * calculateTotalUnitsSold - Sums all quantitySold values.
 *
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 * @return              Total units sold (as float for SalesProcessor compatibility)
 */
float calculateTotalUnitsSold(Transaction *transactions, int count) {
    float totalUnits = 0.0f;
    for (int i = 0; i < count; i++) {
        totalUnits += (float)transactions[i].quantitySold;
    }
    return totalUnits;
}

/**
 * processSalesData - Menu option 5: Demonstrates function pointers.
 *
 * Creates an array of SalesProcessor function pointers, lets the user choose
 * an operation, assigns the corresponding function to a pointer variable,
 * and calls it. This is the core function pointer demonstration.
 *
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 */
void processSalesData(Transaction *transactions, int count) {
    if (count == 0) {
        printf("\n[Info] No transactions to process.\n");
        return;
    }

    /* ---- Array of function pointers ---- */
    /* Each element is a SalesProcessor (function pointer) pointing to one of
       our processing functions. The user's choice indexes into this array. */
    SalesProcessor processors[] = {
        calculateTotalRevenue,       /* Index 0 */
        calculateAverageValue,       /* Index 1 */
        findHighestTransaction,      /* Index 2 */
        findLowestTransaction,       /* Index 3 */
        calculateTotalUnitsSold      /* Index 4 */
    };

    /* Labels for display */
    const char *labels[] = {
        "Total Revenue",
        "Average Transaction Value",
        "Highest Transaction Amount",
        "Lowest Transaction Amount",
        "Total Units Sold"
    };

    int numProcessors = 5;

    printf("\n--- Process Sales Data (Function Pointers) ---\n");
    printf("Select a processing operation:\n");
    for (int i = 0; i < numProcessors; i++) {
        printf("  %d. %s\n", i + 1, labels[i]);
    }

    int choice;
    if (!getValidInt("Enter choice (1-%d): ", &choice)) return;

    if (choice < 1 || choice > numProcessors) {
        printf("[Error] Invalid choice. Please select 1-%d.\n", numProcessors);
        return;
    }

    /* ---- Function pointer assignment and invocation ---- */
    /* Assign the selected function to a SalesProcessor function pointer */
    SalesProcessor selectedProcessor = processors[choice - 1];

    /* Call the function through the function pointer */
    float result = selectedProcessor(transactions, count);

    printf("\n>> %s: $%.2f\n", labels[choice - 1], result);
}

/* ============================================================================
 * Callback / Filter Functions (Callback Demonstration)
 *
 * Each filter function matches the TransactionFilter signature:
 *   int function_name(Transaction *t, void *criteria);
 *
 * These are passed as callbacks to applyFilter(), which iterates over all
 * transactions and calls the callback for each one — demonstrating how
 * callback functions enable flexible, reusable code.
 * ============================================================================ */

/**
 * filterByMinAmount - Callback: returns 1 if the transaction's totalAmount
 * is greater than or equal to the threshold value.
 *
 * @param t         Pointer to the transaction to test
 * @param criteria  Pointer to a float threshold (cast from void*)
 * @return          1 if totalAmount >= threshold, 0 otherwise
 */
int filterByMinAmount(Transaction *t, void *criteria) {
    float threshold = *(float *)criteria;
    return (t->totalAmount >= threshold) ? 1 : 0;
}

/**
 * filterByProduct - Callback: returns 1 if the transaction's product name
 * matches the given product name (case-insensitive comparison).
 *
 * @param t         Pointer to the transaction to test
 * @param criteria  Pointer to a string (product name) (cast from void*)
 * @return          1 if product names match, 0 otherwise
 */
int filterByProduct(Transaction *t, void *criteria) {
    char *productName = (char *)criteria;

    /* Case-insensitive comparison */
    char lower1[MAX_PRODUCT_NAME], lower2[MAX_PRODUCT_NAME];
    toLowerStr(lower1, t->productName);
    toLowerStr(lower2, productName);

    return (strcmp(lower1, lower2) == 0) ? 1 : 0;
}

/**
 * countByCriteria - Counts how many transactions match a given filter callback.
 * Demonstrates passing a callback function as a parameter.
 *
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 * @param filter        Callback function to test each transaction
 * @param criteria      Generic criteria pointer passed to the callback
 * @return              Number of matching transactions
 */
int countByCriteria(Transaction *transactions, int count,
                    TransactionFilter filter, void *criteria) {
    int matches = 0;
    for (int i = 0; i < count; i++) {
        /* Call the callback function for each transaction */
        if (filter(&transactions[i], criteria)) {
            matches++;
        }
    }
    return matches;
}

/**
 * applyFilter - Applies a callback filter function to all transactions
 * and displays those that match. This is the core callback demonstration:
 * it accepts a function pointer (callback) as a parameter and invokes it
 * for each transaction.
 *
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 * @param filter        Callback function to test each transaction
 * @param criteria      Generic criteria pointer passed to the callback
 */
void applyFilter(Transaction *transactions, int count,
                 TransactionFilter filter, void *criteria) {
    int found = 0;

    printf("\n--- Filtered Results ---\n");
    printf("+-------+----------------------+----------+------------+--------------+------------+\n");
    printf("| ID    | Product              | Quantity | Unit Price | Total Amount | Date       |\n");
    printf("+-------+----------------------+----------+------------+--------------+------------+\n");

    for (int i = 0; i < count; i++) {
        /* Call the callback function — this is the callback in action */
        if (filter(&transactions[i], criteria)) {
            displayTransaction(&transactions[i]);
            found++;
        }
    }

    printf("+-------+----------------------+----------+------------+--------------+------------+\n");

    if (found == 0) {
        printf("[Info] No transactions matched the filter criteria.\n");
    } else {
        printf("[Info] %d transaction(s) matched.\n", found);
    }

    /* Also demonstrate countByCriteria using the same callback */
    int totalMatch = countByCriteria(transactions, count, filter, criteria);
    printf("[Info] countByCriteria() confirms: %d matching transaction(s).\n", totalMatch);
}

/**
 * filterTransactionsMenu - Menu option 6: Demonstrates callback functions.
 *
 * Lets the user choose a filter type (by minimum amount or by product name),
 * then passes the appropriate callback function to applyFilter().
 *
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 */
void filterTransactionsMenu(Transaction *transactions, int count) {
    if (count == 0) {
        printf("\n[Info] No transactions to filter.\n");
        return;
    }

    printf("\n--- Filter Transactions (Callbacks) ---\n");
    printf("1. Filter by Minimum Amount\n");
    printf("2. Filter by Product Name\n");

    int choice;
    if (!getValidInt("Enter choice: ", &choice)) return;

    switch (choice) {
        case 1: {
            float threshold;
            if (!getValidFloat("Enter minimum amount threshold: $", &threshold)) return;

            /* Pass filterByMinAmount as a CALLBACK to applyFilter */
            applyFilter(transactions, count, filterByMinAmount, &threshold);
            break;
        }
        case 2: {
            char productName[MAX_PRODUCT_NAME];
            printf("Enter product name to filter: ");
            if (fgets(productName, MAX_PRODUCT_NAME, stdin) == NULL) return;
            productName[strcspn(productName, "\n")] = '\0';

            if (strlen(productName) == 0) {
                printf("[Error] Product name cannot be empty.\n");
                return;
            }

            /* Pass filterByProduct as a CALLBACK to applyFilter */
            applyFilter(transactions, count, filterByProduct, productName);
            break;
        }
        default:
            printf("[Error] Invalid choice.\n");
    }
}

/* ============================================================================
 * Search Functions (Manual Implementation — NO bsearch)
 * ============================================================================ */

/**
 * searchByID - Linear search for a transaction by its unique ID.
 *
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 * @param id            Transaction ID to search for
 * @return              Index of the found transaction, or -1 if not found
 */
int searchByID(Transaction *transactions, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (transactions[i].transactionID == id) {
            return i;
        }
    }
    return -1;
}

/**
 * searchByProductName - Case-insensitive search by product name.
 * Displays all transactions whose product name contains the search term.
 *
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 * @param name          Product name to search for
 */
void searchByProductName(Transaction *transactions, int count, const char *name) {
    int found = 0;
    char lowerSearch[MAX_PRODUCT_NAME], lowerProduct[MAX_PRODUCT_NAME];
    toLowerStr(lowerSearch, name);

    printf("\n--- Search Results for '%s' ---\n", name);

    for (int i = 0; i < count; i++) {
        toLowerStr(lowerProduct, transactions[i].productName);
        /* Check if search term is contained in product name */
        if (strstr(lowerProduct, lowerSearch) != NULL) {
            displayTransaction(&transactions[i]);
            found++;
        }
    }

    if (found == 0) {
        printf("[Info] No transactions found for product '%s'.\n", name);
    } else {
        printf("[Info] Found %d transaction(s).\n", found);
    }
}

/**
 * searchByAmountRange - Searches for transactions whose totalAmount falls
 * within a specified range [minAmount, maxAmount].
 *
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 * @param minAmount     Minimum totalAmount (inclusive)
 * @param maxAmount     Maximum totalAmount (inclusive)
 */
void searchByAmountRange(Transaction *transactions, int count,
                         float minAmount, float maxAmount) {
    int found = 0;

    printf("\n--- Transactions with Amount $%.2f - $%.2f ---\n", minAmount, maxAmount);

    for (int i = 0; i < count; i++) {
        if (transactions[i].totalAmount >= minAmount &&
            transactions[i].totalAmount <= maxAmount) {
            displayTransaction(&transactions[i]);
            found++;
        }
    }

    if (found == 0) {
        printf("[Info] No transactions found in that range.\n");
    } else {
        printf("[Info] Found %d transaction(s) in range.\n", found);
    }
}

/**
 * searchTransactionsMenu - Menu option 7: Lets user choose a search method.
 *
 * @param transactions  Array of transactions
 * @param count         Number of transactions
 */
void searchTransactionsMenu(Transaction *transactions, int count) {
    if (count == 0) {
        printf("\n[Info] No transactions to search.\n");
        return;
    }

    printf("\n--- Search Transactions ---\n");
    printf("1. Search by Transaction ID\n");
    printf("2. Search by Product Name (case-insensitive)\n");
    printf("3. Search by Amount Range\n");

    int choice;
    if (!getValidInt("Enter choice: ", &choice)) return;

    switch (choice) {
        case 1: {
            int id;
            if (!getValidInt("Enter Transaction ID: ", &id)) return;
            int index = searchByID(transactions, count, id);
            if (index == -1) {
                printf("[Info] Transaction ID %d not found.\n", id);
            } else {
                printf("\n--- Transaction Found ---\n");
                displayTransaction(&transactions[index]);
            }
            break;
        }
        case 2: {
            char name[MAX_PRODUCT_NAME];
            printf("Enter Product Name: ");
            if (fgets(name, MAX_PRODUCT_NAME, stdin) == NULL) return;
            name[strcspn(name, "\n")] = '\0';
            if (strlen(name) == 0) {
                printf("[Error] Search term cannot be empty.\n");
                return;
            }
            searchByProductName(transactions, count, name);
            break;
        }
        case 3: {
            float minAmt, maxAmt;
            if (!getValidFloat("Enter minimum amount: $", &minAmt)) return;
            if (!getValidFloat("Enter maximum amount: $", &maxAmt)) return;
            if (minAmt > maxAmt) {
                printf("[Error] Minimum must be less than or equal to maximum.\n");
                return;
            }
            searchByAmountRange(transactions, count, minAmt, maxAmt);
            break;
        }
        default:
            printf("[Error] Invalid choice.\n");
    }
}

/* ============================================================================
 * Sorting Functions (Manual Implementation — NO qsort)
 * Uses comparison callbacks (CompareFunc) to decouple sort logic from
 * comparison logic.
 * ============================================================================ */

/**
 * compareByID - Comparison callback for sorting by Transaction ID.
 *
 * @param a  Pointer to first transaction
 * @param b  Pointer to second transaction
 * @return   Negative if a < b, zero if equal, positive if a > b
 */
int compareByID(const Transaction *a, const Transaction *b) {
    return a->transactionID - b->transactionID;
}

/**
 * compareByProductName - Comparison callback for sorting by product name.
 *
 * @param a  Pointer to first transaction
 * @param b  Pointer to second transaction
 * @return   Result of strcmp on product names
 */
int compareByProductName(const Transaction *a, const Transaction *b) {
    return strcmp(a->productName, b->productName);
}

/**
 * compareByAmountAsc - Comparison callback for sorting by totalAmount ascending.
 *
 * @param a  Pointer to first transaction
 * @param b  Pointer to second transaction
 * @return   Negative if a < b, zero if equal, positive if a > b
 */
int compareByAmountAsc(const Transaction *a, const Transaction *b) {
    if (a->totalAmount < b->totalAmount) return -1;
    if (a->totalAmount > b->totalAmount) return 1;
    return 0;
}

/**
 * compareByAmountDesc - Comparison callback for sorting by totalAmount descending.
 *
 * @param a  Pointer to first transaction
 * @param b  Pointer to second transaction
 * @return   Positive if a < b, zero if equal, negative if a > b
 */
int compareByAmountDesc(const Transaction *a, const Transaction *b) {
    if (a->totalAmount > b->totalAmount) return -1;
    if (a->totalAmount < b->totalAmount) return 1;
    return 0;
}

/**
 * bubbleSort - Manual bubble sort algorithm using a CompareFunc callback.
 *
 * The sort algorithm is decoupled from the comparison logic: the CompareFunc
 * callback determines the ordering, allowing the same sort function to sort
 * by different criteria simply by passing a different comparison function.
 *
 * @param transactions  Array of transactions to sort
 * @param count         Number of transactions
 * @param cmp           Comparison callback function pointer
 */
void bubbleSort(Transaction *transactions, int count, CompareFunc cmp) {
    for (int i = 0; i < count - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < count - i - 1; j++) {
            /* Use the comparison callback to decide ordering */
            if (cmp(&transactions[j], &transactions[j + 1]) > 0) {
                /* Swap transactions */
                Transaction temp = transactions[j];
                transactions[j] = transactions[j + 1];
                transactions[j + 1] = temp;
