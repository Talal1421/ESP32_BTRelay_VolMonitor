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

The bench photos document the assembled prototype, a lit LED load, and Bluetooth command testing alongside voltage readings.

| Voltage comparison | Value |
|---|---:|
| Representative ESP32 terminal reading | 2.916 V |
| Multimeter reading | 2.896 V |
| Difference | +0.020 V (20 mV) |
| Relative difference, using the multimeter as reference | **+0.69%** |

Relative difference = `(2.916 − 2.896) / 2.896 × 100`. This compares one representative displayed value with the meter photo; it is not an accuracy specification across the operating range.

**Why three values per line?** The terminal photo shows comma-separated triples such as `2.914,2.916,2.916`. They are unlabeled, so their individual meanings cannot be recovered from the photo. The saved sketch emits one processed battery-voltage value per update to each output (USB serial and Bluetooth), not three columns. It does not identify these fields as raw, calibrated, or averaged readings; the capture reflects a different diagnostic output format. There is no evidence that the terminal software automatically tripled a single reading.

The values near 2.916 V are also not the nominal voltage of the three-cell pack. The saved sketch scales the measured divider voltage by 4.2 to estimate battery voltage; the photo does not identify the measurement point or scaling used in that diagnostic output.

**Why the difference?** Possible contributors include ADC reference/calibration error, nonlinearity, electrical noise, and resistor tolerances if divider scaling is involved. Averaging reduces fluctuations but does not remove systematic error. See [Espressif’s ADC calibration guidance](https://docs.espressif.com/projects/esp-idf/en/v4.4.4/esp32/api-reference/peripherals/adc.html).

A voltmeter draws a small input current because its input resistance is finite. That can lower the voltage at a high-resistance divider node: this is **meter loading**, rather than the probes preventing a “full current” reading. Voltage is measured across two points. Whether loading explains this 20 mV difference depends on the divider resistance and measurement conditions, which are not established by these photos. [Fluke describes this loading effect](https://assets.fluke.com/manuals/87______umeng0800.pdf).

<details open>
<summary><strong>Prototype and test photos</strong></summary>

| Breadboard prototype | Terminal readings |
|:---:|:---:|
| <img src="images/full_setup.jpeg" alt="Assembled ESP32, battery, divider and relay prototype" width="400"> | <img src="images/readings.jpeg" alt="Terminal capture showing ON/OFF commands and voltage readings" width="400"> |
| **LED load** | **Multimeter reading** |
| <img src="images/LED.jpeg" alt="Illuminated LED load during bench testing" width="400"> | <img src="images/voltmeter.jpeg" alt="Multimeter displaying 2.896 volts" width="400"> |

</details>

## Components

| ESP32 controller | Relay module | Battery pack |
|:---:|:---:|:---:|
| <img src="images/esp32.jpeg" alt="NodeMCU board labeled ESP-32S" width="280"> | <img src="images/relay.jpeg" alt="Relay module marked SRD-05VDC-SL-C" width="280"> | <img src="images/battery.jpeg" alt="Three cells labeled Li-ion rechargeable, 18650, 3.7 volts" width="280"> |
| NodeMCU development board labeled **ESP-32S** | **SRD-05VDC-SL-C** relay module | **3 × 3.7 V 18650 rechargeable lithium-ion cells** |

The battery chemistry is confirmed by the printed **“Li-ion Rechargeable Battery”** labels, not just by rechargeability. The three-cell series arrangement gives 11.1 V nominal; the sketch is designed around a 12.6 V maximum pack voltage. The prototype also uses a resistor divider, breadboard, jumper wires, and an LED load with a series resistor.

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
