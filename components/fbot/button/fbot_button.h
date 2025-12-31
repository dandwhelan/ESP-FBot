#pragma once

#include "../fbot.h"
#include "esphome/components/button/button.h"
#include "esphome/core/component.h"


#ifdef USE_ESP32

namespace esphome {
namespace fbot {

class FbotButton : public button::Button, public Component {
public:
  void setup() override {}
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_parent(Fbot *parent) { this->parent_ = parent; }
  void set_button_type(const std::string &type) { this->button_type_ = type; }

protected:
  void press_action() override;

  Fbot *parent_{nullptr};
  std::string button_type_;
};

} // namespace fbot
} // namespace esphome

#endif // USE_ESP32
