#pragma once

#include "esphome/core/component.h"
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/switch/switch.h"

#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif

#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif

#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif

#ifdef USE_BUTTON
#include "esphome/components/button/button.h"
#endif

#ifdef USE_ESP32

#include <esp_gattc_api.h>

namespace esphome {
namespace fbot {

// BLE Service and Characteristic UUIDs
static const char *const SERVICE_UUID = "0000a002-0000-1000-8000-00805f9b34fb";
static const char *const WRITE_CHAR_UUID = "0000c304-0000-1000-8000-00805f9b34fb";
static const char *const NOTIFY_CHAR_UUID = "0000c305-0000-1000-8000-00805f9b34fb";

// Register definitions - STATUS (0x1104) and SETTINGS (0x1103)
static const uint8_t REG_FACTORY_RESET = 0;          // Settings: Factory Reset / Unbind (Write 1)
static const uint8_t REG_CHARGE_LEVEL_STATUS = 2;    // Status: AC Charge Speed Status (1-5)
static const uint8_t REG_AC_INPUT_WATTS = 3;         // Status: AC Input Watts
static const uint8_t REG_DC_INPUT_WATTS = 4;         // Status: DC / Solar Input Watts
static const uint8_t REG_TOTAL_INPUT_WATTS = 6;      // Status: Total Input Watts (AC + DC)
static const uint8_t REG_AC_GRID_POWER = 7;          // Status: Signed 16-bit AC Grid Power (W)
static const uint8_t REG_ERROR_CODE = 8;             // Status: Active Error Code (78=Inverter, 79=Safety Lockout)
static const uint8_t REG_HARDWARE_ID = 11;           // Settings: Hardware variant (0x0600 US, 0x0200 EU)
static const uint8_t REG_AC_CHARGE_LIMIT = 13;       // Settings/Status: AC Charge Rate Setting (1-5)
static const uint8_t REG_MAX_AC_INPUT = 14;          // Settings/Status: Max AC Input Watts
static const uint8_t REG_FREQUENCY_SET = 16;         // Settings/Status: Frequency Setting (Hz * 10)
static const uint8_t REG_MAX_CHARGE_CURRENT = 17;    // Settings: Max Charge Current Limit (Amps)
static const uint8_t REG_AC_OUT_VOLTAGE = 18;        // Status: AC Out Voltage (V * 10)
static const uint8_t REG_AC_OUT_FREQUENCY = 19;      // Status: AC Out Frequency (Hz * 10)
static const uint8_t REG_TOTAL_OUTPUT_WATTS_20 = 20; // Status: Total Output Power (Sum of all outputs)
static const uint8_t REG_CHARGE_CURRENT_SET = 20;    // Settings: Charge Current Setting (Amps, 1-20)
static const uint8_t REG_AC_IN_VOLTAGE = 21;         // Status: AC In Voltage (V * 10) / Cold code 15
static const uint8_t REG_BATTERY_VOLTAGE = 22;       // Status/Settings: Battery Voltage (V * 10)
static const uint8_t REG_USB_CONTROL = 24;           // Status/Settings: USB Ports Toggle (0/1)
static const uint8_t REG_DC_CONTROL = 25;            // Status/Settings: DC 12V Ports Toggle (0/1)
static const uint8_t REG_AC_CONTROL = 26;            // Status/Settings: AC Inverter Toggle (0/1)
static const uint8_t REG_LIGHT_CONTROL = 27;         // Status/Settings: Light Mode (0=Off, 1=On, 2=Flash, 3=SOS)
static const uint8_t REG_USB_A1_OUT = 30;            // Status: USB-A1 Output (Watts * 10)
static const uint8_t REG_USB_A2_OUT = 31;            // Status: USB-A2 Output (Watts * 10)
static const uint8_t REG_USB_C1_OUT = 34;            // Status: USB-C1 Output (Watts * 10)
static const uint8_t REG_USB_C2_OUT = 35;            // Status: USB-C2 Output (Watts * 10)
static const uint8_t REG_USB_C3_OUT = 36;            // Status: USB-C3 Output (Watts * 10)
static const uint8_t REG_USB_C4_OUT = 37;            // Status: USB-C4 Output (Watts * 10)
static const uint8_t REG_TOTAL_OUTPUT_WATTS = 39;    // Status: Total Output Power (Watts)
static const uint8_t REG_PACK_CONFIG_VOLTAGE = 40;   // Status: Pack Config Voltage (V * 10)
static const uint8_t REG_PORT_FLAGS = 41;            // Status: Port & Subsystem Active Flags
static const uint8_t REG_PROTECTION_FLAGS = 42;      // Status: Hardware Protection & Fault Mask (0x6000 Crit)
static const uint8_t REG_AC_MCU_VERSION = 47;        // Settings: AC MCU Firmware ((val & 0xFF) / 10.0)
static const uint8_t REG_STATUS_FLAGS = 48;          // Status: System Status Flags (0x8000 charging, 0x4000 standby)
static const uint8_t REG_BMS_MCU_VERSION = 48;       // Settings: BMS MCU Firmware ((val & 0xFF) / 10.0)
static const uint8_t REG_PV_MCU_VERSION = 49;        // Settings: PV MPPT Firmware ((val & 0xFF) / 10.0)
static const uint8_t REG_DC_MCU_VERSION = 50;        // Settings: DC / Panel Firmware ((val & 0xFF) / 10.0)
static const uint8_t REG_BATTERY_S1_SOC = 53;        // Status: Ext1 SOC (0=missing, (val-10)/10.0 %)
static const uint8_t REG_BATTERY_CAPACITY = 54;      // Status: Battery Full Capacity (0.1 Ah)
static const uint8_t REG_BATTERY_S2_SOC = 55;        // Status: Ext2 SOC (0=missing, (val-10)/10.0 %)
static const uint8_t REG_BATTERY_MAIN_SOC = 56;      // Status: Main SOC (0.1 %)
static const uint8_t REG_KEY_SOUND = 56;             // Settings: Button Beep Toggle (0/1)
static const uint8_t REG_BOOKING_DELAY = 57;         // Status: Scheduled Charge Delay Remaining (min)
static const uint8_t REG_AC_SILENT_CONTROL = 57;     // Settings: Silent Charging Mode (0/1)
static const uint8_t REG_TIME_TO_FULL = 58;          // Status: Time to Full Charge (min)
static const uint8_t REG_TIME_TO_EMPTY = 59;         // Status: Time to Empty (min)
static const uint8_t REG_USB_STANDBY_TIME = 59;      // Settings: USB Auto-off Timer (sec)
static const uint8_t REG_AC_STANDBY_COUNTER = 60;    // Status: AC Standby Countdown (min)
static const uint8_t REG_AC_STANDBY_TIME = 60;       // Settings: AC Standby Timer (sec)
static const uint8_t REG_DC_STANDBY_COUNTER = 61;    // Status: DC Standby Countdown (min)
static const uint8_t REG_DC_STANDBY_TIME = 61;       // Settings: DC Standby Timer (sec)
static const uint8_t REG_SCREEN_TIMEOUT = 62;        // Settings: Screen Auto-dim / Timeout (sec)
static const uint8_t REG_BOOKING_CHARGE = 63;        // Settings: Stop Charge After / Booking Timer (min)
static const uint8_t REG_POWER_OFF = 64;             // Settings: Power Off Command (Write 1)
static const uint8_t REG_BATTERY_S3_SOC = 66;        // Status: Ext3 SOC (0=missing, (val-10)/10.0 %)
static const uint8_t REG_THRESHOLD_DISCHARGE = 66;   // Settings: Discharge Floor Min SOC (0.1 %)
static const uint8_t REG_BATTERY_S4_SOC = 67;        // Status: Ext4 SOC (0=missing, (val-10)/10.0 %)
static const uint8_t REG_THRESHOLD_CHARGE = 67;      // Settings: Charge Ceiling Max SOC (0.1 %)
static const uint8_t REG_SHUTDOWN_TIMER_STATUS = 68; // Status: Shutdown Countdown (min)
static const uint8_t REG_SLEEP_TIMER = 68;           // Settings: Machine Auto-Shutdown (min) - NEVER 0!
static const uint8_t REG_FAN_LEVEL = 69;             // Status: Cooling Fan Level (0-5)

// Port & Subsystem Bitmasks for Register 41
static const uint16_t PORT_FLAG_AC_OUT_ACTIVE    = (1 << 2);  // 4
static const uint16_t PORT_FLAG_AC_IN_PRESENT    = (1 << 3);  // 8 (Grid)
static const uint16_t PORT_FLAG_AC_IN_CHARGING   = (1 << 4);  // 16
static const uint16_t PORT_FLAG_LOW_PV_PRESENT   = (1 << 5);  // 32
static const uint16_t PORT_FLAG_LOW_PV_CHARGING  = (1 << 6);  // 64
static const uint16_t PORT_FLAG_DC_OUT_ACTIVE    = (1 << 7);  // 128
static const uint16_t PORT_FLAG_CAR_IN_PRESENT   = (1 << 8);  // 256
static const uint16_t STATE_USB_BIT              = (1 << 9);  // 512
static const uint16_t STATE_DC_BIT               = (1 << 10); // 1024
static const uint16_t STATE_AC_BIT               = (1 << 11); // 2048
static const uint16_t STATE_LIGHT_BIT            = (1 << 12); // 4096
static const uint16_t PORT_FLAG_CAR_CHARGING     = (1 << 13); // 8192
static const uint16_t PORT_FLAG_HIGH_PV_PRESENT  = (1 << 14); // 16384
static const uint16_t PORT_FLAG_HIGH_PV_CHARGING = (1 << 15); // 32768

// Protection Bitmasks for Register 42
static const uint16_t PROT_FLAG_CRITICAL_MASK    = 0x6000;    // Bits 13-14: Critical Hardware Fault
static const uint16_t PROT_FLAG_WARNING_LATCH    = 0x8000;    // Bit 15: Warning Latch

// Status Bitmasks for Register 48
static const uint16_t STATUS_FLAG_AC_CHARGING    = 0x8000;    // Bit 15: Active Charging
static const uint16_t STATUS_FLAG_INVERTER_STANDBY = 0x4000;  // Bit 14: Inverter Standby
static const uint16_t STATUS_FLAG_ERROR_PENDING  = 0x0008;    // Bit 3: Transient error/switching

class Fbot : public esphome::ble_client::BLEClientNode, public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                          esp_ble_gattc_cb_param_t *param) override;
  
  // Configuration
  void set_polling_interval(uint32_t interval) { this->polling_interval_ = interval; }
  void set_settings_polling_interval(uint32_t interval) { this->settings_polling_interval_ = interval; }
  void set_poll_timeout(uint32_t timeout) { this->poll_timeout_ms_ = timeout; }
  void set_max_poll_failures(uint8_t max_failures) { this->max_poll_failures_ = max_failures; }
  
  // Sensor setters
  void set_battery_percent_sensor(sensor::Sensor *sensor) { this->battery_percent_sensor_ = sensor; }
  void set_battery_percent_s1_sensor(sensor::Sensor *sensor) { this->battery_percent_s1_sensor_ = sensor; }
  void set_battery_percent_s2_sensor(sensor::Sensor *sensor) { this->battery_percent_s2_sensor_ = sensor; }
  void set_battery_percent_s3_sensor(sensor::Sensor *sensor) { this->battery_percent_s3_sensor_ = sensor; }
  void set_battery_percent_s4_sensor(sensor::Sensor *sensor) { this->battery_percent_s4_sensor_ = sensor; }
  void set_battery_capacity_sensor(sensor::Sensor *sensor) { this->battery_capacity_sensor_ = sensor; }
  void set_battery_voltage_sensor(sensor::Sensor *sensor) { this->battery_voltage_sensor_ = sensor; }
  void set_input_power_sensor(sensor::Sensor *sensor) { this->input_power_sensor_ = sensor; }
  void set_ac_input_power_sensor(sensor::Sensor *sensor) { this->ac_input_power_sensor_ = sensor; }
  void set_dc_input_power_sensor(sensor::Sensor *sensor) { this->dc_input_power_sensor_ = sensor; }
  void set_output_power_sensor(sensor::Sensor *sensor) { this->output_power_sensor_ = sensor; }
  void set_system_power_sensor(sensor::Sensor *sensor) { this->system_power_sensor_ = sensor; }
  void set_total_power_sensor(sensor::Sensor *sensor) { this->total_power_sensor_ = sensor; }
  void set_ac_grid_power_sensor(sensor::Sensor *sensor) { this->ac_grid_power_sensor_ = sensor; }
  void set_remaining_time_sensor(sensor::Sensor *sensor) { this->remaining_time_sensor_ = sensor; }
  void set_time_to_empty_sensor(sensor::Sensor *sensor) { this->time_to_empty_sensor_ = sensor; }
  void set_threshold_charge_sensor(sensor::Sensor *sensor) { this->threshold_charge_sensor_ = sensor; }
  void set_threshold_discharge_sensor(sensor::Sensor *sensor) { this->threshold_discharge_sensor_ = sensor; }
  void set_charge_level_sensor(sensor::Sensor *sensor) { this->charge_level_sensor_ = sensor; }
  void set_ac_out_voltage_sensor(sensor::Sensor *sensor) { this->ac_out_voltage_sensor_ = sensor; }
  void set_ac_out_frequency_sensor(sensor::Sensor *sensor) { this->ac_out_frequency_sensor_ = sensor; }
  void set_ac_in_voltage_sensor(sensor::Sensor *sensor) { this->ac_in_voltage_sensor_ = sensor; }
  void set_ac_in_frequency_sensor(sensor::Sensor *sensor) { this->ac_in_frequency_sensor_ = sensor; }
  void set_time_to_full_sensor(sensor::Sensor *sensor) { this->time_to_full_sensor_ = sensor; }
  void set_booking_charge_countdown_sensor(sensor::Sensor *sensor) { this->booking_charge_countdown_sensor_ = sensor; }
  void set_ac_standby_counter_sensor(sensor::Sensor *sensor) { this->ac_standby_counter_sensor_ = sensor; }
  void set_dc_standby_counter_sensor(sensor::Sensor *sensor) { this->dc_standby_counter_sensor_ = sensor; }
  void set_shutdown_timer_countdown_sensor(sensor::Sensor *sensor) { this->shutdown_timer_countdown_sensor_ = sensor; }
  void set_fan_level_sensor(sensor::Sensor *sensor) { this->fan_level_sensor_ = sensor; }
  void set_error_code_sensor(sensor::Sensor *sensor) { this->error_code_sensor_ = sensor; }
  void set_protection_flags_sensor(sensor::Sensor *sensor) { this->protection_flags_sensor_ = sensor; }
  void set_status_flags_sensor(sensor::Sensor *sensor) { this->status_flags_sensor_ = sensor; }
  void set_hardware_id_sensor(sensor::Sensor *sensor) { this->hardware_id_sensor_ = sensor; }
  void set_max_ac_input_sensor(sensor::Sensor *sensor) { this->max_ac_input_sensor_ = sensor; }
  void set_max_charge_current_sensor(sensor::Sensor *sensor) { this->max_charge_current_sensor_ = sensor; }
  void set_ac_mcu_version_sensor(sensor::Sensor *sensor) { this->ac_mcu_version_sensor_ = sensor; }
  void set_bms_mcu_version_sensor(sensor::Sensor *sensor) { this->bms_mcu_version_sensor_ = sensor; }
  void set_pv_mcu_version_sensor(sensor::Sensor *sensor) { this->pv_mcu_version_sensor_ = sensor; }
  void set_dc_mcu_version_sensor(sensor::Sensor *sensor) { this->dc_mcu_version_sensor_ = sensor; }
  void set_usb_a1_power_sensor(sensor::Sensor *sensor) { this->usb_a1_power_sensor_ = sensor; }
  void set_usb_a2_power_sensor(sensor::Sensor *sensor) { this->usb_a2_power_sensor_ = sensor; }
  void set_usb_c1_power_sensor(sensor::Sensor *sensor) { this->usb_c1_power_sensor_ = sensor; }
  void set_usb_c2_power_sensor(sensor::Sensor *sensor) { this->usb_c2_power_sensor_ = sensor; }
  void set_usb_c3_power_sensor(sensor::Sensor *sensor) { this->usb_c3_power_sensor_ = sensor; }
  void set_usb_c4_power_sensor(sensor::Sensor *sensor) { this->usb_c4_power_sensor_ = sensor; }
  
  // Binary sensor setters
  void set_connected_binary_sensor(binary_sensor::BinarySensor *sensor) { 
    this->connected_binary_sensor_ = sensor; 
  }
  void set_battery_connected_s1_binary_sensor(binary_sensor::BinarySensor *sensor) { 
    this->battery_connected_s1_binary_sensor_ = sensor; 
  }
  void set_battery_connected_s2_binary_sensor(binary_sensor::BinarySensor *sensor) { 
    this->battery_connected_s2_binary_sensor_ = sensor; 
  }
  void set_battery_connected_s3_binary_sensor(binary_sensor::BinarySensor *sensor) { 
    this->battery_connected_s3_binary_sensor_ = sensor; 
  }
  void set_battery_connected_s4_binary_sensor(binary_sensor::BinarySensor *sensor) { 
    this->battery_connected_s4_binary_sensor_ = sensor; 
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
  void set_ac_out_active_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->ac_out_active_binary_sensor_ = sensor;
  }
  void set_ac_in_present_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->ac_in_present_binary_sensor_ = sensor;
  }
  void set_ac_in_charging_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->ac_in_charging_binary_sensor_ = sensor;
  }
  void set_low_pv_in_present_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->low_pv_in_present_binary_sensor_ = sensor;
  }
  void set_low_pv_charging_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->low_pv_charging_binary_sensor_ = sensor;
  }
  void set_dc_out_active_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->dc_out_active_binary_sensor_ = sensor;
  }
  void set_car_in_present_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->car_in_present_binary_sensor_ = sensor;
  }
  void set_car_charging_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->car_charging_binary_sensor_ = sensor;
  }
  void set_high_pv_in_present_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->high_pv_in_present_binary_sensor_ = sensor;
  }
  void set_high_pv_charging_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->high_pv_charging_binary_sensor_ = sensor;
  }
  void set_ups_bypass_active_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->ups_bypass_active_binary_sensor_ = sensor;
  }
  void set_charging_active_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->charging_active_binary_sensor_ = sensor;
  }
  void set_inverter_standby_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->inverter_standby_binary_sensor_ = sensor;
  }
  void set_critical_fault_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->critical_fault_binary_sensor_ = sensor;
  }
  void set_temp_protection_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->temp_protection_binary_sensor_ = sensor;
  }
  void set_inverter_fault_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->inverter_fault_binary_sensor_ = sensor;
  }

#ifdef USE_TEXT_SENSOR
  void set_system_status_text_sensor(text_sensor::TextSensor *sensor) {
    this->system_status_text_sensor_ = sensor;
  }
#endif
  
  // Switch setters
  void set_usb_switch(switch_::Switch *sw) { this->usb_switch_ = sw; }
  void set_dc_switch(switch_::Switch *sw) { this->dc_switch_ = sw; }
  void set_ac_switch(switch_::Switch *sw) { this->ac_switch_ = sw; }
  void set_light_switch(switch_::Switch *sw) { this->light_switch_ = sw; }
  void set_ac_silent_switch(switch_::Switch *sw) { this->ac_silent_switch_ = sw; }
  void set_key_sound_switch(switch_::Switch *sw) { this->key_sound_switch_ = sw; }
  
#ifdef USE_SELECT
  // Select setters
  void set_light_mode_select(select::Select *sel) { this->light_mode_select_ = sel; }
  void set_ac_charge_limit_select(select::Select *sel) { this->ac_charge_limit_select_ = sel; }
  void set_machine_shutdown_select(select::Select *sel) { this->machine_shutdown_select_ = sel; }
#endif
  
#ifdef USE_NUMBER
  // Number setters
  void set_threshold_charge_number(number::Number *num) { this->threshold_charge_number_ = num; }
  void set_threshold_discharge_number(number::Number *num) { this->threshold_discharge_number_ = num; }
  void set_sleep_timer_number(number::Number *num) { this->sleep_timer_number_ = num; }
  void set_charge_current_number(number::Number *num) { this->charge_current_number_ = num; }
  void set_screen_timeout_number(number::Number *num) { this->screen_timeout_number_ = num; }
  void set_booking_charge_number(number::Number *num) { this->booking_charge_number_ = num; }
#endif
  
  // Control methods for switches
  void control_usb(bool state);
  void control_dc(bool state);
  void control_ac(bool state);
  void control_light(bool state);
  void control_ac_silent(bool state);
  void control_key_sound(bool state);

  // Control methods for selects
  void control_light_mode(const std::string &value);
  void control_ac_charge_limit(const std::string &value);
  void control_machine_shutdown_select(const std::string &value);

  // Control methods for thresholds & settings
  void set_threshold_charge(float percent);
  void set_threshold_discharge(float percent);
  void set_sleep_timer(float minutes);
  void set_charge_current(float amps);
  void set_screen_timeout(float seconds);
  void set_booking_charge(float minutes);

  // Control methods for buttons / commands
  void control_power_off();
  void control_factory_reset();
  
  // WiFi configuration method
  void set_wifi_credentials(const std::string &ssid, const std::string &password);

  // Connection state getter
  bool is_connected() const { return connected_; }
  
 protected:
  // BLE characteristics
  uint16_t write_handle_{0};
  uint16_t notify_handle_{0};
  
  // Timing
  uint32_t polling_interval_{2000};
  uint32_t settings_polling_interval_{60000};
  uint32_t last_poll_time_{0};
  uint32_t last_successful_poll_{0};
  uint32_t last_settings_request_time_{0};
  
  // Connection state
  bool connected_{false};
  bool characteristics_discovered_{false};
  bool settings_received_{false};
  
  // Polling failure tracking
  uint8_t consecutive_poll_failures_{0};
  uint8_t max_poll_failures_{3};
  uint32_t poll_timeout_ms_{15000};
  
  // Sensors
  sensor::Sensor *battery_percent_sensor_{nullptr};
  sensor::Sensor *battery_percent_s1_sensor_{nullptr};
  sensor::Sensor *battery_percent_s2_sensor_{nullptr};
  sensor::Sensor *battery_percent_s3_sensor_{nullptr};
  sensor::Sensor *battery_percent_s4_sensor_{nullptr};
  sensor::Sensor *battery_capacity_sensor_{nullptr};
  sensor::Sensor *battery_voltage_sensor_{nullptr};
  sensor::Sensor *input_power_sensor_{nullptr};
  sensor::Sensor *ac_input_power_sensor_{nullptr};
  sensor::Sensor *dc_input_power_sensor_{nullptr};
  sensor::Sensor *output_power_sensor_{nullptr};
  sensor::Sensor *system_power_sensor_{nullptr};
  sensor::Sensor *total_power_sensor_{nullptr};
  sensor::Sensor *ac_grid_power_sensor_{nullptr};
  sensor::Sensor *remaining_time_sensor_{nullptr};
  sensor::Sensor *time_to_empty_sensor_{nullptr};
  sensor::Sensor *threshold_charge_sensor_{nullptr};
  sensor::Sensor *threshold_discharge_sensor_{nullptr};
  sensor::Sensor *charge_level_sensor_{nullptr};
  sensor::Sensor *ac_out_voltage_sensor_{nullptr};
  sensor::Sensor *ac_out_frequency_sensor_{nullptr};
  sensor::Sensor *ac_in_voltage_sensor_{nullptr};
  sensor::Sensor *ac_in_frequency_sensor_{nullptr};
  sensor::Sensor *time_to_full_sensor_{nullptr};
  sensor::Sensor *booking_charge_countdown_sensor_{nullptr};
  sensor::Sensor *ac_standby_counter_sensor_{nullptr};
  sensor::Sensor *dc_standby_counter_sensor_{nullptr};
  sensor::Sensor *shutdown_timer_countdown_sensor_{nullptr};
  sensor::Sensor *fan_level_sensor_{nullptr};
  sensor::Sensor *error_code_sensor_{nullptr};
  sensor::Sensor *protection_flags_sensor_{nullptr};
  sensor::Sensor *status_flags_sensor_{nullptr};
  sensor::Sensor *hardware_id_sensor_{nullptr};
  sensor::Sensor *max_ac_input_sensor_{nullptr};
  sensor::Sensor *max_charge_current_sensor_{nullptr};
  sensor::Sensor *ac_mcu_version_sensor_{nullptr};
  sensor::Sensor *bms_mcu_version_sensor_{nullptr};
  sensor::Sensor *pv_mcu_version_sensor_{nullptr};
  sensor::Sensor *dc_mcu_version_sensor_{nullptr};
  sensor::Sensor *usb_a1_power_sensor_{nullptr};
  sensor::Sensor *usb_a2_power_sensor_{nullptr};
  sensor::Sensor *usb_c1_power_sensor_{nullptr};
  sensor::Sensor *usb_c2_power_sensor_{nullptr};
  sensor::Sensor *usb_c3_power_sensor_{nullptr};
  sensor::Sensor *usb_c4_power_sensor_{nullptr};
  
  // Binary sensors
  binary_sensor::BinarySensor *connected_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *battery_connected_s1_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *battery_connected_s2_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *battery_connected_s3_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *battery_connected_s4_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *usb_active_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *dc_active_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *ac_active_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *light_active_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *ac_out_active_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *ac_in_present_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *ac_in_charging_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *low_pv_in_present_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *low_pv_charging_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *dc_out_active_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *car_in_present_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *car_charging_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *high_pv_in_present_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *high_pv_charging_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *ups_bypass_active_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *charging_active_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *inverter_standby_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *critical_fault_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *temp_protection_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *inverter_fault_binary_sensor_{nullptr};

#ifdef USE_TEXT_SENSOR
  text_sensor::TextSensor *system_status_text_sensor_{nullptr};
#endif
  
  // Switches
  switch_::Switch *usb_switch_{nullptr};
  switch_::Switch *dc_switch_{nullptr};
  switch_::Switch *ac_switch_{nullptr};
  switch_::Switch *light_switch_{nullptr};
  switch_::Switch *ac_silent_switch_{nullptr};
  switch_::Switch *key_sound_switch_{nullptr};
  
#ifdef USE_NUMBER
  // Numbers
  number::Number *threshold_charge_number_{nullptr};
  number::Number *threshold_discharge_number_{nullptr};
  number::Number *sleep_timer_number_{nullptr};
  number::Number *charge_current_number_{nullptr};
  number::Number *screen_timeout_number_{nullptr};
  number::Number *booking_charge_number_{nullptr};
#endif
  
#ifdef USE_SELECT
  // Selects
  select::Select *light_mode_select_{nullptr};
  select::Select *ac_charge_limit_select_{nullptr};
  select::Select *machine_shutdown_select_{nullptr};
#endif
  
  // Protocol methods
  uint16_t calculate_checksum(const uint8_t *data, size_t len);
  void generate_command_bytes(uint8_t address, uint16_t reg, uint16_t value, uint8_t *output);
  void send_read_request();
  void send_settings_request();
  void send_control_command(uint16_t reg, uint16_t value);
  void parse_notification(const uint8_t *data, uint16_t length);
  void parse_settings_notification(const uint8_t *data, uint16_t length);
  uint16_t get_register(const uint8_t *data, uint16_t length, uint16_t reg_index);
  
  // State management
  void update_connected_state(bool state);
  void reset_sensors_to_unknown();
  void check_poll_timeout();
};

}  // namespace fbot
}  // namespace esphome

#endif  // USE_ESP32
