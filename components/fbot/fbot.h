#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif

#ifdef USE_ESP32

#include <esp_gattc_api.h>

namespace esphome {
namespace fbot {

// BLE Service and Characteristic UUIDs
static const char *const SERVICE_UUID = "0000a002-0000-1000-8000-00805f9b34fb";
static const char *const WRITE_CHAR_UUID =
    "0000c304-0000-1000-8000-00805f9b34fb";
static const char *const NOTIFY_CHAR_UUID =
    "0000c305-0000-1000-8000-00805f9b34fb";

// Register definitions
static const uint8_t REG_USB_CONTROL = 24;
static const uint8_t REG_DC_CONTROL = 25;
static const uint8_t REG_AC_CONTROL = 26;
static const uint8_t REG_LIGHT_CONTROL = 27;
static const uint8_t REG_BEEP_CONTROL = 56;
static const uint8_t REG_AC_SILENT_CONTROL = 57;
static const uint8_t REG_THRESHOLD_DISCHARGE = 66;
static const uint8_t REG_THRESHOLD_CHARGE = 67;
static const uint8_t REG_SCREEN_TIMEOUT = 59;
static const uint8_t REG_AC_STANDBY = 60;
static const uint8_t REG_DC_STANDBY = 61;
static const uint8_t REG_USB_STANDBY = 62;
static const uint8_t REG_START_CHARGE_AFTER = 63;
static const uint8_t REG_POWER_OFF = 64;

// State flag bit masks for register 41
static const uint16_t STATE_USB_BIT = 512;    // bit 9
static const uint16_t STATE_DC_BIT = 1024;    // bit 10
static const uint16_t STATE_AC_BIT = 2048;    // bit 11
static const uint16_t STATE_LIGHT_BIT = 4096; // bit 12

class Fbot : public esphome::ble_client::BLEClientNode, public Component {
public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;

  // Configuration
  // Configuration
  void set_polling_interval(uint32_t interval);
  void set_settings_polling_interval(uint32_t interval);

  // Sensor setters
  void set_battery_percent_sensor(sensor::Sensor *sensor) {
    this->battery_percent_sensor_ = sensor;
  }
  void set_battery_percent_s1_sensor(sensor::Sensor *sensor) {
    this->battery_percent_s1_sensor_ = sensor;
  }
  void set_battery_percent_s2_sensor(sensor::Sensor *sensor) {
    this->battery_percent_s2_sensor_ = sensor;
  }
  void set_input_power_sensor(sensor::Sensor *sensor) {
    this->input_power_sensor_ = sensor;
  }
  void set_ac_input_power_sensor(sensor::Sensor *sensor) {
    this->ac_input_power_sensor_ = sensor;
  }
  void set_dc_input_power_sensor(sensor::Sensor *sensor) {
    this->dc_input_power_sensor_ = sensor;
  }
  void set_output_power_sensor(sensor::Sensor *sensor) {
    this->output_power_sensor_ = sensor;
  }
  void set_system_power_sensor(sensor::Sensor *sensor) {
    this->system_power_sensor_ = sensor;
  }
  void set_total_power_sensor(sensor::Sensor *sensor) {
    this->total_power_sensor_ = sensor;
  }
  void set_remaining_time_sensor(sensor::Sensor *sensor) {
    this->remaining_time_sensor_ = sensor;
  }
  void set_threshold_charge_sensor(sensor::Sensor *sensor) {
    this->threshold_charge_sensor_ = sensor;
  }
  void set_threshold_discharge_sensor(sensor::Sensor *sensor) {
    this->threshold_discharge_sensor_ = sensor;
  }
  void set_charge_level_sensor(sensor::Sensor *sensor) {
    this->charge_level_sensor_ = sensor;
  }
  void set_ac_out_voltage_sensor(sensor::Sensor *sensor) {
    this->ac_out_voltage_sensor_ = sensor;
  }
  void set_ac_out_frequency_sensor(sensor::Sensor *sensor) {
    this->ac_out_frequency_sensor_ = sensor;
  }
  void set_ac_in_frequency_sensor(sensor::Sensor *sensor) {
    this->ac_in_frequency_sensor_ = sensor;
  }
  void set_time_to_full_sensor(sensor::Sensor *sensor) {
    this->time_to_full_sensor_ = sensor;
  }
  void set_screen_timeout_sensor(sensor::Sensor *sensor) {
    this->screen_timeout_sensor_ = sensor;
  }
  void set_ac_standby_sensor(sensor::Sensor *sensor) {
    this->ac_standby_sensor_ = sensor;
  }
  void set_dc_standby_sensor(sensor::Sensor *sensor) {
    this->dc_standby_sensor_ = sensor;
  }
  void set_usb_standby_sensor(sensor::Sensor *sensor) {
    this->usb_standby_sensor_ = sensor;
  }
  void set_start_charge_after_sensor(sensor::Sensor *sensor) {
    this->start_charge_after_sensor_ = sensor;
  }

  // Binary sensor setters
  void set_connected_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->connected_binary_sensor_ = sensor;
  }
  void
  set_battery_connected_s1_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->battery_connected_s1_binary_sensor_ = sensor;
  }
  void
  set_battery_connected_s2_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->battery_connected_s2_binary_sensor_ = sensor;
  }
  void set_usb_active_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->usb_active_binary_sensor_ = sensor;
  }
  void set_dc_active_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->dc_active_binary_sensor_ = sensor;
  }
  void set_ac_active_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->ac_active_binary_sensor_ = sensor;
  }
  void set_light_active_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->light_active_binary_sensor_ = sensor;
  }
  void set_silent_charging_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->silent_charging_binary_sensor_ = sensor;
  }

  // Switch setters
  void set_usb_switch(switch_::Switch *sw) { this->usb_switch_ = sw; }
  void set_dc_switch(switch_::Switch *sw) { this->dc_switch_ = sw; }
  void set_ac_switch(switch_::Switch *sw) { this->ac_switch_ = sw; }
  void set_light_switch(switch_::Switch *sw) { this->light_switch_ = sw; }
  void set_ac_silent_switch(switch_::Switch *sw) {
    this->ac_silent_switch_ = sw;
  }
  void set_system_beeps_switch(switch_::Switch *sw) {
    this->system_beeps_switch_ = sw;
  }

#ifdef USE_NUMBER
  // Number setters
  void set_threshold_charge_number(number::Number *num) {
    this->threshold_charge_number_ = num;
  }
  void set_threshold_discharge_number(number::Number *num) {
    this->threshold_discharge_number_ = num;
  }
  void set_polling_interval_number(number::Number *num) {
    this->polling_interval_number_ = num;
  }
  void set_settings_polling_interval_number(number::Number *num) {
    this->settings_polling_interval_number_ = num;
  }
  void set_screen_timeout_number(number::Number *num) {
    this->screen_timeout_number_ = num;
  }
  void set_ac_standby_number(number::Number *num) {
    this->ac_standby_number_ = num;
  }
  void set_dc_standby_number(number::Number *num) {
    this->dc_standby_number_ = num;
  }
  void set_usb_standby_number(number::Number *num) {
    this->usb_standby_number_ = num;
  }
  void set_start_charge_after_number(number::Number *num) {
    this->start_charge_after_number_ = num;
  }
#endif

  // Control methods for switches
  void control_usb(bool state);
  void control_dc(bool state);
  void control_ac(bool state);
  void control_light(bool state);
  void control_ac_silent(bool state);
  void control_system_beeps(bool state);

  // Control methods for thresholds
  void set_threshold_charge(float percent);
  void set_threshold_discharge(float percent);
  void set_screen_timeout(uint16_t minutes);
  void set_ac_standby(uint16_t minutes);
  void set_dc_standby(uint16_t minutes);
  void set_usb_standby(uint16_t seconds);
  void set_start_charge_after(uint16_t minutes);
  void send_power_off();

  // Connection state getter
  bool is_connected() const { return connected_; }

protected:
  // BLE characteristics
  uint16_t write_handle_;
  uint16_t notify_handle_;

  // Timing
  uint32_t polling_interval_{2000};
  uint32_t settings_polling_interval_{
      60000}; // Default: Request settings every 60 seconds
  uint32_t last_poll_time_{0};
  uint32_t last_successful_poll_{0};
  uint32_t last_settings_request_time_{0};

  // Connection state
  bool connected_{false};
  bool characteristics_discovered_{false};
  bool settings_received_{false};

  // Polling failure tracking
  uint8_t consecutive_poll_failures_{0};
  static const uint8_t MAX_POLL_FAILURES = 3;
  static const uint32_t POLL_TIMEOUT_MS = 5000; // 5 seconds timeout

  // Sensors
  sensor::Sensor *battery_percent_sensor_{nullptr};
  sensor::Sensor *battery_percent_s1_sensor_{nullptr};
  sensor::Sensor *battery_percent_s2_sensor_{nullptr};
  sensor::Sensor *input_power_sensor_{nullptr};
  sensor::Sensor *ac_input_power_sensor_{nullptr};
  sensor::Sensor *dc_input_power_sensor_{nullptr};
  sensor::Sensor *output_power_sensor_{nullptr};
  sensor::Sensor *system_power_sensor_{nullptr};
  sensor::Sensor *total_power_sensor_{nullptr};
  sensor::Sensor *remaining_time_sensor_{nullptr};
  sensor::Sensor *threshold_charge_sensor_{nullptr};
  sensor::Sensor *threshold_discharge_sensor_{nullptr};
  sensor::Sensor *charge_level_sensor_{nullptr};
  sensor::Sensor *ac_out_voltage_sensor_{nullptr};
  sensor::Sensor *ac_out_frequency_sensor_{nullptr};
  sensor::Sensor *ac_in_frequency_sensor_{nullptr};
  sensor::Sensor *time_to_full_sensor_{nullptr};
  sensor::Sensor *screen_timeout_sensor_{nullptr};
  sensor::Sensor *ac_standby_sensor_{nullptr};
  sensor::Sensor *dc_standby_sensor_{nullptr};
  sensor::Sensor *usb_standby_sensor_{nullptr};
  sensor::Sensor *start_charge_after_sensor_{nullptr};

  // Binary sensors
  binary_sensor::BinarySensor *connected_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *battery_connected_s1_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *battery_connected_s2_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *usb_active_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *dc_active_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *ac_active_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *light_active_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *silent_charging_binary_sensor_{nullptr};

  // Switches
  switch_::Switch *usb_switch_{nullptr};
  switch_::Switch *dc_switch_{nullptr};
  switch_::Switch *ac_switch_{nullptr};
  switch_::Switch *light_switch_{nullptr};
  switch_::Switch *ac_silent_switch_{nullptr};
  switch_::Switch *system_beeps_switch_{nullptr};

#ifdef USE_NUMBER
  // Numbers
  number::Number *threshold_charge_number_{nullptr};
  number::Number *threshold_discharge_number_{nullptr};
  number::Number *polling_interval_number_{nullptr};
  number::Number *settings_polling_interval_number_{nullptr};
  number::Number *screen_timeout_number_{nullptr};
  number::Number *ac_standby_number_{nullptr};
  number::Number *dc_standby_number_{nullptr};
  number::Number *usb_standby_number_{nullptr};
  number::Number *start_charge_after_number_{nullptr};
#endif

  // Protocol methods
  uint16_t calculate_checksum(const uint8_t *data, size_t len);
  void generate_command_bytes(uint8_t address, uint16_t reg, uint16_t value,
                              uint8_t *output);
  void send_read_request();
  void send_settings_request();
  void send_control_command(uint16_t reg, uint16_t value);
  void parse_notification(const uint8_t *data, uint16_t length);
  void parse_settings_notification(const uint8_t *data, uint16_t length);
  uint16_t get_register(const uint8_t *data, uint16_t length,
                        uint16_t reg_index);

  // State management
  void update_connected_state(bool state);
  void reset_sensors_to_unknown();
  void check_poll_timeout();
};

} // namespace fbot
} // namespace esphome

#endif // USE_ESP32
