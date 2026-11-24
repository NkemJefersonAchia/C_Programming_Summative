# C Programming Summative Assessment

A collection of five programming projects demonstrating proficiency in C programming, embedded systems, shell scripting, and systems programming.

## Repository Structure

```
C_Programming_Summative/
├── README.md                          # This file
├── project1_occupancy_monitor/        # Arduino Occupancy Monitoring System
│   ├── occupancy_monitor.ino
│   └── README.md
├── project2_linux_monitor/            # Linux System Health Monitor
│   ├── system_monitor.sh
│   └── README.md
├── project3_student_records/          # Student Academic Records System
│   ├── student_records.h
│   ├── student_records.c
│   └── README.md
├── project4_sales_system/             # Sales Transaction Processing System
│   ├── sales_system.h
│   ├── sales_system.c
│   └── README.md
└── project5_document_analyzer/        # Multi-threaded Document Analyzer
    ├── document_analyzer.h
    ├── document_analyzer.c
    ├── sample_document1.txt
    ├── sample_document2.txt
    ├── sample_document3.txt
    └── README.md
```

---

## Project Summaries

### Project 1: Smart Occupancy Monitoring and Access Control System

**Platform:** Arduino (Tinkercad Simulation)

An Arduino-based occupancy monitoring system that tracks the number of people inside a facility using entry and exit push buttons. The system enforces a maximum capacity limit and provides real-time feedback through a 16×2 I2C LCD display, green/red LED indicators, and Serial Monitor logging.

**Key Concepts Demonstrated:**
- Structures for organizing occupancy data
- Pointers for accessing and modifying occupancy records
- Dynamic memory allocation (`malloc()` / `free()`)
- Non-blocking programming with `millis()` (no `delay()`)
- I2C communication with LiquidCrystal_I2C (SDA on A4, SCL on A5)
- Button debouncing using `millis()`
- Periodic LCD updates without blocking
- Error handling for boundary conditions (full/empty facility)
- Serial communication for event logging

**Hardware Components:** Arduino Uno, 2 push buttons, 2 LEDs (green/red), 16×2 I2C LCD, resistors, breadboard, jumper wires.

📁 [View Project](project1_occupancy_monitor/)

---

### Project 2: Linux System Health and Resource Monitoring Manager

**Platform:** Linux / Bash Shell

An interactive shell application that monitors system health and resource utilization. It provides real-time information about CPU usage, memory utilization, disk space, running processes, and system uptime through a menu-driven interface.

**Key Features:**
- System information display (hostname, kernel, uptime)
- CPU utilization monitoring with configurable thresholds
- Memory utilization monitoring with percentage calculations
- Disk space monitoring for all mounted filesystems
- Process searching by name or PID
- Safe process termination with confirmation and system process protection
- System health report generation with timestamps
- Activity logging for all monitoring operations
- Comprehensive input validation and error handling

**How to Run:**
```bash
chmod +x project2_linux_monitor/system_monitor.sh
./project2_linux_monitor/system_monitor.sh
```

📁 [View Project](project2_linux_monitor/)

---

### Project 3: Student Academic Records and Performance Analytics System

**Language:** C

A menu-driven application for managing student academic records. The system supports full CRUD operations, course management, manual searching and sorting, academic performance analysis, and file-based data persistence.

**Key Concepts Demonstrated:**
- Structures (`Student`, `CourseRecord`) with nested dynamic arrays
- Pointers for all record access and manipulation
- Dynamic memory allocation (`malloc()`, `realloc()`, `calloc()`, `free()`)
- Manual searching (by ID, name, programme)
- Manual sorting (by ID, name, average, programme — no `qsort()`)
- Academic performance analytics (averages, pass/fail, per-course stats)
- File handling for data persistence (save/load)
- Input validation and comprehensive error handling

**How to Compile and Run:**
```bash
gcc -Wall -Wextra -o student_records project3_student_records/student_records.c
./student_records
```

📁 [View Project](project3_student_records/)

---

### Project 4: Sales Transaction Processing and Analysis System

**Language:** C

A menu-driven sales transaction management application that demonstrates the use of function pointers and callback functions for flexible runtime operation selection. Supports transaction CRUD, sales data processing, filtering, searching, sorting, reporting, and file persistence.

**Key Concepts Demonstrated:**
- **Function pointers** for runtime operation selection (revenue, average, min/max, units)
- **Callback functions** for filtering, searching, and comparison operations
- Structures for transaction records
- Dynamic memory allocation with `realloc()` for growing arrays
- Manual searching (by ID, product, amount)
- Manual sorting (by ID, product, amount — no `qsort()`)
- Sales report generation
- File handling for data persistence

**How to Compile and Run:**
```bash
gcc -Wall -Wextra -o sales_system project4_sales_system/sales_system.c
./sales_system
```

📁 [View Project](project4_sales_system/)

---

### Project 5: Multi-threaded Document Search and Analysis System

**Language:** C (POSIX Threads)

A multi-threaded document search and analysis system that processes multiple text files concurrently using POSIX threads. Each thread independently processes an assigned file, searches for a user-specified keyword (case-insensitive), and generates a detailed analysis report.

**Key Concepts Demonstrated:**
- POSIX threads (`pthread_create()`, `pthread_join()`)
- Concurrent file processing (one thread per file)
- Case-insensitive keyword searching
- Thread argument passing via structures
- Per-thread report generation
- Thread status monitoring and error handling
- Dynamic memory allocation for search results
- Command-line argument processing

**How to Compile and Run:**
```bash
gcc -Wall -Wextra -pthread -o document_analyzer project5_document_analyzer/document_analyzer.c
./document_analyzer "Cameroon" project5_document_analyzer/sample_document1.txt project5_document_analyzer/sample_document2.txt project5_document_analyzer/sample_document3.txt
```

📁 [View Project](project5_document_analyzer/)

---

## Build Requirements

| Project | Requirements |
|---------|-------------|
| Project 1 | Arduino IDE or Tinkercad |
| Project 2 | Linux/macOS with Bash |
| Project 3 | GCC compiler |
| Project 4 | GCC compiler |
| Project 5 | GCC compiler with pthreads (`-pthread` flag) |

## Author

**Nkem Jeferson Achia**

November 2026

## Code Style & Comments

All source code is documented with concise `//` comments (and `#` for shell scripts) directly describing what each function, structure, and critical block does without excessive verbosity.
