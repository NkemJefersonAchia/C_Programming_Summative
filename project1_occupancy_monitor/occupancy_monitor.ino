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
 * System Constants
 * --------------------------------------------------------------------------- */
#define MAX_CAPACITY       10  /* Maximum number of people allowed in the room */
#define DEBOUNCE_DELAY    200  /* Button debounce interval in milliseconds     */
#define LCD_UPDATE_INTERVAL 500 /* LCD refresh interval in milliseconds       */
#define SERIAL_BAUD_RATE 9600  /* Serial communication baud rate              */

/* ---------------------------------------------------------------------------
 * Data Structures
 * --------------------------------------------------------------------------- */

/**
 * @struct OccupancyInfo
 * @brief  Holds all occupancy-related counters for the monitored room.
 *
 * @field currentCount  Number of people currently in the room (0..MAX_CAPACITY)
 * @field maxCapacity   Maximum allowed occupancy
 * @field totalEntries  Cumulative count of successful entry events
 * @field totalExits    Cumulative count of successful exit events
 */
typedef struct {
    int currentCount;
    int maxCapacity;
    unsigned long totalEntries;
    unsigned long totalExits;
} OccupancyInfo;

/* ---------------------------------------------------------------------------
 * Global Variables
 * --------------------------------------------------------------------------- */

/* LCD object: RS, EN, D4, D5, D6, D7 */
LiquidCrystal lcd(LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN,
                   LCD_D6_PIN, LCD_D7_PIN);

/* Pointer to dynamically allocated OccupancyInfo struct */
OccupancyInfo* pOccupancy = NULL;

/* Timing variables for non-blocking debounce (millis-based) */
unsigned long lastEntryPressTime = 0;  /* Last valid entry button press time  */
unsigned long lastExitPressTime  = 0;  /* Last valid exit button press time   */
unsigned long lastLCDUpdateTime  = 0;  /* Last LCD refresh time               */

/* Previous button states for edge detection */
int prevEntryState = HIGH;  /* Previous reading of entry button (pull-up)    */
int prevExitState  = HIGH;  /* Previous reading of exit button  (pull-up)    */

/* ---------------------------------------------------------------------------
 * Function Prototypes
 * --------------------------------------------------------------------------- */
void initSystem(void);
void handleEntry(OccupancyInfo* pOcc);
void handleExit(OccupancyInfo* pOcc);
void updateLCD(OccupancyInfo* pOcc);
void updateLEDs(OccupancyInfo* pOcc);
void printStatus(OccupancyInfo* pOcc, const char* event);
void printError(const char* errorMsg);
void cleanupSystem(void);
String getTimestamp(void);

/* ===========================================================================
 * setup() — Arduino entry point. Initialises hardware and allocates memory.
 * =========================================================================== */
void setup() {
    initSystem();
}

/* ===========================================================================
 * loop() — Main program loop. Reads buttons, updates outputs. No delay().
 * =========================================================================== */
void loop() {
    /* Safety check: if allocation failed, do nothing */
    if (pOccupancy == NULL) {
        return;
    }

    unsigned long currentMillis = millis();

    /* --- Read button states (active LOW with INPUT_PULLUP) --- */
    int entryReading = digitalRead(ENTRY_BUTTON_PIN);
    int exitReading  = digitalRead(EXIT_BUTTON_PIN);

    /* --- Entry button: detect falling edge with debounce --- */
    if (entryReading == LOW && prevEntryState == HIGH) {
        if ((currentMillis - lastEntryPressTime) >= DEBOUNCE_DELAY) {
            handleEntry(pOccupancy);
            lastEntryPressTime = currentMillis;
        }
    }
    prevEntryState = entryReading;

    /* --- Exit button: detect falling edge with debounce --- */
    if (exitReading == LOW && prevExitState == HIGH) {
        if ((currentMillis - lastExitPressTime) >= DEBOUNCE_DELAY) {
            handleExit(pOccupancy);
            lastExitPressTime = currentMillis;
        }
    }
    prevExitState = exitReading;

    /* --- Periodically refresh the LCD (every LCD_UPDATE_INTERVAL ms) --- */
    if ((currentMillis - lastLCDUpdateTime) >= LCD_UPDATE_INTERVAL) {
        updateLCD(pOccupancy);
        lastLCDUpdateTime = currentMillis;
    }

    /* --- Continuously update LEDs to reflect current state --- */
    updateLEDs(pOccupancy);
}

/* ===========================================================================
 * initSystem() — Initialise all peripherals and allocate occupancy struct.
 *
 * Configures pin modes, starts Serial and LCD, dynamically allocates the
 * OccupancyInfo struct via malloc(), validates the allocation, and performs
 * the first LCD + LED update.
 * =========================================================================== */
void initSystem(void) {
    /* Configure button pins with internal pull-up resistors */
    pinMode(ENTRY_BUTTON_PIN, INPUT_PULLUP);
    pinMode(EXIT_BUTTON_PIN,  INPUT_PULLUP);

    /* Configure LED pins as outputs */
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN,   OUTPUT);

    /* Start serial communication */
    Serial.begin(SERIAL_BAUD_RATE);

    /* Initialise the 16x2 LCD */
    lcd.begin(16, 2);

    /* --- Dynamically allocate the OccupancyInfo struct --- */
    pOccupancy = (OccupancyInfo*)malloc(sizeof(OccupancyInfo));

    /* Validate allocation */
    if (pOccupancy == NULL) {
        Serial.println(F("ERROR: malloc() failed — unable to allocate OccupancyInfo!"));
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MEMORY ERROR!");
        lcd.setCursor(0, 1);
        lcd.print("System halted.");
        /* Turn both LEDs on to signal fault */
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, HIGH);
        return;  /* loop() will check for NULL and skip */
    }

    /* Initialise struct fields via pointer */
    pOccupancy->currentCount  = 0;
    pOccupancy->maxCapacity   = MAX_CAPACITY;
    pOccupancy->totalEntries  = 0;
    pOccupancy->totalExits    = 0;

    /* Print startup banner to Serial Monitor */
    Serial.println(F("============================================================"));
    Serial.println(F("  Smart Occupancy Monitoring and Access Control System"));
    Serial.println(F("============================================================"));
    Serial.print(F("  Max Capacity : "));
    Serial.println(MAX_CAPACITY);
    Serial.print(F("  System Start : "));
    Serial.println(getTimestamp());
    Serial.println(F("============================================================"));
    Serial.println();

    /* Initial display update */
    updateLCD(pOccupancy);
    updateLEDs(pOccupancy);
}

/* ===========================================================================
 * handleEntry() — Process an entry event.
 *
 * @param pOcc  Pointer to the OccupancyInfo struct.
 *
 * If currentCount < maxCapacity, increments currentCount and totalEntries,
 * then prints updated status to Serial. Otherwise, prints an error message
 * indicating the room is at full capacity.
 * =========================================================================== */
void handleEntry(OccupancyInfo* pOcc) {
    if (pOcc->currentCount < pOcc->maxCapacity) {
        pOcc->currentCount++;
        pOcc->totalEntries++;
        printStatus(pOcc, "ENTRY");
    } else {
        /* Room is full — reject entry */
        Serial.print("[");
        Serial.print(getTimestamp());
        Serial.print("] ");
        Serial.println(F("ERROR: Entry REJECTED — room is at MAXIMUM CAPACITY!"));
        Serial.print(F("       Current count: "));
        Serial.print(pOcc->currentCount);
        Serial.print(F("/"));
        Serial.println(pOcc->maxCapacity);
        Serial.println();
    }
}

/* ===========================================================================
 * handleExit() — Process an exit event.
 *
 * @param pOcc  Pointer to the OccupancyInfo struct.
 *
 * If currentCount > 0, decrements currentCount and increments totalExits,
 * then prints updated status to Serial. Otherwise, prints an error message
 * indicating the room is already empty.
 * =========================================================================== */
void handleExit(OccupancyInfo* pOcc) {
    if (pOcc->currentCount > 0) {
        pOcc->currentCount--;
        pOcc->totalExits++;
        printStatus(pOcc, "EXIT");
    } else {
        /* Room is empty — reject exit */
        Serial.print("[");
        Serial.print(getTimestamp());
        Serial.print("] ");
        Serial.println(F("ERROR: Exit REJECTED — room is already EMPTY!"));
        Serial.print(F("       Current count: "));
        Serial.print(pOcc->currentCount);
        Serial.print(F("/"));
        Serial.println(pOcc->maxCapacity);
        Serial.println();
    }
}

/* ===========================================================================
 * updateLCD() — Refresh the 16x2 LCD with current occupancy data.
 *
 * @param pOcc  Pointer to the OccupancyInfo struct.
 *
 * Line 1: "People: X"       (current occupant count)
 * Line 2: "Available: Y"    (remaining capacity)
 * =========================================================================== */
void updateLCD(OccupancyInfo* pOcc) {
    int available = pOcc->maxCapacity - pOcc->currentCount;

    /* Line 1: People count */
    lcd.setCursor(0, 0);
    lcd.print("People:    ");      /* Pad to clear previous digits */
    lcd.setCursor(8, 0);
    lcd.print(pOcc->currentCount);
    lcd.print("   ");              /* Clear trailing characters */

    /* Line 2: Available spots */
    lcd.setCursor(0, 1);
    lcd.print("Available: ");
    lcd.setCursor(11, 1);
    lcd.print(available);
    lcd.print("   ");              /* Clear trailing characters */
}

/* ===========================================================================
 * updateLEDs() — Set LED states based on current occupancy.
 *
 * @param pOcc  Pointer to the OccupancyInfo struct.
 *
 * Green ON / Red OFF  → capacity available (currentCount < maxCapacity)
 * Green OFF / Red ON  → at maximum capacity (currentCount == maxCapacity)
 * =========================================================================== */
void updateLEDs(OccupancyInfo* pOcc) {
    if (pOcc->currentCount >= pOcc->maxCapacity) {
        /* At full capacity */
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN,   HIGH);
    } else {
        /* Capacity available */
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN,   LOW);
    }
}

/* ===========================================================================
 * printStatus() — Print a formatted event log entry to the Serial Monitor.
 *
 * @param pOcc   Pointer to the OccupancyInfo struct.
 * @param event  String label for the event (e.g. "ENTRY" or "EXIT").
 *
 * Output format:
 *   [timestamp] EVENT — Count: X/MAX | Available: Y | Entries: E | Exits: X
 * =========================================================================== */
void printStatus(OccupancyInfo* pOcc, const char* event) {
    int available = pOcc->maxCapacity - pOcc->currentCount;

    Serial.print("[");
    Serial.print(getTimestamp());
    Serial.print("] ");
    Serial.print(event);
    Serial.print(F(" — Count: "));
    Serial.print(pOcc->currentCount);
    Serial.print(F("/"));
    Serial.print(pOcc->maxCapacity);
    Serial.print(F(" | Available: "));
    Serial.print(available);
    Serial.print(F(" | Total Entries: "));
    Serial.print(pOcc->totalEntries);
    Serial.print(F(" | Total Exits: "));
    Serial.println(pOcc->totalExits);
}

/* ===========================================================================
 * printError() — Print a formatted error message to the Serial Monitor.
 *
 * @param errorMsg  The error message string to display.
 * =========================================================================== */
void printError(const char* errorMsg) {
    Serial.print("[");
    Serial.print(getTimestamp());
    Serial.print("] ERROR: ");
    Serial.println(errorMsg);
}

