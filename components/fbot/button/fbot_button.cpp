#include "fbot_button.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

namespace esphome {
namespace fbot {

static const char *const TAG = "fbot.button";

void FbotButton::dump_config() {
  LOG_BUTTON("", "Fbot Button", this);
  ESP_LOGCONFIG(TAG, "  Type: %s", this->button_type_.c_str());
}

void FbotButton::press_action() {
  if (this->parent_ == nullptr) {
    ESP_LOGW(TAG, "No parent set for button");
    return;
  }

  if (!this->parent_->is_connected()) {
    ESP_LOGW(TAG, "Cannot press button '%s': device is disconnected",
             this->button_type_.c_str());
    return;
  }

  if (this->button_type_ == "power_off") {
    this->parent_->send_power_off();
  } else {
    ESP_LOGW(TAG, "Unknown button type: %s", this->button_type_.c_str());
  }
}

} // namespace fbot
} // namespace esphome

#endif // USE_ESP32
