# STM32F401 Tap Tempo

A bare-metal tap-tempo application for the STM32F401RE, written in C. Button taps determine the tempo of a blinking LED, while an SSD1306 OLED displays the tempo in BPM.

### Features

- Debounced button input using GPIO edge interrupts
- Rolling average of the two most recent tap intervals
- Tempo clamped to 20–240 BPM
- Initial tempo of 120 BPM
- Beat-phase synchronization when a new tempo is accepted
- Automatic tap-sequence reset after inactivity
- Double-buffered OLED driver with dirty-page updates
- Scalable 5×7 ASCII text rendering
- Fixed-size buffers with no dynamic allocation

### Hardware

- STM32 Nucleo-F401RE
- SSD1306 128×64 I2C OLED, address `0x3C`
- Momentary push button
- LED with a 330 Ω series resistor
- Two 4.7 kΩ I2C pull-up resistors

All components use the board’s 3.3 V supply and share a common ground.

| Component | Connection                                          |
|---|-----------------------------------------------------|
| Button | Between PC12 and 3.3 V; internal pulldown enabled   |
| LED | PB14 → 330 Ω resistor → LED anode; cathode → ground |
| OLED VCC / GND | 3.3 V / ground                                      |
| OLED SDA | PB3, with a 4.7 kΩ pull-up to 3.3 V                 |
| OLED SCL | PB10, with a 4.7 kΩ pull-up to 3.3 V                |

The OLED uses I2C2, configured for 400 kHz operation.

### Usage

The LED starts at 120 BPM. Tap the button three times to establish a new tempo from two intervals. Further taps update the rolling average and synchronize the LED’s beat phase.

Intervals outside the supported tempo range produce an output clamped to 20–240 BPM.

After approximately 4.5 seconds without a new press following release, the tap history resets. The LED continues at the previously selected tempo while a new sequence is collected.

### Design

GPIO interrupts record button edges. Debouncing, tap processing, rendering, and peripheral updates run in the foreground.

A 1 kHz SysTick counter supplies software timers for button debounce, LED timing, and tap-history reset. Tap intervals are stored in a fixed-capacity circular queue.

The OLED driver uses two framebuffers and submits dirty pages through the HAL’s queued, interrupt-driven I2C interface. The text renderer writes pixels through a callback, keeping font rendering separate from the display driver.

Hardware access is provided by the separately developed [STM32F401RE HAL](https://github.com/tom-lyden/f401_re_hal), fetched by CMake at version `v0.1.1`.

### Project layout

| Directory | Contents |
|---|---|
| `src/` | Application setup and tap-tempo logic |
| `include/` | Application interfaces and board pin assignments |
| `drivers/` | Button and SSD1306 drivers |
| `utils/` | Software timers, circular queue, and text renderer |
| `cmake/` | ARM GCC toolchain configuration |

### Build and development

The project uses C11, CMake 4.3 or newer, Ninja, and `arm-none-eabi-gcc`. The compiler must be available on `PATH`. An internet connection is required when CMake first fetches the HAL dependency.

Development and hardware debugging are performed in CLion using the board’s onboard ST-Link. Both Debug and Release presets are used.

1. Open the repository as a CMake project in CLion.
2. Select the Debug or Release preset.
3. Connect the Nucleo board through its ST-Link USB connector.
4. Build and launch using the configured Run or Debug workflow.

Flashing and debugging require a working local CLion/ST-Link configuration; the CMake presets alone do not configure that connection.

### Hardware validation

Verified on the Nucleo-F401RE:

- Button taps update the tempo and OLED display.
- Inactivity resets the tap sequence.
- Inputs outside the supported range clamp to 20–240 BPM.
- Tempo was checked against an external tempo app.
- The device continued producing the selected tempo during an overnight run.
- After disconnecting and reconnecting the I2C lines, entering a new tempo restores display updates without an MCU reset.
- LED operation continues independently of OLED operation, and OLED operation continues independently of LED operation.
- After reconnecting OLED power, an MCU reset restores normal display operation, with blank pages displaying correctly.

Automatic recovery from OLED power loss is not supported. Controlled mid-transfer interruption has not been tested.
