#include "fbot_number.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

namespace esphome {
namespace fbot {

static const char *const TAG = "fbot.number";

void FbotNumber::setup() {
  // Numbers start with no value until first update
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

  // Check if device is connected before allowing changes
  if (!this->parent_->is_connected()) {
    ESP_LOGW(TAG, "Cannot change number '%s': device is disconnected",
             this->number_type_.c_str());
    return;
  }

  // Call the appropriate control method based on number type
  if (this->number_type_ == "threshold_charge") {
    this->parent_->set_threshold_charge(value);
  } else if (this->number_type_ == "threshold_discharge") {
    this->parent_->set_threshold_discharge(value);
  } else if (this->number_type_ == "polling_interval") {
    this->parent_->set_polling_interval(static_cast<uint32_t>(value * 1000));
  } else if (this->number_type_ == "settings_polling_interval") {
    this->parent_->set_settings_polling_interval(
        static_cast<uint32_t>(value * 1000));
  } else if (this->number_type_ == "screen_timeout") {
    this->parent_->set_screen_timeout(static_cast<uint16_t>(value));
  } else if (this->number_type_ == "ac_standby") {
    this->parent_->set_ac_standby(static_cast<uint16_t>(value));
  } else if (this->number_type_ == "dc_standby") {
    this->parent_->set_dc_standby(static_cast<uint16_t>(value));
  } else if (this->number_type_ == "usb_standby") {
    this->parent_->set_usb_standby(static_cast<uint16_t>(value));
  } else if (this->number_type_ == "start_charge_after") {
    this->parent_->set_start_charge_after(static_cast<uint16_t>(value));
  } else {
    ESP_LOGW(TAG, "Unknown number type: %s", this->number_type_.c_str());
    return;
  }

  // Publish the new value optimistically
  // The actual value will be confirmed when the next status update arrives
  this->publish_state(value);
}

} // namespace fbot
} // namespace esphome

#endif // USE_ESP32
