// Smart Occupancy Monitoring and Access Control System
// Author: Nkem Jeferson Achia

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <stdlib.h>

// Pin definitions
#define ENTRY_BUTTON_PIN  2   // Push button for entry (digital pin 2)
#define EXIT_BUTTON_PIN   3   // Push button for exit  (digital pin 3)
#define GREEN_LED_PIN     8   // Green LED — capacity available
#define RED_LED_PIN       9   // Red LED   — at maximum capacity

// I2C LCD configuration (SDA connects to A4, SCL connects to A5 on Arduino Uno)
#define I2C_LCD_ADDR      0x27 // Default I2C address (0x20, 0x27 or 0x3F in Tinkercad)
#define LCD_COLS          16   // 16 columns
#define LCD_ROWS          2    // 2 rows

// System constants
#define MAX_CAPACITY        10  // Maximum number of people allowed in room
#define DEBOUNCE_DELAY     200  // Button debounce interval in milliseconds
#define LCD_UPDATE_INTERVAL 500 // LCD refresh interval in milliseconds
#define SERIAL_BAUD_RATE  9600  // Serial communication baud rate

// Structure to organize occupancy data
typedef struct {
    int currentCount;
    int maxCapacity;
    unsigned long totalEntries;
    unsigned long totalExits;
} OccupancyInfo;

// Create I2C LCD object
LiquidCrystal_I2C lcd(I2C_LCD_ADDR, LCD_COLS, LCD_ROWS);

// Pointer to dynamically allocated occupancy structure
OccupancyInfo* pOccupancy = NULL;

// Timing variables for non-blocking debounce and LCD refresh
unsigned long lastEntryPressTime = 0;  // Last valid entry press time
unsigned long lastExitPressTime  = 0;  // Last valid exit press time
unsigned long lastLCDUpdateTime  = 0;  // Last LCD refresh time

// Previous button readings for edge detection
int prevEntryState = HIGH;  // Previous state of entry button (pull-up)
int prevExitState  = HIGH;  // Previous state of exit button  (pull-up)

// Function prototypes
void initSystem(void);
void handleEntry(OccupancyInfo* pOcc);
void handleExit(OccupancyInfo* pOcc);
void updateLCD(OccupancyInfo* pOcc);
void updateLEDs(OccupancyInfo* pOcc);
void printStatus(OccupancyInfo* pOcc, const char* event);
void printError(const char* errorMsg);
void cleanupSystem(void);
String getTimestamp(void);

// Setup hardware and allocate memory
void setup() {
    initSystem();
}

// Main loop: non-blocking button checking and periodic display refresh
void loop() {
    if (pOccupancy == NULL) {
        return;
    }

    unsigned long currentMillis = millis();

    // Read button states (active LOW with INPUT_PULLUP)
    int entryReading = digitalRead(ENTRY_BUTTON_PIN);
    int exitReading  = digitalRead(EXIT_BUTTON_PIN);

    // Entry button press detection with millis() debounce
    if (entryReading == LOW && prevEntryState == HIGH) {
        if ((currentMillis - lastEntryPressTime) >= DEBOUNCE_DELAY) {
            handleEntry(pOccupancy);
            lastEntryPressTime = currentMillis;
        }
    }
    prevEntryState = entryReading;

    // Exit button press detection with millis() debounce
    if (exitReading == LOW && prevExitState == HIGH) {
        if ((currentMillis - lastExitPressTime) >= DEBOUNCE_DELAY) {
            handleExit(pOccupancy);
            lastExitPressTime = currentMillis;
        }
    }
    prevExitState = exitReading;

    // Refresh LCD every 500 ms without blocking
    if ((currentMillis - lastLCDUpdateTime) >= LCD_UPDATE_INTERVAL) {
        updateLCD(pOccupancy);
        lastLCDUpdateTime = currentMillis;
    }

    // Continuously update LED indicators
    updateLEDs(pOccupancy);
}

// Initialize GPIO pins, I2C LCD, Serial, and allocate heap memory
void initSystem(void) {
    // Configure buttons with internal pull-up resistors
    pinMode(ENTRY_BUTTON_PIN, INPUT_PULLUP);
    pinMode(EXIT_BUTTON_PIN,  INPUT_PULLUP);

    // Configure LED output pins
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN,   OUTPUT);

    // Start Serial communication
    Serial.begin(SERIAL_BAUD_RATE);

    // Initialize I2C bus and LCD screen
    Wire.begin();
    lcd.init();
    lcd.backlight();

    // Dynamically allocate memory for OccupancyInfo struct
    pOccupancy = (OccupancyInfo*)malloc(sizeof(OccupancyInfo));

    // Validate memory allocation
    if (pOccupancy == NULL) {
        Serial.println(F("ERROR: malloc() failed — unable to allocate memory!"));
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MEMORY ERROR!");
        lcd.setCursor(0, 1);
        lcd.print("System halted.");
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, HIGH);
        return;
    }

    // Initialize structure fields via pointer
    pOccupancy->currentCount  = 0;
    pOccupancy->maxCapacity   = MAX_CAPACITY;
    pOccupancy->totalEntries  = 0;
    pOccupancy->totalExits    = 0;

    // Print startup message to Serial Monitor
    Serial.println(F("============================================================"));
    Serial.println(F("  Smart Occupancy Monitoring and Access Control System (I2C)"));
    Serial.println(F("============================================================"));
    Serial.print(F("  Max Capacity : "));
    Serial.println(MAX_CAPACITY);
    Serial.print(F("  System Start : "));
    Serial.println(getTimestamp());
    Serial.println(F("============================================================"));
    Serial.println();

    // Initial display and LED update
    updateLCD(pOccupancy);
    updateLEDs(pOccupancy);
}

// Process entry event: increment count if below max capacity
void handleEntry(OccupancyInfo* pOcc) {
    if (pOcc->currentCount < pOcc->maxCapacity) {
        pOcc->currentCount++;
        pOcc->totalEntries++;
        printStatus(pOcc, "ENTRY");
    } else {
        // Reject entry when facility is at maximum capacity
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

// Process exit event: decrement count if above zero
void handleExit(OccupancyInfo* pOcc) {
    if (pOcc->currentCount > 0) {
        pOcc->currentCount--;
        pOcc->totalExits++;
        printStatus(pOcc, "EXIT");
    } else {
        // Reject exit when room is already empty
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

// Update 16x2 I2C LCD with current people count and available capacity
void updateLCD(OccupancyInfo* pOcc) {
    int available = pOcc->maxCapacity - pOcc->currentCount;

    // Line 1: current occupancy count
    lcd.setCursor(0, 0);
    lcd.print("People:    ");
    lcd.setCursor(8, 0);
    lcd.print(pOcc->currentCount);
    lcd.print("   ");

    // Line 2: remaining available capacity
    lcd.setCursor(0, 1);
    lcd.print("Available: ");
    lcd.setCursor(11, 1);
    lcd.print(available);
    lcd.print("   ");
}

// Update LEDs: Green when capacity available, Red when at maximum capacity
void updateLEDs(OccupancyInfo* pOcc) {
    if (pOcc->currentCount >= pOcc->maxCapacity) {
        // Room full: Green OFF, Red ON
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN,   HIGH);
    } else {
        // Room has space: Green ON, Red OFF
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN,   LOW);
    }
}

// Print formatted event log to Serial Monitor
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

// Print formatted error message to Serial Monitor
void printError(const char* errorMsg) {
    Serial.print("[");
    Serial.print(getTimestamp());
    Serial.print("] ERROR: ");
    Serial.println(errorMsg);
}

// Calculate elapsed time since system boot formatted as HH:MM:SS
String getTimestamp(void) {
    unsigned long totalSeconds = millis() / 1000;
    unsigned long hours   = totalSeconds / 3600;
    unsigned long minutes = (totalSeconds % 3600) / 60;
    unsigned long seconds = totalSeconds % 60;

    char buffer[12];
    sprintf(buffer, "%02lu:%02lu:%02lu", hours, minutes, seconds);
    return String(buffer);
}

// Free dynamically allocated occupancy structure from heap
void cleanupSystem(void) {
    if (pOccupancy != NULL) {
        free(pOccupancy);
        pOccupancy = NULL;
        Serial.println(F("System cleanup complete — memory freed."));
    }
}
