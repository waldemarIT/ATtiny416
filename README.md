# ATtiny416
This embedded project demonstrates a low-power system implemented on the ATtiny416 microcontroller using C. The device cycles through three operational states (Sleep, Active, Monitoring) depending on button input and sensor readings, optimizing energy consumption while maintaining responsive behavior.
🔋 Power-Efficient Finite State Machine with ATtiny416
This embedded project demonstrates a low-power system implemented on the ATtiny416 microcontroller using C. The device cycles through three operational states (Sleep, Active, Monitoring) depending on button input and sensor readings, optimizing energy consumption while maintaining responsive behavior.

💡 Features
Finite State Machine (FSM) with 3 distinct modes:

STATE_SLEEP: deep power-down mode with minimal consumption

STATE_2: active LED blinking with varying patterns depending on sensor input

STATE_3: monitoring mode with timed LED feedback and voltage reading

I/O Handling using GPIOs for LEDs and pushbuttons

Power Management with sleep modes (IDLE and POWER-DOWN)

Timer-based Interrupts for non-blocking execution and debouncing

Voltage Monitoring using the built-in ADC

Sensor Input emulated via button state or pin logic level

⚙️ Tech Details
Microcontroller: ATtiny416 (3.33 MHz internal clock)

Written in pure C with avr-gcc

Uses:

ISR() to handle timer-based state updates

Sleep library from <avr/sleep.h>

Low-level register manipulation for full control

📦 Possible Applications
Low-power wearable sensor logic

Battery-powered automation controller

Educational FSM demo on ATtiny series

📁 File Structure
main.c: main project logic (FSM, setup, sleep handling)
