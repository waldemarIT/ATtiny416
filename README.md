# ATtiny416 Boozer Controller

This project implements a low-power embedded control system for a signaling module codenamed **Boozer**, built using an **ATtiny416** microcontroller. The controller features multi-state logic, signal control, analog voltage monitoring, and low-power operation — ideal for IoT or power-sensitive embedded applications.

## Overview

The system consists of:

- **State 1 (Deep Sleep):** Minimal power consumption. Awakens only when PB5 ("Charged") goes high.
- **State 2 (Active):** Reads user input (Button, BZ_FC), powers buzzer/LED with timed patterns, monitors analog input voltage.
- **State 3 (Post-Charge):** Begins countdown cycles with buzzer/LED, performs voltage check after 5 minutes and responds accordingly.

Inputs and Outputs:
| Pin    | Function        | Type         |
|--------|-----------------|--------------|
| PC1    | Button          | Digital (IN) |
| PA3    | Buzzer_ON       | Digital (OUT)|
| PA4    | LED_ON          | Digital (OUT)|
| PB3    | BZ_FC feedback  | Digital (IN) |
| PB4    | Power state     | Digital (IN) |
| PB5    | Charged status  | Digital (IN) |
| PA6    | OPAMP control   | Digital (OUT)|
| PA7    | VoltageCAP      | Analog (IN)  |
| PA5    | Bypass trigger  | Digital (OUT)|
| PA0/1/2| UPDI/UART       | Debug        |

## State Machine

The controller operates across 3 main states:

- **State 1:** Deep power-down until PB5 signal detected
- **State 2:** Activates buzzer/LED with delays and pulse logic; reacts to PB3 state
- **State 3:** After PB5 drops low, executes timing-driven logic: LED sequences, voltage monitoring, and fallback to State 1 on conditions

Voltage is read via ADC from PA7 and interpreted with 0.1 V precision. Timing is driven by hardware timers with ISR logic.

## Tech Specs

- MCU: ATtiny416 @ 20 MHz
- Language: C (AVR-GCC)
- Toolchain: Atmel Studio
- ADC: 10-bit single-ended
- Sleep Modes: Idle, Power-down
- Interrupts: Timer-based ISR
- No external libraries or RTOS used

## How to Build

1. Open the project in **Atmel Studio**
2. Flash to ATtiny416 using UPDI
3. Ensure external hardware matches pin layout
4. Power at 3.3–5 V with necessary peripherals (buzzer, LEDs, buttons)

## Future Improvements

- Integrate accelerometer event logic (via I²C/SPI or analog triggers)
- Add UART debug output
- Fine-tune sleep wake-up with pin change interrupts

## Author

Developed by **Vlad** — my first hands-on embedded C project. Focused on full system control logic, energy management, and real-world signal coordination.

GitHub: [your-link]  
LinkedIn: [your-link]

