# Smart Occupancy Monitoring and Access Control System (I2C LCD)

## Project Description

An Arduino-based room occupancy monitoring system that tracks the number of people entering and exiting a room in real time. The system enforces a maximum capacity limit, provides visual feedback through LEDs and a **16×2 LCD with I2C module**, and logs all events to the Serial Monitor. Designed for simulation in **Tinkercad**.

## How the System Works

1. **Entry Detection** — Pressing the **Entry button** (pin 2) increments the occupancy count if the room is not at maximum capacity. If the room is full, the entry is rejected and an error is logged to the Serial Monitor.
2. **Exit Detection** — Pressing the **Exit button** (pin 3) decrements the occupancy count if the room is not empty. If the room is already empty, the exit is rejected and an error is logged.
3. **I2C LCD Display** — A 16×2 LCD with I2C backpack continuously shows the current number of people and remaining available spots, refreshed every 500 ms using `millis()`.
4. **LED Indicators** — A **green LED** lights up when capacity is available; a **red LED** lights up when the room reaches maximum capacity.
5. **Serial Logging** — Every entry/exit event is logged with a timestamp, current count, available spots, and cumulative entry/exit totals.

> The system uses **no `delay()` calls**. All timing (button debounce, LCD refresh) is handled with `millis()` for a fully non-blocking design.

## Component List

| # | Component                         | Quantity | Notes                                               |
|---|-----------------------------------|----------|-----------------------------------------------------|
| 1 | Arduino Uno                       | 1        | Main microcontroller                                |
| 2 | 16×2 LCD with I2C Backpack        | 1        | PCF8574 I2C interface (address `0x27` / `0x20` / `0x3F`) |
| 3 | Push Button                       | 2        | One for entry, one for exit                         |
| 4 | Green LED                         | 1        | Indicates capacity available                        |
| 5 | Red LED                           | 1        | Indicates room at max capacity                      |
| 6 | 220 Ω Resistor                    | 2        | Current-limiting resistors for LEDs                 |
| 7 | Breadboard                        | 1        | For prototyping connections                         |
| 8 | Jumper Wires                      | Various  | For all connections                                 |

## Pin Assignments

| Arduino Pin | Component                | Function / Connection                         |
|-------------|--------------------------|-----------------------------------------------|
| Pin 2       | Entry Push Button        | Digital input (INPUT_PULLUP, other pin to GND)|
| Pin 3       | Exit Push Button         | Digital input (INPUT_PULLUP, other pin to GND)|
| Pin 8       | Green LED (anode)        | Digital output (through 220 Ω to GND)         |
| Pin 9       | Red LED (anode)          | Digital output (through 220 Ω to GND)         |
| Pin A4      | LCD SDA (Serial Data)    | I2C Data line                                 |
| Pin A5      | LCD SCL (Serial Clock)   | I2C Clock line                                |
| 5V          | LCD VCC                  | Power rail (5V)                               |
| GND         | LCD GND, Buttons, LEDs   | Common ground                                 |

## Wiring Diagram

```
+---------------------------------------------------------------------+
|                        ARDUINO UNO                                  |
|                                                                     |
|  Digital Pins:                                                      |
|  +---------+----------------------------------------------------+   |
|  | Pin  2  | -----> Entry Button -----> GND                     |   |
|  | Pin  3  | -----> Exit Button  -----> GND                     |   |
|  | Pin  8  | -----> 220Ω Resistor -----> Green LED -----> GND   |   |
|  | Pin  9  | -----> 220Ω Resistor -----> Red LED   -----> GND   |   |
|  +---------+----------------------------------------------------+   |
|                                                                     |
|  Analog / I2C Pins:                                                 |
|  +---------+----------------------------------------------------+   |
|  | Pin A4  | -----> LCD SDA (Serial Data)                       |   |
|  | Pin A5  | -----> LCD SCL (Serial Clock)                      |   |
|  +---------+----------------------------------------------------+   |
|                                                                     |
|  Power Pins:                                                        |
|  +---------+----------------------------------------------------+   |
|  | 5V      | -----> LCD VCC (Power Rail)                        |   |
|  | GND     | -----> LCD GND, LED Cathodes, Button Terminals     |   |
|  +---------+----------------------------------------------------+   |
|                                                                     |
|  16x2 I2C LCD Module (4-Wire Interface):                            |
|  +---------+----------------------------------------------------+   |
|  | GND     | -----> Arduino GND                                 |   |
|  | VCC     | -----> Arduino 5V                                  |   |
|  | SDA     | -----> Arduino Pin A4 (SDA)                        |   |
|  | SCL     | -----> Arduino Pin A5 (SCL)                        |   |
|  +---------+----------------------------------------------------+   |
+---------------------------------------------------------------------+

Button Wiring Detail (using internal pull-up resistors):

    Arduino Pin 2/3
         |
         +---[ Button ]--- GND
         |
    (INPUT_PULLUP: HIGH when open, LOW when pressed)
```

## Features

- **I2C communication** — reduces LCD wiring to just 4 wires (VCC, GND, SDA, SCL)
- **Real-time occupancy tracking** with entry and exit counters
- **Dynamic memory allocation** (`malloc` / `free`) for the occupancy data struct
- **Pointer-based data access** throughout the codebase
- **Non-blocking design** — all timing via `millis()`, zero `delay()` calls
- **Button debouncing** using `millis()` with 200 ms debounce window
- **Boundary enforcement** — prevents count going below 0 or above maximum capacity
- **Visual indicators** — green/red LEDs for instant capacity status
- **I2C LCD display** — shows current people count and available spots, refreshed every 500 ms
- **Serial Monitor logging** — timestamped event log with full statistics
- **Error reporting** — clear error messages for invalid entry/exit attempts
- **Modular code** — well-structured functions (`initSystem`, `handleEntry`, `handleExit`, `updateLCD`, `updateLEDs`, `printStatus`, etc.)
- **Configurable capacity** — `MAX_CAPACITY` defined as a preprocessor constant (default: 10)

## How to Simulate in Tinkercad

1. Go to [tinkercad.com](https://www.tinkercad.com) and sign in.
2. Create a new **Circuit** project.
3. Drag an **Arduino Uno** onto the workspace.
4. Search components for **"LCD 16 x 2 (I2C)"** and drag it onto the canvas.
5. Add the remaining components: 2 push buttons, 2 LEDs (green and red), and 2 × 220 Ω resistors.
6. Connect the I2C LCD:
   - **GND** pin of LCD to Arduino **GND**
   - **VCC** pin of LCD to Arduino **5V**
   - **SDA** pin of LCD to Arduino **A4**
   - **SCL** pin of LCD to Arduino **A5**
7. Connect the push buttons (one terminal to Pin 2/3, other terminal to GND).
8. Connect the LEDs (anodes through 220 Ω resistors to Pin 8/9, cathodes to GND).
9. Click **Code**, select **Text** mode, and paste `occupancy_monitor.ino`.
10. Click **Start Simulation**.
11. Open the **Serial Monitor** at the bottom to observe real-time status and error reports.

## Project Structure

```
project1_occupancy_monitor/
├── occupancy_monitor.ino   # Arduino sketch (LiquidCrystal_I2C, millis, malloc/free)
└── README.md               # Documentation & wiring diagram
```
