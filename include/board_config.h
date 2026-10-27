#pragma once
// Example classic ESP32 DevKit pins; validate boot pins and board pinout.
namespace Pins {
constexpr int TRIG = 5;
constexpr int ECHO = 18; // HC-SR04 requires a 3.3 V-safe interface here.
constexpr int IR_LEFT = 19;
constexpr int IR_RIGHT = 23;
constexpr int PWMA = 25;
constexpr int AIN1 = 26;
constexpr int AIN2 = 27;
constexpr int PWMB = 14;
constexpr int BIN1 = 16;
constexpr int BIN2 = 17;
constexpr int STBY = 13;
}
namespace Config {
constexpr float STOP_DISTANCE_CM = 22.0f;
constexpr float MAX_DISTANCE_CM = 350.0f;
constexpr uint32_t ULTRASONIC_TIMEOUT_US = 25000;
constexpr uint32_t LOOP_PERIOD_MS = 40;
constexpr uint32_t SENSOR_STALE_MS = 200;
constexpr int PWM_FREQUENCY_HZ = 18000;
constexpr int PWM_BITS = 8;
}
