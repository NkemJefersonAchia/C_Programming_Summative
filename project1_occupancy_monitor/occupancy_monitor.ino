/*
 * ============================================================================
 * Project:     Smart Occupancy Monitoring and Access Control System
 * Description: An Arduino-based system that monitors room occupancy using
 *              entry/exit push buttons, displays real-time occupancy data on
 *              a 16x2 LCD, indicates capacity status via LEDs, and logs all
 *              events to the Serial Monitor. Uses dynamic memory allocation
 *              with pointers for occupancy data management.
 * Author:      Nkem Jeferson Achia
 * Date:        2026-11-21
 * Platform:    Arduino Uno (Tinkercad Simulation)
 * ============================================================================
 */

#include <LiquidCrystal.h>
#include <stdlib.h>

