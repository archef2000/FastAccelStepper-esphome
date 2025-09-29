#include "fast_accel_stepper.h"
#include "esphome/core/log.h"
#include <cfloat>
#include <cstdint>

namespace esphome {
namespace fast_accel_stepper {

static const char *const TAG = "fast_accel_stepper";

void FastAccelStepperComponent::setup() {
  // Initialize engine and attach stepper
  engine_.init(1);
  engine_.task_rate(10); // defaul 4ms
  ESP_LOGE(TAG, "Attaching to step pin %u", step_pin_);
  uint8_t step_gpio = step_pin_;
  fas_ = engine_.stepperConnectToPin(step_gpio);
  if (fas_ == nullptr) {
    ESP_LOGE(TAG, "FastAccelStepper failed to attach to step pin %u",
             step_pin_);
    this->mark_failed();
    return;
  }
  if (has_dir_) {
    fas_->setDirectionPin(dir_pin_, dir_inverted_);
  }
  if (has_enable_) {
    ESP_LOGE(TAG, "Attaching to enable pin %u", enable_pin_);
    fas_->setAutoEnable(true);
    fas_->setEnablePin(enable_pin_, enable_inverted_);
  }
  fas_->setSpeedInHz(this->max_speed_);

  int32_t lib_pos = fas_->getCurrentPosition();
  position_offset_ = this->current_position - lib_pos;
}

void FastAccelStepperComponent::on_update_speed() {
  if (fas_ != nullptr) {
    ESP_LOGD(TAG, "on_update_speed: %f", this->max_speed_);
    if (unlock_speed_) {
      fas_->setAbsoluteSpeedLimit(15);
    }
    if (this->max_speed_ < 0) {
      this->max_speed_ = MAXFLOAT;
    }
    fas_->setSpeedInHz(this->max_speed_);
  }
}

void FastAccelStepperComponent::loop() {
  if (fas_ == nullptr)
    return;
  if (last_acceleration_ != this->acceleration_) {
    ESP_LOGD(TAG, "acceleration updated: %f", this->acceleration_);
    last_acceleration_ = this->acceleration_;
    fas_->setAcceleration(this->acceleration_);
    fas_->applySpeedAcceleration();
  }
  int32_t lib_pos = fas_->getCurrentPosition();

  if (last_commanded_target_ != this->target_position) {
    last_commanded_target_ = this->target_position;

    ESP_LOGD(TAG, "current: %ld", this->current_position);
    ESP_LOGD(TAG, "published: %ld", last_published_position_);
    ESP_LOGD(TAG, "target: %ld", this->target_position);
    int32_t lib_delta = last_published_position_ - (lib_pos - position_offset_);
    ESP_LOGD(TAG, "lib_delta: %ld", lib_delta);
    int32_t delta = this->target_position - this->current_position;
    if (delta == 0) {
      fas_->stopMove();
    } else {
      uint8_t ramp = fas_->rampState();
      uint8_t dir = (ramp & RAMP_DIRECTION_MASK) / 32;
      uint8_t target_dir = delta > 0 ? 1 : 2;
      if (dir == 0) {
      } else if (target_dir != dir) {
        ESP_LOGD(TAG, "Direction change detected: %d -> %d", dir, target_dir);
        fas_->stopMove();
      }
      ESP_LOGD(TAG, "move delta=%ld, ramp=%d", delta, ramp);
      fas_->move(delta + lib_delta);
    }
    ESP_LOGD(TAG, "move done");
  }
  if (this->current_position != last_published_position_) {
    ESP_LOGE(TAG, "Positions last_published: %ld current: %ld lib_pos: %ld",
             last_published_position_, this->current_position, lib_pos);
    ESP_LOGE(TAG, "Position offset changed: from %ld", position_offset_);
    position_offset_ += last_published_position_ - this->current_position;
    ESP_LOGE(TAG, "Position offset changed: to %ld", position_offset_);
    last_commanded_target_ = this->current_position;
  }
  this->current_position = last_published_position_ =
      lib_pos - position_offset_;
}

void FastAccelStepperComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "FastAccelStepper Stepper:");
  ESP_LOGCONFIG(TAG, "  Step pin: %u", this->step_pin_);
  ESP_LOGCONFIG(TAG, "  Dir pin: %u", this->dir_pin_);
  ESP_LOGCONFIG(TAG, "  Enable pin: %u", this->enable_pin_);
  LOG_STEPPER(this);
}

} // namespace fast_accel_stepper
} // namespace esphome
