# Basic Oscillator Module Firmware (ESP32-S3 Modular Synth)

This repository contains the ESP-IDF firmware for a Basic Oscillator module within the ESP32-S3 Modular Synthesizer project.

## Purpose

This module acts as a digital sound source, generating classic waveforms. It listens for commands and parameter changes from the Central Controller via the main I2C bus and outputs its audio signal onto the shared I2S TDM bus.

## Functionality

* **Waveforms:** Generates Sine, Square, Sawtooth, and Triangle waves (selectable via I2C).
* **Pitch Control:** Responds to pitch information (e.g., MIDI note number + fine tune) sent via I2C.
* **Level Control:** Output level controllable via I2C.
* **I2S TDM Output:** Outputs audio signal as an I2S slave onto TDM slot(s) assigned by the Central Controller via I2C (`REG_COMMON_I2S_CONFIG`).
* **I2C Slave:** Responds to commands defined in the `module_i2c_proto` specification.
* **(Optional/If Implemented)** Local UI: Uses SSD1306 display and encoder for setting the module's I2C address and potentially displaying status (requires `menu_system_ssd1306` component).

## Hardware Target

* **MCU:** ESP32-S3 (Tested on DevKitC-1 N8R2 and N32R8V variants).
* **Connections:**
  * Backplane connector for Power, Ground, Main I2C (SDA/SCL), I2S TDM (MCLK, BCLK, WS input; SD_OUT output).
  * (Optional) Local I2C connector for SSD1306 Display.
  * (Optional) GPIO connections for Rotary Encoder(s).

## Dependencies

* **ESP-IDF:** Version 5.x (e.g., v5.4-dev or a stable release like v5.2.1 - specify!).
* **`module_i2c_proto`:** Shared component defining the I2C communication protocol.
* **(Optional) `common_definitions`:** Shared component for basic types/constants.
* **(Optional) `menu_system_ssd1306`:** Shared component for local UI, if implemented.
* **(Optional) `esp-dsp`:** May use ESP-DSP library for optimized oscillator or filter functions.

*(These dependencies should be declared in this project's `idf_component.yml` file for management via the ESP-IDF Component Manager)*

## Configuration

Project-specific settings can be configured via `idf.py menuconfig`, including:

* Default I2C Slave Address (used if no address is found in NVS).
* (If applicable) Specific GPIO assignments for local UI elements.
* Logging levels.

## Building & Flashing

1. Ensure ESP-IDF environment is set up correctly for the target version.
2. Navigate to this firmware directory (`firmware/basic_oscillator/` or similar).
3. Set target: `idf.py set-target esp32s3`
4. Configure: `idf.py menuconfig` (optional)
5. Build: `idf.py build`
6. Flash: `idf.py -p /dev/ttyUSB0 flash monitor` (replace `/dev/ttyUSB0` with your serial port).

## I2C Interface Summary

This module responds to various commands defined in `module_i2c_proto`, including:

* `REG_COMMON_MODULE_TYPE` (Responds with `MODULE_TYPE_OSCILLATOR`)
* `REG_COMMON_FIRMWARE_VERSION`
* `REG_COMMON_STATUS`
* `CMD_COMMON_RESET`
* `REG_COMMON_I2S_CONFIG`
* `REG_COMMON_SET_PARAM` for parameters like:
  * `PARAM_OSC_WAVEFORM`
  * `PARAM_OSC_PITCH_MIDI` / `PARAM_OSC_PITCH_FIXED_HZ`
  * `PARAM_OSC_LEVEL_U16`
  * `PARAM_OSC_PW_U16`
  * *(Add other relevant parameters this module implements)*

*(Refer to the `module_i2c_proto` documentation/repository for the complete protocol definition.)*

## I2S Interface

* Operates as an I2S TDM Slave.
* Requires MCLK, BCLK, WS signals from the backplane bus.
* Outputs audio data on one or more TDM slots on the `SD_OUT` line, as configured by the Central Controller via the `REG_COMMON_I2S_CONFIG` I2C command.
