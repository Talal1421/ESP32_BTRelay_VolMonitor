# ESP32 Bluetooth Relay Control & Battery Monitoring

![ESP32](https://img.shields.io/badge/ESP32-Espressif-E7352C?style=flat-square)
![Arduino](https://img.shields.io/badge/Arduino-Framework-00979D?style=flat-square)
![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=flat-square)
![Bluetooth Classic](https://img.shields.io/badge/Bluetooth_Classic-SPP-0082FC?style=flat-square)
![ADC](https://img.shields.io/badge/ADC-12_bit-546E7A?style=flat-square)

An ESP32 prototype for switching a battery-powered load over Bluetooth while monitoring battery voltage. I combined wireless relay control, calibrated ADC conversion, sample averaging, and voltage-dependent correction factors in an Arduino sketch, then assembled and tested the circuit on a breadboard.

**Talal Aloushan · August 2024**

## Overview

![Connection diagram showing the ESP32, Bluetooth controller, battery voltage divider, relay, and LED load](Connections_new.png)

A Bluetooth controller sends `ON` or `OFF` to the ESP32, which drives the relay and returns a confirmation. The ESP32 also sends battery-voltage readings to the controller and USB serial output.

## Results

The bench photos document the assembled prototype, a lit LED load, and received commands alongside voltage readings in a serial terminal. The terminal capture shows readings around 2.9 V; a separate multimeter photo shows 2.896 V. These photos record bench testing rather than a formal accuracy measurement.

<details open>
<summary><strong>Prototype and test photos</strong></summary>

| Breadboard prototype | Terminal readings |
|:---:|:---:|
| <img src="images/full_setup.jpeg" alt="Assembled ESP32, battery, divider and relay prototype" width="400"> | <img src="images/readings.jpeg" alt="Terminal capture showing ON/OFF commands and voltage readings" width="400"> |
| **LED load** | **Multimeter reading** |
| <img src="images/LED.jpeg" alt="Illuminated LED load during bench testing" width="400"> | <img src="images/voltmeter.jpeg" alt="Multimeter displaying 2.896 volts" width="400"> |

</details>

## Approach

- **Wireless control:** Bluetooth Classic Serial Port Profile, advertised as `ESP32-BT-Slave`; `ON` sets GPIO16 high and `OFF` sets it low.
- **Voltage sensing:** GPIO34 reads the divider output. The sketch assumes a 4.2 divider ratio, corresponding to 3 V at the ADC for a 12.6 V battery input.
- **Reading processing:** Espressif's ADC calibration functions convert raw samples to millivolts. A 30-sample moving average and piecewise correction factors process the readings before scaling back to battery voltage.
- **Feedback:** Command acknowledgements and voltage readings share the Bluetooth connection. Voltage is reported approximately once per second, with USB serial at 9600 baud.

## Project files

| File | Purpose |
|---|---|
| [`ESP32_BTH_RLY_ADC.ino`](ESP32_BTH_RLY_ADC.ino) | Relay control, Bluetooth communication, ADC conversion and filtering |
| [`Connections_new.png`](Connections_new.png) | Main connection diagram with Bluetooth control shown |
| [`Connections.PNG`](Connections.PNG) | Earlier connection diagram |
| [`images/`](images/) | Original hardware and bench-testing photos |
| [`ci.json`](ci.json) | ESP32 target exclusion configuration |
