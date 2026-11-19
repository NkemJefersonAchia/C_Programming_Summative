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

/* ---------------------------------------------------------------------------
 * Pin Definitions
 * --------------------------------------------------------------------------- */
#define ENTRY_BUTTON_PIN  2   /* Push button for entry (digital pin 2)       */
#define EXIT_BUTTON_PIN   3   /* Push button for exit  (digital pin 3)       */
#define LCD_RS_PIN       12   /* LCD Register Select pin                     */
#define LCD_EN_PIN       11   /* LCD Enable pin                              */
#define LCD_D4_PIN        7   /* LCD Data pin 4                              */
#define LCD_D5_PIN        6   /* LCD Data pin 5                              */
#define LCD_D6_PIN        5   /* LCD Data pin 6                              */
#define LCD_D7_PIN        4   /* LCD Data pin 7                              */
#define GREEN_LED_PIN     8   /* Green LED — capacity available              */
#define RED_LED_PIN       9   /* Red LED   — at maximum capacity             */

/* ---------------------------------------------------------------------------
