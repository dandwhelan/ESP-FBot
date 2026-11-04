#include "fbot_number.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

namespace esphome {
namespace fbot {

static const char *const TAG = "fbot.number";

void FbotNumber::setup() {
  this->publish_state(NAN);
}

void FbotNumber::dump_config() {
  LOG_NUMBER("", "Fbot Number", this);
  ESP_LOGCONFIG(TAG, "  Type: %s", this->number_type_.c_str());
}

void FbotNumber::control(float value) {
  if (this->parent_ == nullptr) {
    ESP_LOGW(TAG, "No parent set for number");
    return;
  }

  if (!this->parent_->is_connected()) {
    ESP_LOGW(TAG, "Cannot change number '%s': device is disconnected", this->number_type_.c_str());
    return;
  }

  if (this->number_type_ == "threshold_charge") {
    this->parent_->set_threshold_charge(value);
  } else if (this->number_type_ == "threshold_discharge") {
    this->parent_->set_threshold_discharge(value);
  } else if (this->number_type_ == "sleep_timer") {
    this->parent_->set_sleep_timer(value);
  } else if (this->number_type_ == "charge_current") {
    this->parent_->set_charge_current(value);
  } else if (this->number_type_ == "screen_timeout") {
    this->parent_->set_screen_timeout(value);
  } else if (this->number_type_ == "booking_charge") {
    this->parent_->set_booking_charge(value);
  } else {
    ESP_LOGW(TAG, "Unknown number type: %s", this->number_type_.c_str());
    return;
  }

  this->publish_state(value);
}

}  // namespace fbot
}  // namespace esphome

#endif  // USE_ESP32
