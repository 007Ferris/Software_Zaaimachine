# Zaaimachine — ESP32 Seeding Machine

An ESP32-based control system for an automated seeding machine ("zaaimachine"), running on **FreeRTOS** and written in **C++**. The firmware targets the NodeMCU32S platform board and drives the machine's motor, actuators, sensors and CAN communication.

---

## Overview

The machine places seeds in a controlled, repeatable way. An ESP32 acts as the central controller: it reads position feedback from a quadrature encoder, detects seeds and objects with three sensors, drives the seeding actuators through a motor driver, and talks to the rest of the installation over a CAN bus.

The codebase is organised as a set of reusable hardware-driver libraries (one per peripheral) on top of which the seeding application logic and its FreeRTOS tasks are built.

---

## Hardware

### Controller

- **ESP32** (NodeMCU32S module) — central controller, runs the FreeRTOS firmware.

### Power supply chain

The machine is fed from a 44 V supply that also carries CAN, and steps the voltage down for the various subsystems:

| Stage | Input | Output |
|-------|-------|--------|
| `44TO24V_1` / `44TO24V_2` | 44 V | 24 V |
| `24TO12V` | 24 V | 12 V |

Resulting rails distributed through the system: **24 V**, **12 V**, **5 V**, **3V3** and **GND**, plus the **CAN** line.

### Communication

- **CAN bus** via a `TTL_TO_CAN` converter (ESP32 `RX`/`TX` ↔ `CAN_H`/`CAN_L`).
- **I²C-to-UART** bridge using the **SC16IS740** (I²C address `0x4D`) for an additional serial channel.

### Actuators & motion

- **Motor driver** — driven by the ESP32 digital outputs (`ENA`, `IN1`, `IN2`), powered from 24 V, produces `OUT+ / OUT-` to the motor.
- **Linear actuator** and a **BLDC motor** as the machine's actuators.
- **Quadrature counter** (LS7366R) reading the motor/encoder (`Encoder_H` / `Encoder_L`) for position feedback.

### Sensors

| Sensor | Signal | Type |
|--------|--------|------|
| Capacitive | `SEN_CAP` | Object/seed detection |
| Optical | `SEN_OPT` | Object/seed detection |
| Infrared | `SEN_IR` | Object/seed detection |

Sensor cabling follows the M12 connector convention (BN = VCC, WH = NC, BU = GND, BK = NO).

### On-board platform peripherals (NodeMCU32S board)

The firmware also makes use of the peripherals on the NodeMCU32S platform board:

- **OLED display** — SSD1306 128×64 over I²C (address `0x3C`).
- **8-channel ADC** — MCP3208 (channels 0–3: −10 V…+10 V, channels 4–7: 0…2.5 V; analog buttons on channels 6 & 7).
- **4-channel DAC** — 2× MCP4922 with output amplifiers (`Vout = -10 + 8 × Vdac`, for `0 ≤ Vdac ≤ 2.5`).
- **6-bit digital input / 6-bit digital output** with level shifting.
- **EEPROM** — both I²C (24LC64) and SPI (25LC640) variants supported.
- **LEDs** — on-board blue LED and red LED (GPIO15).
- **SPI device selection** via a 74HC138 multiplexer (one chip-select line per device).

---

## Software

### Requirements

- **Arduino core for ESP32** (FreeRTOS is included with the ESP32 Arduino core).
- C++ toolchain (Arduino IDE or PlatformIO).
- Third-party libraries: `SSD1306Wire` (OLED) and an `I2C_eeprom` library.

### Library structure

Each peripheral has its own driver. Most use a C-style `prefix_Function()` API; the newer drivers (`QC7366`, `SPIeeprom`) are C++ classes.

| File | Responsibility |
|------|----------------|
| `SPILib` | VSPI bus + 74HC138 device selection (DAC01, DAC23, QC0, QC1, ADC, EXT_5, EXT_6) |
| `I2CLib` | I²C init and bus-lockup recovery |
| `I2CScanner` | Scan the I²C bus and report device addresses |
| `IOLib` | 6-bit digital input / output |
| `LEDLib` | Blue and red status LEDs |
| `ButtonLib` | Button handling |
| `InterruptLib` | Interrupt handling |
| `ADC3208Lib` | MCP3208 8-channel ADC (raw + voltage, analog buttons) |
| `DAC4922Lib` | MCP4922 4-channel DAC |
| `QC7366Lib` / `QC7366` | LS7366R quadrature counter (function-style and class-style) |
| `UART740Lib` | SC16IS740 I²C-to-UART bridge |
| `EepromLib` / `SPIeeprom` | I²C and SPI EEPROM access |
| `OLEDLibESP32` | SSD1306 OLED display |
| `TaskSleep` | `taskSleep(ms)` wrapper around `vTaskDelay` |
| `InfoRTOS` | FreeRTOS task list, CPU info and version reporting |

### Key configuration

Board-specific values (I²C addresses, EEPROM type, etc.) are defined in `Config.h`.

**Pin mapping (ESP32):**

- I²C: SDA = GPIO21, SCL = GPIO22
- SPI (VSPI): MISO = GPIO19, MOSI = GPIO23, SCLK = GPIO18; chip-select via MUX bits GPIO16/17/5
- Digital inputs: GPIO36, 39, 34, 35, 32, 33
- Digital outputs: GPIO25, 26, 27, 14, 12, 13
- Red LED: GPIO15

### FreeRTOS

The application runs as a set of FreeRTOS tasks. `InfoRTOS` provides runtime diagnostics:

- `info_Tasks()` — lists registered tasks with their state and stack high-water mark.
- `info_CPU()` — chip model, core count, clock, heap and sketch info.
- `info_Version()` — build timestamp, ESP32 SDK and FreeRTOS version.

Tasks can be registered for tracking with `info_RegisterTaskByName()`.

---

## Getting started

1. Install the Arduino ESP32 core (or set up the PlatformIO ESP32 environment).
2. Install the `SSD1306Wire` and `I2C_eeprom` libraries.
3. Set the correct addresses and options in `Config.h`.
4. Build and flash to the NodeMCU32S / ESP32.
5. Use `info_Version()` / `info_CPU()` on the serial monitor to confirm the board is running, and `i2c_ScanBus()` to verify the I²C devices (OLED, UART, EEPROM) are detected.

---

## Project info

- **Project:** ElektrischeTekening_Zaaimachine
- **Platform:** NodeMCU32S Platform v2
- **Driver libraries author:** Roel Smeets
- **Language:** C++ · **RTOS:** FreeRTOS · **MCU:** ESP32
