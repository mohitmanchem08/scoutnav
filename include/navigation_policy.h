#pragma once

enum class RoverMode { STOPPED, FORWARD, TURN_LEFT, TURN_RIGHT, REVERSE, FAULT };

struct SensorState {
  bool frontValid;
  float frontDistanceCm;
  bool leftBlocked;
  bool rightBlocked;
};

struct MotorCommand {
  float left;
  float right;
  RoverMode mode;
};

inline MotorCommand stopped(RoverMode mode = RoverMode::STOPPED) {
  return {0.0f, 0.0f, mode};
}

inline MotorCommand decideNavigation(const SensorState &s, float stopDistanceCm) {
  if (!s.frontValid) return stopped(RoverMode::FAULT);
  if (s.frontDistanceCm <= stopDistanceCm) {
    if (s.leftBlocked && s.rightBlocked) return {-0.35f, -0.35f, RoverMode::REVERSE};
    if (s.leftBlocked) return {0.45f, -0.45f, RoverMode::TURN_RIGHT};
    if (s.rightBlocked) return {-0.45f, 0.45f, RoverMode::TURN_LEFT};
    return {-0.35f, -0.35f, RoverMode::REVERSE};
  }
  if (s.leftBlocked && !s.rightBlocked) return {0.35f, -0.35f, RoverMode::TURN_RIGHT};
  if (s.rightBlocked && !s.leftBlocked) return {-0.35f, 0.35f, RoverMode::TURN_LEFT};
  if (s.leftBlocked && s.rightBlocked) return stopped(RoverMode::STOPPED);
  return {0.45f, 0.45f, RoverMode::FORWARD};
}
