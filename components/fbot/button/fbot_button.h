#pragma once

#include "esphome/core/component.h"
#include "esphome/components/button/button.h"
#include "../fbot.h"

#ifdef USE_ESP32

namespace esphome {
namespace fbot {

class FbotButton : public button::Button, public Component {
 public:
  void set_parent(Fbot *parent) { this->parent_ = parent; }
  void set_button_type(const std::string &type) { this->button_type_ = type; }
  void dump_config() override;

 protected:
  void press_action() override;

  Fbot *parent_{nullptr};
  std::string button_type_;
};

}  // namespace fbot
}  // namespace esphome

#endif  // USE_ESP32
