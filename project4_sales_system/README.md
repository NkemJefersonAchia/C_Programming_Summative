# Project 4: Sales Transaction Processing and Analysis System

## Project Description

A comprehensive **Sales Transaction Management System** written in C that demonstrates advanced programming concepts including **function pointers**, **callback functions**, dynamic memory management, file I/O, and modular program design. The system allows users to manage sales transactions through a full-featured interactive menu with CRUD operations, data processing, filtering, searching, sorting, and report generation.

## Key Concepts Demonstrated

### Function Pointers (`SalesProcessor`)

Function pointers allow storing the address of a function in a variable and calling it indirectly. This program defines:

```c
typedef float (*SalesProcessor)(Transaction *transactions, int count);
```

Five processing functions match this signature:
- `calculateTotalRevenue()` — sum of all transaction amounts
- `calculateAverageValue()` — average transaction amount
- `findHighestTransaction()` — highest transaction amount
- `findLowestTransaction()` — lowest transaction amount
- `calculateTotalUnitsSold()` — total units sold

In **Menu Option 5 (Process Sales Data)**, these functions are stored in an **array of function pointers**. The user selects an operation, and the corresponding function is assigned to a `SalesProcessor` variable and invoked — demonstrating runtime function selection via function pointers.

### Callback Functions (`TransactionFilter`)

Callback functions are passed as arguments to other functions, enabling flexible and reusable code. This program defines:

```c
typedef int (*TransactionFilter)(Transaction *t, void *criteria);
```

Callback implementations:
- `filterByMinAmount()` — returns 1 if transaction amount ≥ threshold
- `filterByProduct()` — returns 1 if product name matches

The `applyFilter()` function accepts a `TransactionFilter` callback and applies it to every transaction — the caller decides *what* to filter by, while `applyFilter()` handles *how* to iterate.

### Comparison Callbacks (`CompareFunc`)

```c
typedef int (*CompareFunc)(const Transaction *a, const Transaction *b);
```

The `bubbleSort()` function accepts a comparison callback, decoupling the sort algorithm from the comparison logic. Four comparison functions enable sorting by ID, product name, or amount (ascending/descending).

### Dynamic Memory Management

- `malloc()` for initial allocation of the transaction array
- `realloc()` to double capacity when the array is full
- `free()` to release all allocated memory before exit
- Allocation failure handling at every allocation point

## Features

| Feature | Description |
|---------|-------------|
| Add Transaction | Add with duplicate ID prevention, positive value validation |
| Display All | Formatted table display of all transactions |
| Update | Modify existing transaction fields by ID |
| Delete | Remove transaction with confirmation prompt |
| Process Data | Function pointer-based data processing (5 operations) |
| Filter | Callback-based filtering by amount threshold or product |
| Search | Manual search by ID, product name (case-insensitive), or amount range |
| Sort | Manual bubble sort with comparison callbacks (4 criteria) |
| Sales Report | Comprehensive report with per-product revenue breakdown |
| Save/Load | Persistent file storage in pipe-delimited text format |

## Compilation

```bash
gcc -o sales_system sales_system.c -Wall -Wextra
```

Or with debug symbols:

```bash
gcc -o sales_system sales_system.c -Wall -Wextra -g
```

## Usage

### Running the Program

```bash
./sales_system
```

### Main Menu

```
===== SALES TRANSACTION MANAGEMENT SYSTEM =====
1.  Add New Transaction
2.  Display All Transactions
3.  Update Transaction
4.  Delete Transaction
5.  Process Sales Data
6.  Filter Transactions (Callbacks)
7.  Search Transactions
8.  Sort Transactions
9.  Generate Sales Report
10. Save Transactions
11. Load Transactions
0.  Exit
================================================
```

### Example Workflow

1. **Add transactions** (Option 1) — enter ID, product name, quantity, unit price, and date
2. **Display all** (Option 2) — view transactions in a formatted table
3. **Process data** (Option 5) — select a processing operation (demonstrates function pointers)
4. **Filter** (Option 6) — filter by minimum amount or product (demonstrates callbacks)
5. **Search** (Option 7) — find transactions by ID, name, or amount range
6. **Sort** (Option 8) — sort by various criteria using comparison callbacks
7. **Generate report** (Option 9) — view comprehensive sales analysis
8. **Save** (Option 10) — persist data to `transactions.txt`

### File Format

Transactions are saved in pipe-delimited format:

```
# Sales Transactions Data File
# Format: ID|ProductName|Quantity|UnitPrice|TotalAmount|Date
1001|Laptop|5|999.99|4999.95|2026-01-15
1002|Mouse|50|25.99|1299.50|2026-01-16
```

## File Structure

```
project4_sales_system/
├── sales_system.h      # Header: structs, typedefs, function prototypes
├── sales_system.c      # Main implementation with all functions
├── README.md           # This file
└── transactions.txt    # Auto-generated data file (created on save)
```

## Input Validation

- Duplicate transaction IDs are rejected
- Negative quantities and prices are rejected
- Empty product names and dates are rejected
- Invalid menu choices display error messages
- Division by zero is handled (e.g., average with 0 transactions)
- File I/O errors are reported gracefully

## Author

**Nkem Jeferson Achia**  
C Programming — Trimester 6, 2026
