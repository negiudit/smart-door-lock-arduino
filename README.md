# IoT Smart Door Lock System

A keypad-controlled electronic door lock built on Arduino Uno.
Enter a 4-digit PIN to control a servo motor (lock/unlock) with LED status indication.
Password is stored persistently in EEPROM.

## Features
- 4-digit PIN entry via 4×4 matrix keypad
- Servo motor as physical lock (0° = locked, 90° = unlocked)
- LED visual status indicator
- EEPROM password storage (survives power cycles)
- Auto-relock after 5 seconds
- 3-attempt lockout (30 second freeze) to prevent brute-force

## Hardware Used
Component - Purpose
Arduino Uno - Main microcontroller
4×4 Matrix Keypad - PIN input
SG90 Servo Motor - Lock mechanism
LED + 220Ω Resistor - Status indicator
EEPROM (built-in) - Password storage

## How to Use
1. Default PIN is '1234'
2. Enter digits → press '#' to submit
3. Press '*' to clear input
4. Press 'A' to change password

## Project Files
- 'src/' — Arduino source code
- 'docs/' — Circuit diagram, flow diagram, project report
- 'presentation/' — Internship presentation slides

## Built With
Arduino IDE · C++ · Tinkercad (simulation)
