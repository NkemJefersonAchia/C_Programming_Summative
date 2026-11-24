# Linux System Health and Resource Monitoring Manager

## Project Description

The **Linux System Health and Resource Monitoring Manager** is an interactive Bash shell script that provides comprehensive real-time monitoring of system resources on a Linux machine. It allows users to inspect CPU, memory, and disk utilization, manage running processes, generate detailed health reports, and maintain an activity log of all monitoring actions performed.

This tool is designed for system administrators and Linux users who need a quick, menu-driven interface to assess system health without installing third-party monitoring software.

---

## Features

- **System Information Display** — View hostname, OS details, kernel version, uptime, and current date/time.
- **CPU Utilization Monitoring** — View CPU usage summary, top CPU-consuming processes, and set custom warning thresholds.
- **Memory Utilization Monitoring** — View total, used, and available memory with percentage calculations and threshold-based warnings.
- **Disk Utilization Monitoring** — View disk space usage across all filesystems and identify partitions near capacity.
- **Process Viewing & Searching** — List all running processes or search by process name or PID.
- **Process Termination** — Kill a process by PID with confirmation, system-process protection (PID < 100), and permission handling.
- **Health Report Generation** — Generate a comprehensive system health report saved to a user-specified directory with a timestamped filename.
- **Activity Logging** — All actions are recorded with timestamps in an `activity_log.txt` file for audit and review.
- **Input Validation** — Robust validation for menu choices, thresholds (1–100), PIDs, and directory paths.
- **Error Handling** — Gracefully handles missing commands, permission errors, and invalid inputs.
- **Color-Coded Output** — Uses ANSI colors for clear, readable terminal output.

---

## How to Run

### Prerequisites

The script uses standard Linux commands that are available on most distributions:

| Command   | Purpose                          |
|-----------|----------------------------------|
| `bash`    | Shell interpreter (v4.0+)       |
| `uname`   | System/kernel information        |
| `hostname`| Display system hostname          |
| `uptime`  | System uptime                    |
| `top`     | CPU usage (batch mode)           |
| `free` / `vm_stat` | Memory usage statistics (Linux / macOS) |
| `df`      | Disk filesystem usage            |
| `ps`      | Process listing                  |
| `kill`    | Process termination              |
| `grep`    | Text pattern searching           |
| `awk`     | Text processing                  |
| `bc`      | Arithmetic calculations          |
| `wc`      | Line/word counting               |
| `date`    | Current date/time                |

### Running the Script

```bash
# 1. Navigate to the project directory
cd project2_linux_monitor/

# 2. Make the script executable (first time only)
chmod +x system_monitor.sh

# 3. Run the script
./system_monitor.sh
```

---

## Menu Options

When the script runs, it presents an interactive menu:

```
========================================================================
   Linux System Health and Resource Monitoring Manager
========================================================================

  1) Display System Information
  2) Monitor CPU Utilization
  3) Monitor Memory Utilization
  4) Monitor Disk Utilization
  5) View and Search Running Processes
  6) Terminate a Process
  7) Generate System Health Report
  8) View Activity History
  9) Clear Activity Log
  0) Exit

========================================================================
```

### Option Details

| Option | Name                        | Description |
|--------|-----------------------------|-------------|
| **1**  | Display System Information  | Shows hostname, full OS info (`uname -a`), kernel version, system uptime, and current date/time. |
| **2**  | Monitor CPU Utilization     | Displays CPU usage summary, lists the top 10 CPU-consuming processes, and allows setting a custom CPU warning threshold (1–100%). |
| **3**  | Monitor Memory Utilization  | Shows total, used, and available memory (using `free -h` on Linux or `vm_stat`/`sysctl` on macOS), calculates usage percentage, and warns if usage exceeds the user-defined threshold. |
| **4**  | Monitor Disk Utilization    | Displays disk space for all filesystems using `df -h`, highlights any filesystem at or above the usage threshold, and allows threshold customization. |
| **5**  | View & Search Processes     | Provides three sub-options: list all processes sorted by CPU, search by process name, or look up a specific PID. Displays PID, CPU%, MEM%, and command. |
| **6**  | Terminate a Process         | Prompts for a PID, shows process details, asks for confirmation, then terminates the process. Blocks termination of system-critical PIDs (< 100) and handles permission errors. |
| **7**  | Generate System Health Report | Creates a comprehensive text report with CPU, memory, disk, uptime, and process data. Saves to a user-specified directory with a timestamped filename (e.g., `system_health_report_20261121_143022.txt`). |
| **8**  | View Activity History       | Displays the contents of the `activity_log.txt` file, showing all actions performed with timestamps. |
| **9**  | Clear Activity Log          | Clears the activity log after user confirmation. Shows the number of entries before clearing. |
| **0**  | Exit                        | Logs the exit action and cleanly terminates the script. |

---

## Sample Output

### System Information (Option 1)
```
========================================================================
        SYSTEM INFORMATION
========================================================================
Hostname       : my-server
OS Information : Linux my-server 5.15.0-91-generic #101-Ubuntu SMP x86_64 GNU/Linux
Kernel Version : 5.15.0-91-generic
System Uptime  : 10:30:22 up 15 days,  3:42,  2 users,  load average: 0.52, 0.38, 0.31
Date/Time      : 2026-11-21 14:30:22 UTC
========================================================================
```

### Health Report Filename
```
system_health_report_20261121_143022.txt
```

---

## Activity Log

All actions are automatically logged to `activity_log.txt` in the script's directory. Each entry includes a timestamp:

```
[2026-11-21 14:30:00] System Monitor started
[2026-11-21 14:30:05] Viewed system information
[2026-11-21 14:30:15] Monitored CPU utilization
[2026-11-21 14:30:20] CPU threshold set to 90%
[2026-11-21 14:31:00] Generated system health report: ./system_health_report_20261121_143100.txt
[2026-11-21 14:31:30] System Monitor exited
```

---

## Project Structure

```
project2_linux_monitor/
├── system_monitor.sh      # Main monitoring script
├── README.md              # This documentation file
└── activity_log.txt       # Auto-generated activity log (created at runtime)
```

---

## Author

**Nkem Jeferson Achia**

---

## License

This project was developed as a summative assignment for the C Programming course (Trimester 6).
