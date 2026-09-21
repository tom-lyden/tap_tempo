# STM32F401 Tap Tempo

Bare-metal tap-tempo implementation for the STM32F401RE, written in C.

The project captures button tap intervals, calculates a rolling average tempo, and drives an indicator at the resulting beat.

## Features

* Tap-tempo input with debounced button handling
* Rolling average over recent tap intervals
* Configurable tempo limits and duty cycle
* Automatic input reset after inactivity
* Beat phase synchronization when a new tempo is accepted
* SysTick-based software timers
* Fixed-capacity circular queue
* No dynamic allocation

## Hardware

Developed and tested on an STM32 Nucleo-F401RE.

The application uses a separately developed register-level STM32F401RE HAL for GPIO, interrupts, and SysTick.

## Design

GPIO interrupts only record button edges. Debouncing and tap-tempo processing run in the foreground.

Tap intervals are stored in a circular queue and averaged to determine the active beat period. The indicator continues running at the current tempo while new tap input is collected.

## Build

Built with:

* C11
* CMake
* Ninja
* `arm-none-eabi-gcc`
