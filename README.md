# PS/2 Keyboard Emulator for ESP32

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE.md) ![Work in Progress](https://img.shields.io/badge/work_in_progress-orange)

This project implements a PS/2 keyboard emulator using the ESP32 microcontroller. It allows keystrokes to be sent to a server running on the ESP32, which then outputs the corresponding PS/2 keyboard signals through its GPIO pins. The ESP32 can be connected to any device that accepts PS/2 keyboard input, effectively acting as a physical PS/2 keyboard.

## Key Features

- Emulates a PS/2 keyboard via the ESP32's GPIO pins.
- Receives keystrokes from a server hosted on the ESP32.
- Plug-and-play functionality for devices with PS/2 keyboard ports.

## Setup and Development Environment

To simplify the development and deployment process, this project utilizes the [espressif/idf](https://hub.docker.com/r/espressif/idf) Docker container. This approach eliminates the need to manually install ESP-IDF tools, streamlining the setup.

Additionally, a pre-configured Visual Studio Code project is included, making it easier to start developing. Just clone the repository and open it in Visual Studio Code to get started with minimal configuration.