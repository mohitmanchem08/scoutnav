import unittest
from dataclasses import dataclass
from enum import Enum

class Mode(Enum):
    STOPPED = "STOPPED"
    FORWARD = "FORWARD"
    TURN_LEFT = "TURN_LEFT"
    TURN_RIGHT = "TURN_RIGHT"
    REVERSE = "REVERSE"
    FAULT = "FAULT"

@dataclass
class Sensors:
    front_valid: bool
    front_cm: float
    left_blocked: bool = False
    right_blocked: bool = False

def decide(s, stop_cm=22.0):
    if not s.front_valid:
        return (0.0, 0.0, Mode.FAULT)
    if s.front_cm <= stop_cm:
        if s.left_blocked and s.right_blocked:
            return (-0.35, -0.35, Mode.REVERSE)
        if s.left_blocked:
            return (0.45, -0.45, Mode.TURN_RIGHT)
        if s.right_blocked:
            return (-0.45, 0.45, Mode.TURN_LEFT)
        return (-0.35, -0.35, Mode.REVERSE)
    if s.left_blocked and not s.right_blocked:
        return (0.35, -0.35, Mode.TURN_RIGHT)
    if s.right_blocked and not s.left_blocked:
        return (-0.35, 0.35, Mode.TURN_LEFT)
    if s.left_blocked and s.right_blocked:
        return (0.0, 0.0, Mode.STOPPED)
    return (0.45, 0.45, Mode.FORWARD)

class NavigationPolicyTests(unittest.TestCase):
    def test_invalid_range_stops(self):
        self.assertEqual(decide(Sensors(False, 0))[2], Mode.FAULT)
    def test_clear_path_moves_forward(self):
        self.assertEqual(decide(Sensors(True, 100))[2], Mode.FORWARD)
    def test_front_obstacle_reverses(self):
        self.assertEqual(decide(Sensors(True, 10))[2], Mode.REVERSE)
    def test_left_obstacle_turns_right(self):
        self.assertEqual(decide(Sensors(True, 100, True, False))[2], Mode.TURN_RIGHT)
    def test_right_obstacle_turns_left(self):
        self.assertEqual(decide(Sensors(True, 100, False, True))[2], Mode.TURN_LEFT)
    def test_both_side_sensors_stop_when_front_clear(self):
        self.assertEqual(decide(Sensors(True, 100, True, True))[2], Mode.STOPPED)

if __name__ == "__main__":
    unittest.main()
