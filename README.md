# Microprocessors-Final-Project
Microprocessors Final Project: Reflex 
&amp; Memory Testing System
# Reflex & Memory Testing System

An Arduino-based embedded system for measuring human reaction time and sequence memory performance.

## Project Overview

The Reflex & Memory Testing System is a microprocessor-based prototype built using an Arduino UNO R4 WiFi, an SH1106 OLED display, arcade-style push buttons, and LEDs. The system provides two interactive test modes:

1. **Reaction Test** — Measures how quickly a user responds to a random LED stimulus.
2. **Sequence Memory Game** — Displays a random LED sequence that the user must memorize and repeat using buttons.

The system includes an OLED menu interface, difficulty selection, button input handling, LED feedback, and millisecond-level timing using `millis()`.

## Features

- Menu-driven OLED interface
- Reaction time measurement in milliseconds
- Sequence memory game similar to Simon Says
- Easy, Medium, and Hard difficulty levels
- LED-based visual stimuli
- Arcade push-button controls
- Early press and wrong button detection
- Average reaction time and best time tracking
- SH1106 OLED support using the U8g2 library

## Hardware Components

| Component | Description |
|---|---|
| Arduino UNO R4 WiFi | Main microcontroller |
| SH1106 OLED Display | 128x64 I2C display |
| Arcade Push Buttons | User input controls |
| Red, Blue, Green LEDs | Visual stimuli |
| Resistors | LED current limiting |
| Breadboard | Circuit prototyping |
| Jumper Wires | Electrical connections |

## Pin Configuration

| Component | Arduino Pin |
|---|---|
| Red Button | D2 |
| Blue Button | D4 |
| Green Button | D6 |
| Red LED | D8 |
| Blue LED | D9 |
| Green LED | D10 |
| OLED SDA | SDA / A4 |
| OLED SCL | SCL / A5 |
| OLED VCC | 3.3V or 5V depending on module |
| OLED GND | GND |

> Note: The physical prototype used an Arduino UNO R4 WiFi. In simulation diagrams, the regular Arduino UNO may be used as a visual representation.

## Required Libraries

Install the following libraries in Arduino IDE:

- `Wire.h` — included with Arduino IDE
- `U8g2` by Oliver Kraus

To install U8g2:

1. Open Arduino IDE.
2. Go to **Tools > Manage Libraries**.
3. Search for **U8g2**.
4. Install **U8g2 by oliver**.

## How to Run the Project

1. Connect the circuit according to the pin configuration table.
2. Open `ReflexMemoryTestingSystem.ino` in Arduino IDE.
3. Select the correct board:
   - **Arduino UNO R4 WiFi**
4. Select the correct port.
5. Upload the code.
6. Use the OLED menu to choose:
   - Reaction Test
   - Sequence Game
   - Difficulty

## Controls

| Button | Function |
|---|---|
| Green Button | Scroll up/down depending on menu logic |
| Blue Button | Scroll up/down depending on menu logic |
| Red Button | Select option / confirm |

During the games, each button corresponds to its matching LED color.

## Game Modes

### Reaction Test

The reaction test waits a random amount of time, turns on a random LED, and measures the time between the LED turning on and the user pressing the correct button.

The system detects:
- Correct responses
- Wrong button presses
- Early presses
- Average reaction time
- Best reaction time

### Sequence Memory Game

The sequence game displays a random LED pattern. The user must repeat the pattern using the buttons. Each successful round increases the sequence length.

Difficulty affects:
- Sequence speed
- Number of rounds
- Reaction timing limits

## Problems Solved During Development

Several issues were solved during development:

- SH1106 OLED was not compatible with SSD1306-only code.
- OLED text corruption was fixed using the U8g2 library.
- I2C communication stability was improved by lowering the bus clock.
- Button bounce was handled using debounce logic.
- LED stimuli were used instead of OLED-only prompts to improve timing accuracy.
- Common ground wiring issues were corrected during testing.

## Future Improvements

- Add buzzer sound effects
- Add EEPROM or cloud-based score saving
- Add WiFi leaderboard using Arduino UNO R4 WiFi
- Add more buttons and colors
- Build a stronger enclosure
- Add a math-based multiple-choice game mode
- Create full Doxygen documentation

## Authors

- Erick Reyes
- Steven Baez

## Course

Microprocessors  
Computer Engineering Undergraduate Project
