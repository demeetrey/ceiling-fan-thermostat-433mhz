🇺🇸 English | 🇷🇺 [Русский](README-RU.md)

# Arduino Nano Ceiling Fan Thermostat with 433 MHz RF Remote Control

## Description

An Arduino project for automatically controlling a ceiling fan over a **433 MHz** radio channel based on readings from two **DS18B20** temperature sensors.

The project is designed to periodically circulate hot air that accumulates under the ceiling from a fireplace.

The main sketch is `sketch-cft-433mhz.ino`. Two additional sketches are used for scanning and tuning RF codes and transmission parameters.

## What the Project Does

- Reads temperature from two DS18B20 sensors.
- Turns the fan on if the temperature on one of the sensors rises above the configured maximum.
- Turns the fan off if the temperature on one of the sensors drops below the configured minimum.
- Sends 433 MHz RF commands using the `RCSwitch` library.
- Uses sequences of remote-control codes for power on, power off, and direction switching commands.
- Shows the system status using green and red LEDs.

## Sketches

| File | Purpose |
| --- | --- |
| `sketch-cft-433mhz.ino` | Main sketch for automatic temperature-based fan control. |
| `sketch-radio-input-scan.ino` | Receives and prints RF codes from the remote control to the Serial Monitor. Used to capture codes from the device remote. |
| `sketch-radio-output-scan.ino` | Iterates through `pulseLength` values and sends known codes. Used to find the working pulse length. |

## Required Components

- Arduino-compatible board.
- 433 MHz RF transmitter.
- 433 MHz RF receiver for scanning codes.
- 2 DS18B20 temperature sensors.
- 2 LEDs for status indication.
- Resistors for the LEDs.
- Pull-up resistors for DS18B20 sensors, 4.7 kΩ between DATA and VCC.

## Libraries

Install the following libraries using the Arduino IDE Library Manager:

- `RCSwitch`
- `OneWire`
- `DallasTemperature`

## Wiring

The main sketch uses the following pins:

| Function | Arduino Pin |
| --- | --- |
| RF transmitter DATA | D2 |
| Green LED | D4 |
| Red LED | D6 |
| DS18B20 #1 DATA | D8 |
| DS18B20 #2 DATA | D10 |

The RF code receiver sketch uses the receiver on pin **D2**, through interrupt `0`.

## Schematics

Schematics are located in the `schemes-svg` directory. They can be opened in any modern web browser.

## Temperature Settings

The temperature thresholds in the main sketch are defined as follows:

```cpp
const short int MIN_TEMPERATURE = 37;
const short int MAX_TEMPERATURE = 40;
```

Operating logic:

- if the fan is off and the temperature on one of the sensors is above `MAX_TEMPERATURE`, the fan turns on;
- if the fan is on and the temperature on one of the sensors is below `MIN_TEMPERATURE`, the fan turns off.

## RF Settings

The main sketch uses the following parameters:

```cpp
mySwitch.setProtocol(1);
mySwitch.setPulseLength(270);
```

The working `pulseLength` was selected using the separate `sketch-radio-output-scan.ino` sketch.

## How to Use

1. Open `sketch-radio-input-scan.ino` and upload it to the Arduino.
2. Connect the 433 MHz RF receiver to D2.
3. Open the Serial Monitor at `9600` baud.
4. Press the buttons on the remote control and save the received RF codes.
5. If needed, use `sketch-radio-output-scan.ino` to find the correct `pulseLength`.
6. Update the code arrays in `sketch-cft-433mhz.ino` for your remote control.
7. Upload the main sketch `sketch-cft-433mhz.ino`.
8. Check that the temperature sensors and RF commands work correctly.

## Status Indication

- The green LED blinks 3 times on startup, then stays on.
- The red LED turns on when the fan is running.
- If the first DS18B20 sensor is not found, the red LED blinks once per second 10 times.
- If the second DS18B20 sensor is not found, the red LED blinks in double flashes 10 times.

## LEDs

For better system power efficiency, it is recommended to use PWM brightness control.  
Install the following resistors on the board:

- Red 3 mm LED: `680 Ω`
- Green 3 mm LED: `470 Ω`

Then connect the LEDs to the corresponding pins on the controller board.  
On the Arduino Nano, PWM is supported on pins `D3`, `D5`, `D6`, `D9`, `D10`, and `D11`.

Brightness is controlled using the following commands:

- `analogWrite(LED_R, 0)` — minimum / off
- `analogWrite(LED_R, 255)` — maximum brightness

The value can be in the range from `0` to `255`.

## Notes

- RF codes in this project are tied to a specific remote control and device. For another remote control, the codes must be captured again.
- The main sketch uses a cyclic queue of codes because the same command type may have several sequential values.
- After startup, a short calibration sequence is performed: fan on, direction switch, and fan off.
- Before permanent use, test the system manually and make sure the commands do not conflict with other 433 MHz devices.
- The `Button-codes.xlsx` file contains the saved RF codes for the remote-control buttons.
- `Deadlock` recovery for missed RF commands: if the fan is expected to be running but the temperature stays above the upper threshold, the controller sends OFF → ON to restore a known fan state.

## License

Copyright &copy; 2026 Dmitry Maslennikov (demeetrey)  
See the [LICENSE](LICENSE) file for details.

## Contacts

For questions, suggestions, or bug reports, please open an issue in this repository.
