#include <Arduino.h>
#include "board_config.h"
#include "navigation_policy.h"

class MotorDriver {
 public:
  void begin() {
    pinMode(Pins::AIN1, OUTPUT); pinMode(Pins::AIN2, OUTPUT);
    pinMode(Pins::BIN1, OUTPUT); pinMode(Pins::BIN2, OUTPUT);
    pinMode(Pins::STBY, OUTPUT);
    digitalWrite(Pins::STBY, LOW);
    ledcAttach(Pins::PWMA, Config::PWM_FREQUENCY_HZ, Config::PWM_BITS);
    ledcAttach(Pins::PWMB, Config::PWM_FREQUENCY_HZ, Config::PWM_BITS);
    stop();
  }
  void setOne(int pwmPin, int in1, int in2, float command) {
    command = constrain(command, -1.0f, 1.0f);
    if (command > 0) { digitalWrite(in1, HIGH); digitalWrite(in2, LOW); }
    else if (command < 0) { digitalWrite(in1, LOW); digitalWrite(in2, HIGH); }
    else { digitalWrite(in1, LOW); digitalWrite(in2, LOW); }
    ledcWrite(pwmPin, static_cast<uint32_t>(roundf(fabsf(command) * 255.0f)));
  }
  void apply(const MotorCommand &cmd) {
    if (cmd.left == 0.0f && cmd.right == 0.0f) { stop(); return; }
    digitalWrite(Pins::STBY, HIGH);
    setOne(Pins::PWMA, Pins::AIN1, Pins::AIN2, cmd.left);
    setOne(Pins::PWMB, Pins::BIN1, Pins::BIN2, cmd.right);
  }
  void stop() {
    ledcWrite(Pins::PWMA, 0); ledcWrite(Pins::PWMB, 0);
    digitalWrite(Pins::AIN1, LOW); digitalWrite(Pins::AIN2, LOW);
    digitalWrite(Pins::BIN1, LOW); digitalWrite(Pins::BIN2, LOW);
    digitalWrite(Pins::STBY, LOW);
  }
};

MotorDriver motors;
bool emergencyStop = false;
uint32_t lastLoopMs = 0;
uint32_t lastGoodSensorMs = 0;

float readFrontDistanceCm(bool &valid) {
  digitalWrite(Pins::TRIG, LOW);
  delayMicroseconds(3);
  digitalWrite(Pins::TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(Pins::TRIG, LOW);
  const unsigned long duration = pulseIn(Pins::ECHO, HIGH, Config::ULTRASONIC_TIMEOUT_US);
  if (duration == 0) { valid = false; return 0.0f; }
  const float cm = duration * 0.0343f / 2.0f;
  valid = cm > 1.5f && cm <= Config::MAX_DISTANCE_CM;
  return cm;
}

bool irBlocked(int pin) {
  // Many modules are active-low. Confirm with your exact module.
  return digitalRead(pin) == LOW;
}

const char *modeName(RoverMode mode) {
  switch (mode) {
    case RoverMode::STOPPED: return "STOPPED";
    case RoverMode::FORWARD: return "FORWARD";
    case RoverMode::TURN_LEFT: return "TURN_LEFT";
    case RoverMode::TURN_RIGHT: return "TURN_RIGHT";
    case RoverMode::REVERSE: return "REVERSE";
    default: return "FAULT";
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(Pins::TRIG, OUTPUT);
  pinMode(Pins::ECHO, INPUT);
  pinMode(Pins::IR_LEFT, INPUT_PULLUP);
  pinMode(Pins::IR_RIGHT, INPUT_PULLUP);
  motors.begin();
  Serial.println("#ScoutNav ready: e=emergency stop, r=reset");
}

void loop() {
  while (Serial.available()) {
    const char c = static_cast<char>(Serial.read());
    if (c == 'e' || c == 'E') { emergencyStop = true; motors.stop(); }
    if (c == 'r' || c == 'R') { emergencyStop = false; lastGoodSensorMs = millis(); }
  }
  const uint32_t now = millis();
  if (now - lastLoopMs < Config::LOOP_PERIOD_MS) return;
  lastLoopMs = now;
  if (emergencyStop) { motors.stop(); return; }

  bool valid = false;
  const float distance = readFrontDistanceCm(valid);
  const SensorState sensors = {valid, distance, irBlocked(Pins::IR_LEFT), irBlocked(Pins::IR_RIGHT)};
  if (valid) lastGoodSensorMs = now;
  if (!valid || now - lastGoodSensorMs > Config::SENSOR_STALE_MS) {
    motors.stop();
    Serial.println("{\"mode\":\"FAULT\",\"reason\":\"invalid_or_stale_range\"}");
    return;
  }

  const MotorCommand command = decideNavigation(sensors, Config::STOP_DISTANCE_CM);
  motors.apply(command);
  Serial.printf("{\"front_valid\":true,\"front_cm\":%.2f,\"left_blocked\":%s,"
                "\"right_blocked\":%s,\"left_cmd\":%.2f,\"right_cmd\":%.2f,"
                "\"mode\":\"%s\"}\n",
                distance, sensors.leftBlocked ? "true" : "false",
                sensors.rightBlocked ? "true" : "false",
                command.left, command.right, modeName(command.mode));
}
