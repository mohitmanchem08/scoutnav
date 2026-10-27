# Wiring and Bring-Up

The example pin map is in `include/board_config.h`; it assumes a classic ESP32 DevKit and a TB6612FNG-style interface. Confirm pin availability and LEDC API for your ESP32 Arduino core version.

## Required checks
- Choose the driver by motor voltage and stall current; verify thermal limits.
- Use a fuse, switch and appropriate battery. Do not power motors from ESP32 3.3 V.
- HC-SR04 ECHO is usually 5 V: use a calculated resistor divider or logic-level shifter.
- Confirm IR module output voltage and active polarity.
- Join grounds where required by the driver's reference design.
- Lift wheels for initial motor direction tests; use low PWM and accessible power disconnect.

If a wheel spins backward for a positive command, swap its output leads or invert its sign in the adapter and document the change.

## Physical setup photos

No physical rover wiring photograph is included because the assembled wiring has not been physically verified for this repository. Add real photos of the ESP32, motor driver, voltage divider/level shifter, sensors, power switch and chassis after testing.
