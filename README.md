# FastAccelStepper ESPHome (external component)

A stepper platform for ESPHome that uses the FastAccelStepper PlatformIO library.

## Usage

```yaml
external_components:
  - source: github://archef2000/FastAccelStepper-esphome
    components: [ fast_accel_stepper ]

stepper:
  - platform: fast_accel_stepper
    id: x_axis
    step_pin: GPIO25
    dir_pin: GPIO26
    enable_pin: GPIO27      # optional
    max_speed: 2000         # steps/s
    acceleration: 1000      # steps/s^2
```
