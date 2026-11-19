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
