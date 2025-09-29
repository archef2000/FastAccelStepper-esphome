#include "esphome/components/stepper/stepper.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include <cstdint>
#include <stdint.h>

// FastAccelStepper library
#include <FastAccelStepper.h>

namespace esphome {
namespace fast_accel_stepper {

class FastAccelStepperComponent : public stepper::Stepper, public Component {
public:
  void set_step_pin(int pin) {
    // only during boot
    step_pin_ = pin;
  }
  void set_dir_pin(int pin, bool inverted) {
    // only during boot
    has_dir_ = true;
    dir_pin_ = pin;
    dir_inverted_ = inverted;
  }
  void set_enable_pin(int pin, bool inverted) {
    // only during boot
    has_enable_ = true;
    enable_pin_ = pin;
    enable_inverted_ = inverted;
  }
  void set_acceleration(float acceleration) {
    // only during boot
    acceleration_ = acceleration;
  }
  void set_deceleration(float deceleration) {
    // only during boot
    deceleration_ = deceleration;
  }
  void set_max_speed(float max_speed) {
    // only during boot
    this->max_speed_ = max_speed;
  }
  void set_unlock_speed(bool unlock) {
    // only during boot
    unlock_speed_ = unlock;
  }

  void on_update_speed();

  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

protected:
  // set from actions:
  // float acceleration_{1e6f};
  // float deceleration_{1e6f};
  // int32_t target_position{0};
  // float max_speed_{1000.0f};

  int step_pin_;
  bool has_dir_{false};
  bool unlock_speed_{false};
  int dir_pin_;
  bool dir_inverted_{false};
  bool has_enable_{false};
  int enable_pin_;
  bool enable_inverted_{false};

  FastAccelStepperEngine engine_;
  FastAccelStepper *fas_{nullptr};
  int32_t last_commanded_target_{0};
  float last_acceleration_{0.0f};
  // Offset between the library's internal position and the exposed position.
  // exposed_position = lib_position + position_offset_
  int32_t position_offset_{0};
  int32_t last_published_position_{0};
};

} // namespace fast_accel_stepper
} // namespace esphome
