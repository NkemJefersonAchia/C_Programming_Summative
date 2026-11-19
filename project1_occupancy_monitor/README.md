# Smart Occupancy Monitoring and Access Control System

## Project Description

An Arduino-based room occupancy monitoring system that tracks the number of people entering and exiting a room in real time. The system enforces a maximum capacity limit, provides visual feedback through LEDs and a 16×2 LCD display, and logs all events to the Serial Monitor. Designed for simulation in **Tinkercad**.

## How the System Works

1. **Entry Detection** — Pressing the **Entry button** (pin 2) increments the occupancy count if the room is not at maximum capacity. If the room is full, the entry is rejected and an error is logged to the Serial Monitor.
2. **Exit Detection** — Pressing the **Exit button** (pin 3) decrements the occupancy count if the room is not empty. If the room is already empty, the exit is rejected and an error is logged.
3. **LCD Display** — A 16×2 LCD continuously shows the current number of people and the remaining available spots, refreshed every 500 ms using `millis()`.
4. **LED Indicators** — A **green LED** lights up when capacity is available; a **red LED** lights up when the room reaches maximum capacity.
5. **Serial Logging** — Every entry/exit event is logged with a timestamp, current count, available spots, and cumulative entry/exit totals.

> The system uses **no `delay()` calls**. All timing (button debounce, LCD refresh) is handled with `millis()` for a fully non-blocking design.

## Component List

| # | Component             | Quantity | Notes                                   |
|---|-----------------------|----------|-----------------------------------------|
| 1 | Arduino Uno           | 1        | Main microcontroller                    |
| 2 | 16×2 LCD Display      | 1        | HD44780-compatible, 4-bit mode          |
| 3 | Push Button           | 2        | One for entry, one for exit             |
| 4 | Green LED             | 1        | Indicates capacity available            |
| 5 | Red LED               | 1        | Indicates room at max capacity          |
| 6 | 220 Ω Resistor        | 2        | Current-limiting resistors for LEDs     |
| 7 | 10 kΩ Potentiometer   | 1        | LCD contrast adjustment (V0 pin)        |
| 8 | Breadboard            | 1        | For prototyping connections              |
| 9 | Jumper Wires          | Various  | For all connections                     |

## Pin Assignments

| Arduino Pin | Component           | Function                |
|-------------|---------------------|-------------------------|
| 2           | Entry Push Button   | Digital input (INPUT_PULLUP) |
| 3           | Exit Push Button    | Digital input (INPUT_PULLUP) |
| 4           | LCD D7              | Data bit 7              |
| 5           | LCD D6              | Data bit 6              |
| 6           | LCD D5              | Data bit 5              |
| 7           | LCD D4              | Data bit 4              |
| 8           | Green LED (anode)   | Digital output           |
| 9           | Red LED (anode)     | Digital output           |
| 11          | LCD EN              | Enable signal            |
| 12          | LCD RS              | Register Select          |
| 5V          | LCD VDD, Pot end    | Power                   |
| GND         | LCD VSS, LEDs cathode, Buttons, Pot end | Ground |

## Wiring Diagram

```
+---------------------------------------------------------------------+
|                        ARDUINO UNO                                  |
|                                                                     |
|  Digital Pins:                                                      |
|  +---------+----------------------------------------------------+   |
|  | Pin  2  | -----> Entry Button -----> GND                     |   |
|  | Pin  3  | -----> Exit Button  -----> GND                     |   |
|  | Pin  4  | -----> LCD D7                                      |   |
|  | Pin  5  | -----> LCD D6                                      |   |
|  | Pin  6  | -----> LCD D5                                      |   |
|  | Pin  7  | -----> LCD D4                                      |   |
|  | Pin  8  | -----> 220Ω Resistor -----> Green LED -----> GND   |   |
|  | Pin  9  | -----> 220Ω Resistor -----> Red LED   -----> GND   |   |
|  | Pin 11  | -----> LCD EN (Enable)                             |   |
|  | Pin 12  | -----> LCD RS (Register Select)                    |   |
|  +---------+----------------------------------------------------+   |
|                                                                     |
|  Power Pins:                                                        |
|  +---------+----------------------------------------------------+   |
|  | 5V      | -----> LCD VDD (pin 2), Potentiometer end          |   |
|  | GND     | -----> LCD VSS (pin 1), LCD RW (pin 5),           |   |
|  |         |        LED cathodes, Button other terminals,       |   |
|  |         |        Potentiometer other end                     |   |
|  +---------+----------------------------------------------------+   |
|                                                                     |
|  LCD 16x2 Connections:                                              |
|  +---------+----------------------------------------------------+   |
|  | LCD  1  | VSS -----> GND                                     |   |
|  | LCD  2  | VDD -----> 5V                                      |   |
|  | LCD  3  | V0  -----> Potentiometer wiper (contrast)          |   |
|  | LCD  4  | RS  -----> Arduino Pin 12                          |   |
|  | LCD  5  | RW  -----> GND (write mode)                        |   |
|  | LCD  6  | EN  -----> Arduino Pin 11                          |   |
|  | LCD 11  | D4  -----> Arduino Pin 7                           |   |
|  | LCD 12  | D5  -----> Arduino Pin 6                           |   |
|  | LCD 13  | D6  -----> Arduino Pin 5                           |   |
|  | LCD 14  | D7  -----> Arduino Pin 4                           |   |
|  | LCD 15  | A   -----> 5V (backlight anode)                    |   |
|  | LCD 16  | K   -----> GND (backlight cathode)                 |   |
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

- **Real-time occupancy tracking** with entry and exit counters
- **Dynamic memory allocation** (`malloc` / `free`) for the occupancy data struct
- **Pointer-based data access** throughout the codebase
- **Non-blocking design** — all timing via `millis()`, zero `delay()` calls
- **Button debouncing** using `millis()` with 200 ms debounce window
- **Boundary enforcement** — prevents count going below 0 or above maximum capacity
- **Visual indicators** — green/red LEDs for instant capacity status
- **LCD display** — shows current people count and available spots, refreshed every 500 ms
- **Serial Monitor logging** — timestamped event log with full statistics
- **Error reporting** — clear error messages for invalid entry/exit attempts
- **Modular code** — well-documented functions (`initSystem`, `handleEntry`, `handleExit`, `updateLCD`, `updateLEDs`, `printStatus`, etc.)
- **Configurable capacity** — `MAX_CAPACITY` defined as a preprocessor constant (default: 10)

## How to Simulate in Tinkercad

1. Go to [tinkercad.com](https://www.tinkercad.com) and sign in.
2. Create a new **Circuit** project.
3. Drag an **Arduino Uno** onto the workspace.
4. Add the components listed above: 16×2 LCD, 2 push buttons, 2 LEDs, 2 × 220 Ω resistors, and a 10 kΩ potentiometer.
5. Wire everything according to the **Pin Assignments** and **Wiring Diagram** sections above.
6. Click on the Arduino, select **Code Editor**, switch to **Text** mode, and paste the contents of `occupancy_monitor.ino`.
7. Click **Start Simulation**.
8. Open the **Serial Monitor** (bottom of the screen) to view event logs.
9. Click the **Entry button** to simulate someone entering — watch the LCD and LEDs update.
10. Click the **Exit button** to simulate someone leaving.
11. Try pressing Entry when the room is full (10 people) or Exit when empty to see error messages.

## Project Structure

```
project1_occupancy_monitor/
├── occupancy_monitor.ino   # Main Arduino sketch
└── README.md               # This documentation file
```
