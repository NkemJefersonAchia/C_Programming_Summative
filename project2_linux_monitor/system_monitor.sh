#!/bin/bash

###############################################################################
# Project Title : Linux System Health and Resource Monitoring Manager
# Description   : An interactive Bash shell script that monitors system health
#                 and resources on a Linux system. It provides real-time
#                 information about CPU, memory, disk utilization, process
#                 management, and generates comprehensive health reports.
# Author        : Nkem Jeferson Achia
# Date          : 2026-11-21
# Version       : 1.0
# Usage         : chmod +x system_monitor.sh && ./system_monitor.sh
###############################################################################

# =============================================================================
# GLOBAL VARIABLES
# =============================================================================

# Directory where this script resides (used for log file location)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Activity log file — stored in the same directory as the script
LOG_FILE="${SCRIPT_DIR}/activity_log.txt"

# Default thresholds (can be overridden by user at runtime)
CPU_THRESHOLD=80
MEMORY_THRESHOLD=80
DISK_THRESHOLD=80

# Color codes for terminal output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'  # No Color / Reset

# =============================================================================
# UTILITY FUNCTIONS
# =============================================================================

# -----------------------------------------------------------------------------
# log_activity()
# Description : Records an action with a timestamp to the activity log file.
# Parameters  : $1 — The activity message to log
# Returns     : None
# -----------------------------------------------------------------------------
log_activity() {
    local message="$1"
    local timestamp
    timestamp="$(date '+%Y-%m-%d %H:%M:%S')"
    echo "[${timestamp}] ${message}" >> "${LOG_FILE}" 2>/dev/null
    if [[ $? -ne 0 ]]; then
        echo -e "${RED}Warning: Could not write to activity log.${NC}"
    fi
}

# -----------------------------------------------------------------------------
# validate_threshold()
# Description : Validates that a threshold value is a number between 1 and 100.
# Parameters  : $1 — The value to validate
# Returns     : 0 if valid, 1 if invalid
# -----------------------------------------------------------------------------
validate_threshold() {
    local value="$1"

    # Check that it is a positive integer
    if ! [[ "${value}" =~ ^[0-9]+$ ]]; then
        echo -e "${RED}Error: Threshold must be a number between 1 and 100.${NC}"
        return 1
    fi

    # Check the range
    if [[ "${value}" -lt 1 || "${value}" -gt 100 ]]; then
        echo -e "${RED}Error: Threshold must be between 1 and 100.${NC}"
        return 1
    fi

    return 0
}

# -----------------------------------------------------------------------------
# check_command()
# Description : Checks if a required command is available on the system.
# Parameters  : $1 — The command name to check
# Returns     : 0 if available, 1 if missing
# -----------------------------------------------------------------------------
check_command() {
    local cmd="$1"
    if ! command -v "${cmd}" &>/dev/null; then
        echo -e "${YELLOW}Warning: '${cmd}' command not found. Some features may be limited.${NC}"
        return 1
    fi
    return 0
}

# -----------------------------------------------------------------------------
# print_separator()
# Description : Prints a visual separator line for readability.
# Parameters  : None
# Returns     : None
# -----------------------------------------------------------------------------
print_separator() {
    echo -e "${CYAN}========================================================================${NC}"
}

# -----------------------------------------------------------------------------
# press_enter()
# Description : Pauses execution and waits for the user to press Enter.
# Parameters  : None
# Returns     : None
# -----------------------------------------------------------------------------
press_enter() {
    echo ""
    read -rp "Press [Enter] to return to the menu..."
}

# =============================================================================
# CORE MONITORING FUNCTIONS
# =============================================================================

# -----------------------------------------------------------------------------
# display_system_info()
# Description : Displays general system information including hostname, OS,
#               kernel version, uptime, and current date/time.
# Parameters  : None
# Returns     : None
# -----------------------------------------------------------------------------
display_system_info() {
    print_separator
    echo -e "${BOLD}${BLUE}        SYSTEM INFORMATION${NC}"
    print_separator

    # Hostname
    echo -e "${GREEN}Hostname       :${NC} $(hostname 2>/dev/null || echo 'N/A')"

    # OS Information (full uname output)
    echo -e "${GREEN}OS Information :${NC} $(uname -a 2>/dev/null || echo 'N/A')"

    # Kernel Version
    echo -e "${GREEN}Kernel Version :${NC} $(uname -r 2>/dev/null || echo 'N/A')"

    # System Uptime
    if check_command uptime; then
        echo -e "${GREEN}System Uptime  :${NC} $(uptime 2>/dev/null || echo 'N/A')"
    else
        echo -e "${GREEN}System Uptime  :${NC} N/A (uptime command not found)"
    fi

    # Current Date and Time
    echo -e "${GREEN}Date/Time      :${NC} $(date '+%Y-%m-%d %H:%M:%S %Z' 2>/dev/null || echo 'N/A')"

    print_separator

    log_activity "Viewed system information"
    press_enter
}

# -----------------------------------------------------------------------------
# monitor_cpu()
# Description : Monitors CPU utilization, displays top CPU-consuming processes,
#               and allows the user to set a CPU usage warning threshold.
# Parameters  : None
# Returns     : None
# -----------------------------------------------------------------------------
monitor_cpu() {
    print_separator
    echo -e "${BOLD}${BLUE}        CPU UTILIZATION MONITOR${NC}"
    print_separator

    # --- Gather CPU usage ---
    # Try 'top' first (batch mode, single iteration)
    if check_command top; then
        echo -e "${GREEN}CPU Usage Summary:${NC}"

        # On Linux, top -bn1 works; on macOS, top works differently
        if [[ "$(uname)" == "Linux" ]]; then
            # Extract the CPU line from top
            local cpu_line
            cpu_line=$(top -bn1 2>/dev/null | grep -i "cpu(s)" | head -1)
            if [[ -n "${cpu_line}" ]]; then
                echo "  ${cpu_line}"
                # Calculate usage: 100 - idle
                local idle
                idle=$(echo "${cpu_line}" | grep -oP '[\d.]+(?=\s*id)' 2>/dev/null)
                if [[ -n "${idle}" ]]; then
                    local usage
                    usage=$(echo "100 - ${idle}" | bc 2>/dev/null)
                    if [[ -n "${usage}" ]]; then
                        echo -e "  ${GREEN}Overall CPU Usage:${NC} ${usage}%"
                        # Check against threshold
                        local usage_int
                        usage_int=$(printf "%.0f" "${usage}" 2>/dev/null)
                        if [[ -n "${usage_int}" && "${usage_int}" -ge "${CPU_THRESHOLD}" ]]; then
                            echo -e "  ${RED}⚠  WARNING: CPU usage (${usage}%) exceeds threshold (${CPU_THRESHOLD}%)!${NC}"
                        fi
                    fi
                fi
            else
                echo "  Could not parse CPU information from top."
            fi
        else
            # macOS fallback
            echo "  (Using macOS-compatible mode)"
            top -l 1 -n 0 2>/dev/null | grep -i "cpu usage" | head -1
        fi
        echo ""
    elif check_command mpstat; then
        echo -e "${GREEN}CPU Usage (via mpstat):${NC}"
        mpstat 2>/dev/null
        echo ""
    else
        echo -e "${RED}Error: Neither 'top' nor 'mpstat' is available.${NC}"
    fi

    # --- Top CPU-consuming processes ---
    echo -e "${GREEN}Top 10 CPU-Consuming Processes:${NC}"
    echo -e "${BOLD}  PID     CPU%   MEM%   COMMAND${NC}"
    if check_command ps; then
        ps aux --sort=-%cpu 2>/dev/null | head -11 | tail -10 | \
            awk '{printf "  %-8s %-6s %-6s %s\n", $2, $3, $4, $11}' 2>/dev/null
        # Fallback for macOS (ps aux --sort may not work)
        if [[ $? -ne 0 ]]; then
            ps aux 2>/dev/null | sort -nrk 3 | head -10 | \
                awk '{printf "  %-8s %-6s %-6s %s\n", $2, $3, $4, $11}'
        fi
    fi
    echo ""

    # --- Allow user to set CPU threshold ---
    echo -e "${YELLOW}Current CPU warning threshold: ${CPU_THRESHOLD}%${NC}"
    read -rp "Set new CPU threshold (1-100) or press Enter to keep current: " new_threshold

    if [[ -n "${new_threshold}" ]]; then
        if validate_threshold "${new_threshold}"; then
            CPU_THRESHOLD="${new_threshold}"
            echo -e "${GREEN}CPU threshold updated to ${CPU_THRESHOLD}%.${NC}"
            log_activity "CPU threshold set to ${CPU_THRESHOLD}%"
        fi
    fi

    log_activity "Monitored CPU utilization"
    press_enter
}

# -----------------------------------------------------------------------------
# monitor_memory()
# Description : Monitors memory utilization, shows total/used/available memory,
#               calculates usage percentage, and warns if above threshold.
# Parameters  : None
# Returns     : None
# -----------------------------------------------------------------------------
monitor_memory() {
    print_separator
    echo -e "${BOLD}${BLUE}        MEMORY UTILIZATION MONITOR${NC}"
    print_separator

    # --- Display memory info ---
    if check_command free; then
        echo -e "${GREEN}Memory Information:${NC}"
        free -h 2>/dev/null
        echo ""

        # Calculate memory usage percentage
        local mem_info
        mem_info=$(free 2>/dev/null | grep -i "mem:")
        if [[ -n "${mem_info}" ]]; then
            local total used available
            total=$(echo "${mem_info}" | awk '{print $2}')
            used=$(echo "${mem_info}" | awk '{print $3}')
            available=$(echo "${mem_info}" | awk '{print $7}')

            if [[ "${total}" -gt 0 ]] 2>/dev/null; then
                local usage_pct
                usage_pct=$(( (used * 100) / total ))
                echo -e "${GREEN}Memory Usage:${NC} ${usage_pct}% used"
                echo -e "${GREEN}Total:${NC} $(free -h | grep -i mem | awk '{print $2}')  |  " \
                     "${GREEN}Used:${NC} $(free -h | grep -i mem | awk '{print $3}')  |  " \
                     "${GREEN}Available:${NC} $(free -h | grep -i mem | awk '{print $7}')"
                echo ""

                # Check against threshold
                if [[ "${usage_pct}" -ge "${MEMORY_THRESHOLD}" ]]; then
                    echo -e "${RED}⚠  WARNING: Memory usage (${usage_pct}%) exceeds threshold (${MEMORY_THRESHOLD}%)!${NC}"
                fi
            fi
        fi
    else
        # Fallback: try /proc/meminfo (Linux-specific)
        if [[ -f /proc/meminfo ]]; then
            echo -e "${GREEN}Memory Information (from /proc/meminfo):${NC}"
            local total_kb free_kb available_kb
            total_kb=$(grep -i "MemTotal" /proc/meminfo | awk '{print $2}')
            free_kb=$(grep -i "MemFree" /proc/meminfo | awk '{print $2}')
            available_kb=$(grep -i "MemAvailable" /proc/meminfo | awk '{print $2}')

            echo "  Total:     $((total_kb / 1024)) MB"
            echo "  Free:      $((free_kb / 1024)) MB"
            echo "  Available: $((available_kb / 1024)) MB"

            if [[ "${total_kb}" -gt 0 ]] 2>/dev/null; then
                local used_kb=$((total_kb - available_kb))
                local usage_pct=$(( (used_kb * 100) / total_kb ))
                echo -e "  ${GREEN}Usage:${NC} ${usage_pct}%"
                if [[ "${usage_pct}" -ge "${MEMORY_THRESHOLD}" ]]; then
                    echo -e "  ${RED}⚠  WARNING: Memory usage (${usage_pct}%) exceeds threshold (${MEMORY_THRESHOLD}%)!${NC}"
                fi
            fi
        else
            echo -e "${RED}Error: 'free' command not available and /proc/meminfo not found.${NC}"
        fi
    fi
    echo ""

    # --- Allow user to set memory threshold ---
    echo -e "${YELLOW}Current memory warning threshold: ${MEMORY_THRESHOLD}%${NC}"
    read -rp "Set new memory threshold (1-100) or press Enter to keep current: " new_threshold

    if [[ -n "${new_threshold}" ]]; then
        if validate_threshold "${new_threshold}"; then
            MEMORY_THRESHOLD="${new_threshold}"
            echo -e "${GREEN}Memory threshold updated to ${MEMORY_THRESHOLD}%.${NC}"
            log_activity "Memory threshold set to ${MEMORY_THRESHOLD}%"
        fi
    fi

    log_activity "Monitored memory utilization"
    press_enter
}

# -----------------------------------------------------------------------------
# monitor_disk()
# Description : Monitors disk utilization, shows filesystem usage, identifies
#               filesystems near capacity, and allows threshold configuration.
# Parameters  : None
# Returns     : None
# -----------------------------------------------------------------------------
monitor_disk() {
    print_separator
    echo -e "${BOLD}${BLUE}        DISK UTILIZATION MONITOR${NC}"
    print_separator

    # --- Display disk space ---
    if check_command df; then
        echo -e "${GREEN}Disk Space Usage:${NC}"
        df -h 2>/dev/null
        echo ""

        # --- Identify filesystems near or above threshold ---
        echo -e "${GREEN}Filesystems at or above ${DISK_THRESHOLD}% capacity:${NC}"
        local found_warning=false

        # Parse df output, skip header line
        while IFS= read -r line; do
            # Extract the usage percentage (remove the % sign)
            local usage_pct
            usage_pct=$(echo "${line}" | awk '{print $5}' | tr -d '%')

            # Validate it is a number
            if [[ "${usage_pct}" =~ ^[0-9]+$ ]]; then
                if [[ "${usage_pct}" -ge "${DISK_THRESHOLD}" ]]; then
                    local filesystem mountpoint
                    filesystem=$(echo "${line}" | awk '{print $1}')
                    mountpoint=$(echo "${line}" | awk '{print $6}')
                    echo -e "  ${RED}⚠  ${filesystem} mounted on ${mountpoint} — ${usage_pct}% used${NC}"
                    found_warning=true
                fi
            fi
        done < <(df -h 2>/dev/null | tail -n +2)

        if [[ "${found_warning}" == false ]]; then
            echo -e "  ${GREEN}✓ All filesystems are below the ${DISK_THRESHOLD}% threshold.${NC}"
        fi
    else
        echo -e "${RED}Error: 'df' command not found.${NC}"
    fi
    echo ""

    # --- Allow user to set disk threshold ---
    echo -e "${YELLOW}Current disk usage warning threshold: ${DISK_THRESHOLD}%${NC}"
    read -rp "Set new disk threshold (1-100) or press Enter to keep current: " new_threshold

    if [[ -n "${new_threshold}" ]]; then
        if validate_threshold "${new_threshold}"; then
            DISK_THRESHOLD="${new_threshold}"
            echo -e "${GREEN}Disk threshold updated to ${DISK_THRESHOLD}%.${NC}"
            log_activity "Disk threshold set to ${DISK_THRESHOLD}%"
        fi
    fi

    log_activity "Monitored disk utilization"
    press_enter
}

# -----------------------------------------------------------------------------
# search_processes()
# Description : Lists running processes and allows user to search by name
#               or PID. Displays PID, CPU%, MEM%, and command information.
# Parameters  : None
# Returns     : None
# -----------------------------------------------------------------------------
search_processes() {
    print_separator
    echo -e "${BOLD}${BLUE}        VIEW AND SEARCH RUNNING PROCESSES${NC}"
    print_separator

    if ! check_command ps; then
        echo -e "${RED}Error: 'ps' command not found.${NC}"
        log_activity "Attempted to view processes — ps command not found"
        press_enter
        return
    fi

    echo "Options:"
    echo "  1) List all running processes"
    echo "  2) Search by process name"
    echo "  3) Search by PID"
    echo ""
    read -rp "Select an option (1-3): " search_option

    case "${search_option}" in
        1)
            echo ""
            echo -e "${GREEN}All Running Processes (sorted by CPU usage):${NC}"
            echo -e "${BOLD}  PID      CPU%   MEM%   COMMAND${NC}"
            ps aux --sort=-%cpu 2>/dev/null | tail -n +2 | \
                awk '{printf "  %-8s %-6s %-6s %s\n", $2, $3, $4, $11}' 2>/dev/null
            # Fallback if --sort is not supported
            if [[ $? -ne 0 ]]; then
                ps aux 2>/dev/null | tail -n +2 | sort -nrk 3 | \
                    awk '{printf "  %-8s %-6s %-6s %s\n", $2, $3, $4, $11}'
            fi
            log_activity "Listed all running processes"
            ;;
        2)
            read -rp "Enter process name to search: " proc_name
            if [[ -z "${proc_name}" ]]; then
                echo -e "${RED}Error: Process name cannot be empty.${NC}"
            else
                echo ""
                echo -e "${GREEN}Processes matching '${proc_name}':${NC}"
                echo -e "${BOLD}  PID      CPU%   MEM%   COMMAND${NC}"
                local results
                results=$(ps aux 2>/dev/null | grep -i "${proc_name}" | grep -v "grep" | \
                    awk '{printf "  %-8s %-6s %-6s %s\n", $2, $3, $4, $11}')
                if [[ -z "${results}" ]]; then
                    echo -e "  ${YELLOW}No processes found matching '${proc_name}'.${NC}"
                else
                    echo "${results}"
                fi
                log_activity "Searched processes by name: '${proc_name}'"
            fi
            ;;
        3)
            read -rp "Enter PID to search: " search_pid
            if ! [[ "${search_pid}" =~ ^[0-9]+$ ]]; then
                echo -e "${RED}Error: PID must be a positive number.${NC}"
            else
                echo ""
                echo -e "${GREEN}Process with PID ${search_pid}:${NC}"
                echo -e "${BOLD}  PID      CPU%   MEM%   COMMAND${NC}"
                local result
                result=$(ps aux 2>/dev/null | awk -v pid="${search_pid}" '$2 == pid {printf "  %-8s %-6s %-6s %s\n", $2, $3, $4, $11}')
                if [[ -z "${result}" ]]; then
                    echo -e "  ${YELLOW}No process found with PID ${search_pid}.${NC}"
                else
                    echo "${result}"
                fi
                log_activity "Searched process by PID: ${search_pid}"
            fi
            ;;
        *)
            echo -e "${RED}Invalid option. Please select 1, 2, or 3.${NC}"
            ;;
    esac

    press_enter
}

# -----------------------------------------------------------------------------
# terminate_process()
# Description : Terminates a process by PID after user confirmation. Protects
#               system-critical processes (PID < 100) and handles permission
#               errors gracefully.
# Parameters  : None
# Returns     : None
# -----------------------------------------------------------------------------
terminate_process() {
    print_separator
    echo -e "${BOLD}${BLUE}        TERMINATE A PROCESS${NC}"
    print_separator

    if ! check_command kill; then
        echo -e "${RED}Error: 'kill' command not found.${NC}"
        log_activity "Attempted to terminate process — kill command not found"
        press_enter
        return
    fi

    read -rp "Enter the PID of the process to terminate: " target_pid

    # --- Validate PID input ---
    if [[ -z "${target_pid}" ]]; then
        echo -e "${RED}Error: PID cannot be empty.${NC}"
        press_enter
        return
    fi

    if ! [[ "${target_pid}" =~ ^[0-9]+$ ]]; then
        echo -e "${RED}Error: PID must be a positive number.${NC}"
        press_enter
        return
    fi

    # --- Protect system-critical processes ---
    if [[ "${target_pid}" -lt 100 ]]; then
        echo -e "${RED}Error: Cannot terminate system-critical process (PID < 100).${NC}"
        echo -e "${RED}PIDs below 100 are reserved for essential system processes.${NC}"
        log_activity "Blocked attempt to kill system process PID ${target_pid}"
        press_enter
        return
    fi

    # --- Check if the process exists ---
    if ! ps -p "${target_pid}" &>/dev/null; then
        echo -e "${RED}Error: No process found with PID ${target_pid}.${NC}"
        press_enter
        return
    fi

    # --- Show process details before killing ---
    echo -e "${GREEN}Process details:${NC}"
    ps -p "${target_pid}" -o pid,user,%cpu,%mem,comm 2>/dev/null
    echo ""

    # --- Confirm with the user ---
    read -rp "Are you sure you want to terminate PID ${target_pid}? (y/n): " confirm
    if [[ "${confirm}" != "y" && "${confirm}" != "Y" ]]; then
        echo -e "${YELLOW}Process termination cancelled.${NC}"
        log_activity "Cancelled termination of PID ${target_pid}"
        press_enter
        return
    fi

    # --- Attempt to kill the process ---
    if kill "${target_pid}" 2>/dev/null; then
        echo -e "${GREEN}✓ Process ${target_pid} has been terminated successfully.${NC}"
        log_activity "Terminated process PID ${target_pid}"
    else
        echo -e "${RED}Error: Failed to terminate PID ${target_pid}.${NC}"
        echo -e "${YELLOW}You may not have sufficient permissions. Try running with sudo.${NC}"
        log_activity "Failed to terminate PID ${target_pid} — permission denied"
    fi

    press_enter
}

# -----------------------------------------------------------------------------
# generate_report()
# Description : Generates a comprehensive system health report containing
#               CPU, memory, disk, and uptime information. Saves to a
#               user-specified directory with a timestamp in the filename.
# Parameters  : None
# Returns     : None
# -----------------------------------------------------------------------------
generate_report() {
    print_separator
    echo -e "${BOLD}${BLUE}        GENERATE SYSTEM HEALTH REPORT${NC}"
    print_separator

    # --- Ask user for the output directory ---
    read -rp "Enter directory to save the report (or press Enter for current directory): " report_dir

    # Default to current directory if empty
    if [[ -z "${report_dir}" ]]; then
        report_dir="."
    fi

    # --- Validate directory ---
    if [[ ! -d "${report_dir}" ]]; then
        echo -e "${YELLOW}Directory '${report_dir}' does not exist. Create it? (y/n): ${NC}"
        read -r create_dir
        if [[ "${create_dir}" == "y" || "${create_dir}" == "Y" ]]; then
            mkdir -p "${report_dir}" 2>/dev/null
            if [[ $? -ne 0 ]]; then
                echo -e "${RED}Error: Could not create directory '${report_dir}'.${NC}"
                log_activity "Failed to create report directory: ${report_dir}"
                press_enter
                return
            fi
        else
            echo -e "${RED}Report generation cancelled.${NC}"
            press_enter
            return
        fi
    fi

    # Check write permission
    if [[ ! -w "${report_dir}" ]]; then
        echo -e "${RED}Error: No write permission to directory '${report_dir}'.${NC}"
        log_activity "Report generation failed — no write permission to ${report_dir}"
        press_enter
        return
    fi

    # --- Generate the report ---
    local timestamp
    timestamp="$(date '+%Y%m%d_%H%M%S')"
    local report_file="${report_dir}/system_health_report_${timestamp}.txt"

    echo -e "${GREEN}Generating system health report...${NC}"

    {
        echo "================================================================================"
        echo "                     SYSTEM HEALTH REPORT"
        echo "================================================================================"
        echo "Generated on : $(date '+%Y-%m-%d %H:%M:%S %Z')"
        echo "Hostname     : $(hostname 2>/dev/null || echo 'N/A')"
        echo ""

        echo "--- SYSTEM INFORMATION ---"
        echo "OS Info      : $(uname -a 2>/dev/null || echo 'N/A')"
        echo "Kernel       : $(uname -r 2>/dev/null || echo 'N/A')"
        echo "Uptime       : $(uptime 2>/dev/null || echo 'N/A')"
        echo ""

        echo "--- CPU INFORMATION ---"
        if [[ "$(uname)" == "Linux" ]]; then
            top -bn1 2>/dev/null | head -5
        else
            top -l 1 -n 0 2>/dev/null | head -10
        fi
        echo ""
        echo "Top 10 CPU Processes:"
        ps aux --sort=-%cpu 2>/dev/null | head -11 || ps aux 2>/dev/null | sort -nrk 3 | head -11
        echo ""

        echo "--- MEMORY INFORMATION ---"
        if check_command free; then
            free -h 2>/dev/null
        elif [[ -f /proc/meminfo ]]; then
            head -5 /proc/meminfo
        else
            echo "Memory info not available"
        fi
        echo ""

        echo "--- DISK INFORMATION ---"
        df -h 2>/dev/null || echo "Disk info not available"
        echo ""

        echo "--- ACTIVE PROCESSES COUNT ---"
        echo "Total processes: $(ps aux 2>/dev/null | wc -l | tr -d ' ')"
        echo ""

        echo "================================================================================"
        echo "                     END OF REPORT"
        echo "================================================================================"
    } > "${report_file}" 2>/dev/null

    if [[ $? -eq 0 ]]; then
        echo -e "${GREEN}✓ Report saved to: ${report_file}${NC}"
        log_activity "Generated system health report: ${report_file}"
    else
        echo -e "${RED}Error: Failed to write report to '${report_file}'.${NC}"
        log_activity "Failed to generate report at: ${report_file}"
    fi

    press_enter
}

# -----------------------------------------------------------------------------
# view_activity_log()
# Description : Displays the contents of the activity log file. If the log
#               does not exist or is empty, informs the user.
# Parameters  : None
# Returns     : None
# -----------------------------------------------------------------------------
view_activity_log() {
    print_separator
    echo -e "${BOLD}${BLUE}        ACTIVITY HISTORY${NC}"
    print_separator

    if [[ ! -f "${LOG_FILE}" ]]; then
        echo -e "${YELLOW}No activity log found. No actions have been recorded yet.${NC}"
    elif [[ ! -s "${LOG_FILE}" ]]; then
        echo -e "${YELLOW}Activity log is empty.${NC}"
    else
        echo -e "${GREEN}Activity Log (${LOG_FILE}):${NC}"
        echo ""
        cat "${LOG_FILE}"
    fi

    press_enter
}

# -----------------------------------------------------------------------------
# clear_activity_log()
# Description : Clears the activity log file after user confirmation.
# Parameters  : None
# Returns     : None
# -----------------------------------------------------------------------------
clear_activity_log() {
    print_separator
    echo -e "${BOLD}${BLUE}        CLEAR ACTIVITY LOG${NC}"
    print_separator

    if [[ ! -f "${LOG_FILE}" ]]; then
        echo -e "${YELLOW}No activity log file exists. Nothing to clear.${NC}"
