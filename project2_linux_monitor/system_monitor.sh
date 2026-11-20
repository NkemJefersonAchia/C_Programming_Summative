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
