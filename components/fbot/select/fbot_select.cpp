#include "fbot_select.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

namespace esphome {
namespace fbot {

static const char *const TAG = "fbot.select";

void FbotSelect::setup() {
}

void FbotSelect::dump_config() {
  LOG_SELECT("", "Fbot Select", this);
  ESP_LOGCONFIG(TAG, "  Type: %s", this->select_type_.c_str());
}

void FbotSelect::control(const std::string &value) {
  if (this->parent_ == nullptr) {
    ESP_LOGW(TAG, "No parent set for select");
    return;
  }
  
  if (!this->parent_->is_connected()) {
    ESP_LOGW(TAG, "Cannot change select '%s': device is disconnected", this->select_type_.c_str());
    return;
  }
  
  if (this->select_type_ == "light_mode") {
    this->parent_->control_light_mode(value);
  } else if (this->select_type_ == "ac_charge_limit") {
    this->parent_->control_ac_charge_limit(value);
  } else if (this->select_type_ == "machine_shutdown") {
    this->parent_->control_machine_shutdown_select(value);
  } else {
    ESP_LOGW(TAG, "Unknown select type: %s", this->select_type_.c_str());
    return;
  }
  
  this->publish_state(value);
}

}  // namespace fbot
}  // namespace esphome

#endif  // USE_ESP32
