// Smart Occupancy Monitoring and Access Control System (Tinkercad version)
// Author: Nkem Jeferson Achia
// Uses Adafruit_LiquidCrystal, the library Tinkercad provides for its I2C LCD.

#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>
#include <stdlib.h>

// Pin definitions
#define ENTRY_BUTTON_PIN  2   // Push button for entry
#define EXIT_BUTTON_PIN   3   // Push button for exit
#define GREEN_LED_PIN     8   // Green LED: capacity available
#define RED_LED_PIN       9   // Red LED: at maximum capacity

// LCD configuration (Tinkercad I2C LCD: SDA = A4, SCL = A5, default address 0x20)
#define LCD_COLS          16
#define LCD_ROWS          2

// System constants
#define MAX_CAPACITY        10
#define DEBOUNCE_DELAY     200
#define LCD_UPDATE_INTERVAL 500
#define SERIAL_BAUD_RATE  9600

// Structure to organize occupancy data
typedef struct {
    int currentCount;
    int maxCapacity;
    unsigned long totalEntries;
    unsigned long totalExits;
} OccupancyInfo;

// Argument 0 = I2C address offset (address 0x20 + 0)
Adafruit_LiquidCrystal lcd(0);

OccupancyInfo* pOccupancy = NULL;

unsigned long lastEntryPressTime = 0;
unsigned long lastExitPressTime  = 0;
unsigned long lastLCDUpdateTime  = 0;

int prevEntryState = HIGH;
int prevExitState  = HIGH;

void initSystem(void);
void handleEntry(OccupancyInfo* pOcc);
void handleExit(OccupancyInfo* pOcc);
void updateLCD(OccupancyInfo* pOcc);
void updateLEDs(OccupancyInfo* pOcc);
void printStatus(OccupancyInfo* pOcc, const char* event);
void printError(const char* errorMsg);
void cleanupSystem(void);
String getTimestamp(void);

void setup() {
    initSystem();
}

void loop() {
    if (pOccupancy == NULL) {
        return;
    }

    unsigned long currentMillis = millis();

    int entryReading = digitalRead(ENTRY_BUTTON_PIN);
    int exitReading  = digitalRead(EXIT_BUTTON_PIN);

    if (entryReading == LOW && prevEntryState == HIGH) {
        if ((currentMillis - lastEntryPressTime) >= DEBOUNCE_DELAY) {
            handleEntry(pOccupancy);
            lastEntryPressTime = currentMillis;
        }
    }
    prevEntryState = entryReading;

    if (exitReading == LOW && prevExitState == HIGH) {
        if ((currentMillis - lastExitPressTime) >= DEBOUNCE_DELAY) {
            handleExit(pOccupancy);
            lastExitPressTime = currentMillis;
        }
    }
    prevExitState = exitReading;

    if ((currentMillis - lastLCDUpdateTime) >= LCD_UPDATE_INTERVAL) {
        updateLCD(pOccupancy);
        lastLCDUpdateTime = currentMillis;
    }

    updateLEDs(pOccupancy);
}

// Initialize pins, LCD, Serial, and allocate heap memory
void initSystem(void) {
    pinMode(ENTRY_BUTTON_PIN, INPUT_PULLUP);
    pinMode(EXIT_BUTTON_PIN,  INPUT_PULLUP);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN,   OUTPUT);

    Serial.begin(SERIAL_BAUD_RATE);

    lcd.begin(LCD_COLS, LCD_ROWS);
    lcd.setBacklight(1);

    pOccupancy = (OccupancyInfo*)malloc(sizeof(OccupancyInfo));

    if (pOccupancy == NULL) {
        Serial.println(F("ERROR: malloc() failed - unable to allocate memory!"));
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MEMORY ERROR!");
        lcd.setCursor(0, 1);
        lcd.print("System halted.");
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, HIGH);
        return;
    }

    pOccupancy->currentCount  = 0;
    pOccupancy->maxCapacity   = MAX_CAPACITY;
    pOccupancy->totalEntries  = 0;
    pOccupancy->totalExits    = 0;

    Serial.println(F("============================================================"));
    Serial.println(F("  Smart Occupancy Monitoring and Access Control System"));
    Serial.println(F("============================================================"));
    Serial.print(F("  Max Capacity : "));
    Serial.println(MAX_CAPACITY);
    Serial.print(F("  System Start : "));
    Serial.println(getTimestamp());
    Serial.println(F("============================================================"));
    Serial.println();

    lcd.clear();
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
        Serial.print("[");
        Serial.print(getTimestamp());
        Serial.print("] ");
        Serial.println(F("ERROR: Entry REJECTED - room is at MAXIMUM CAPACITY!"));
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
        Serial.print("[");
        Serial.print(getTimestamp());
        Serial.print("] ");
        Serial.println(F("ERROR: Exit REJECTED - room is already EMPTY!"));
        Serial.print(F("       Current count: "));
        Serial.print(pOcc->currentCount);
        Serial.print(F("/"));
        Serial.println(pOcc->maxCapacity);
        Serial.println();
    }
}

// Update the LCD with current people count and available capacity
void updateLCD(OccupancyInfo* pOcc) {
    int available = pOcc->maxCapacity - pOcc->currentCount;

    lcd.setCursor(0, 0);
    lcd.print("People:    ");
    lcd.setCursor(8, 0);
    lcd.print(pOcc->currentCount);
    lcd.print("   ");

    lcd.setCursor(0, 1);
    lcd.print("Available: ");
    lcd.setCursor(11, 1);
    lcd.print(available);
    lcd.print("   ");
}

// Green when capacity is available, red when at maximum capacity
void updateLEDs(OccupancyInfo* pOcc) {
    if (pOcc->currentCount >= pOcc->maxCapacity) {
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN,   HIGH);
    } else {
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
    Serial.print(F(" - Count: "));
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

// Elapsed time since boot formatted as HH:MM:SS
String getTimestamp(void) {
    unsigned long totalSeconds = millis() / 1000;
    unsigned long hours   = totalSeconds / 3600;
    unsigned long minutes = (totalSeconds % 3600) / 60;
    unsigned long seconds = totalSeconds % 60;

    char buffer[12];
    sprintf(buffer, "%02lu:%02lu:%02lu", hours, minutes, seconds);
    return String(buffer);
}

// Free the dynamically allocated occupancy structure
void cleanupSystem(void) {
    if (pOccupancy != NULL) {
        free(pOccupancy);
        pOccupancy = NULL;
        Serial.println(F("System cleanup complete - memory freed."));
    }
}
