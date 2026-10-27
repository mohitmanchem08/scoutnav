# ScoutNav Interface Contract

State labels: `STOPPED`, `FORWARD`, `TURN_LEFT`, `TURN_RIGHT`, `REVERSE`, `FAULT`.

- Current firmware reports range in centimetres as `front_cm`; convert to metres for ROS 2.
- Motor commands are normalized to `[-1, 1]`; sign indicates direction.
- IR `true` means obstacle detected after active polarity is calibrated.
- Serial telemetry is JSON. Consumers should parse the `mode` string and preserve invalid sensor status.
- ROS commands must be time-bounded. Embedded firmware retains local e-stop and sensor-fault protection.
