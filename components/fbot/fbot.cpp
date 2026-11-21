#include "fbot.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esp_attr.h"

#ifdef USE_ESP32

namespace esphome {
namespace fbot {

static const char *const TAG = "fbot";

// Last settings (0x1103) response, kept in RTC slow memory so it survives deep
// sleep. Lets settings show straight after a wake without asking the power station.
static const uint32_t SETTINGS_CACHE_MAGIC = 0x46425354;  // "FBST"
static const uint16_t SETTINGS_CACHE_MAX = 256;
struct SettingsCache {
  uint32_t magic;
  uint16_t length;
  uint8_t data[SETTINGS_CACHE_MAX];
};
static RTC_DATA_ATTR SettingsCache rtc_settings_cache = {0, 0, {0}};

static const uint32_t SENSOR_GRACE_MS = 60000;

void Fbot::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Fbot...");
  this->reset_sensors_to_unknown();
  this->update_connected_state(false);
  // Republish settings cached before deep sleep (once entities are set up)
  if (rtc_settings_cache.magic == SETTINGS_CACHE_MAGIC && rtc_settings_cache.length > 0 &&
      rtc_settings_cache.length <= SETTINGS_CACHE_MAX) {
    this->defer([this]() {
      ESP_LOGD(TAG, "Restoring cached settings (%u bytes)", rtc_settings_cache.length);
      this->parse_settings_notification(rtc_settings_cache.data, rtc_settings_cache.length);
    });
  }
}

void Fbot::loop() {
  // After a link drop keep the last values for a grace period (reconnects are
  // normally quick); only show "unknown" if the power station stays away.
  if (!this->connected_ && !this->sensors_reset_ && this->disconnected_since_ != 0 &&
      millis() - this->disconnected_since_ > SENSOR_GRACE_MS) {
    ESP_LOGW(TAG, "No connection for %us, marking sensors unknown", SENSOR_GRACE_MS / 1000);
    this->reset_sensors_to_unknown();
    this->sensors_reset_ = true;
  }
  // Poll for data if connected
  if (this->connected_ && this->characteristics_discovered_) {
    uint32_t now = millis();
    
    // Check if it's time to poll status data (0x1104)
    if (now - this->last_poll_time_ >= this->polling_interval_) {
      this->send_read_request();
      this->last_poll_time_ = now;
    }
    
    // Check if it's time to poll settings data (0x1103)
    if (now - this->last_settings_request_time_ >= this->settings_polling_interval_) {
      this->send_settings_request();
      this->last_settings_request_time_ = now;
    }
    
    // Check for poll timeout
    this->check_poll_timeout();
  }
}

void Fbot::dump_config() {
  ESP_LOGCONFIG(TAG, "Fbot Battery:");
  ESP_LOGCONFIG(TAG, "  Polling interval: %ums", this->polling_interval_);
  ESP_LOGCONFIG(TAG, "  Settings polling interval: %ums", this->settings_polling_interval_);
  ESP_LOGCONFIG(TAG, "  Poll timeout: %ums", this->poll_timeout_ms_);
  ESP_LOGCONFIG(TAG, "  Max poll failures: %u", this->max_poll_failures_);
  LOG_SENSOR("  ", "Battery Percent", this->battery_percent_sensor_);
  LOG_SENSOR("  ", "Battery S1 Percent", this->battery_percent_s1_sensor_);
  LOG_SENSOR("  ", "Battery S2 Percent", this->battery_percent_s2_sensor_);
  LOG_SENSOR("  ", "Battery S3 Percent", this->battery_percent_s3_sensor_);
  LOG_SENSOR("  ", "Battery S4 Percent", this->battery_percent_s4_sensor_);
  LOG_SENSOR("  ", "Battery Capacity", this->battery_capacity_sensor_);
  LOG_SENSOR("  ", "Battery Voltage", this->battery_voltage_sensor_);
  LOG_SENSOR("  ", "Total Input Power", this->input_power_sensor_);
  LOG_SENSOR("  ", "AC Input Power", this->ac_input_power_sensor_);
  LOG_SENSOR("  ", "DC Input Power", this->dc_input_power_sensor_);
  LOG_SENSOR("  ", "Output Power", this->output_power_sensor_);
  LOG_SENSOR("  ", "Total Output Power", this->total_power_sensor_);
  LOG_SENSOR("  ", "AC Grid Power", this->ac_grid_power_sensor_);
  LOG_SENSOR("  ", "Remaining Minutes", this->remaining_time_sensor_);
  LOG_SENSOR("  ", "Time to Empty", this->time_to_empty_sensor_);
  LOG_SENSOR("  ", "Time to Full", this->time_to_full_sensor_);
  LOG_SENSOR("  ", "AC Out Voltage", this->ac_out_voltage_sensor_);
  LOG_SENSOR("  ", "AC Out Frequency", this->ac_out_frequency_sensor_);
  LOG_SENSOR("  ", "AC In Voltage", this->ac_in_voltage_sensor_);
  LOG_SENSOR("  ", "Fan Level", this->fan_level_sensor_);
  LOG_SENSOR("  ", "Error Code", this->error_code_sensor_);
  LOG_SENSOR("  ", "Protection Flags", this->protection_flags_sensor_);
  LOG_SENSOR("  ", "Status Flags", this->status_flags_sensor_);
  LOG_BINARY_SENSOR("  ", "Connected", this->connected_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "Critical Fault", this->critical_fault_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "Temp Protection", this->temp_protection_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "Inverter Fault", this->inverter_fault_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "UPS Bypass Active", this->ups_bypass_active_binary_sensor_);
}

void Fbot::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                   esp_ble_gattc_cb_param_t *param) {
  switch (event) {
    case ESP_GATTC_OPEN_EVT: {
      if (param->open.status == ESP_GATT_OK) {
        ESP_LOGI(TAG, "Connected to Fbot");
        this->connected_ = true;
        this->consecutive_poll_failures_ = 0;
        this->last_successful_poll_ = 0;
        this->disconnected_since_ = 0;
        this->sensors_reset_ = false;
        this->update_connected_state(true);
      } else {
        ESP_LOGW(TAG, "Connection failed, status=%d", param->open.status);
        this->update_connected_state(false);
      }
      break;
    }
    
    case ESP_GATTC_DISCONNECT_EVT: {
      ESP_LOGW(TAG, "Disconnected from Fbot");
      this->connected_ = false;
      this->characteristics_discovered_ = false;
      this->disconnected_since_ = millis() ? millis() : 1;
      this->update_connected_state(false);
      break;
    }
    
    case ESP_GATTC_SEARCH_CMPL_EVT: {
      ESP_LOGD(TAG, "Service search complete");
      
      // Get write characteristic
      auto *write_chr = this->parent()->get_characteristic(
          esp32_ble_tracker::ESPBTUUID::from_raw(SERVICE_UUID),
          esp32_ble_tracker::ESPBTUUID::from_raw(WRITE_CHAR_UUID));
      if (write_chr == nullptr) {
        ESP_LOGW(TAG, "Write characteristic not found");
        break;
      }
      this->write_handle_ = write_chr->handle;
      ESP_LOGD(TAG, "Write characteristic handle: 0x%04x", this->write_handle_);
      
      // Get notify characteristic
      auto *notify_chr = this->parent()->get_characteristic(
          esp32_ble_tracker::ESPBTUUID::from_raw(SERVICE_UUID),
          esp32_ble_tracker::ESPBTUUID::from_raw(NOTIFY_CHAR_UUID));
      if (notify_chr == nullptr) {
        ESP_LOGW(TAG, "Notify characteristic not found");
        break;
      }
      this->notify_handle_ = notify_chr->handle;
      ESP_LOGD(TAG, "Notify characteristic handle: 0x%04x", this->notify_handle_);
      
      // Register for notifications
      auto status = esp_ble_gattc_register_for_notify(gattc_if, this->parent()->get_remote_bda(),
                                                       this->notify_handle_);
      if (status) {
        ESP_LOGW(TAG, "esp_ble_gattc_register_for_notify failed, status=%d", status);
      } else {
        ESP_LOGD(TAG, "Registered for notifications");
      }
      
      this->characteristics_discovered_ = true;
      this->node_state = esp32_ble_tracker::ClientState::ESTABLISHED;
      break;
    }
    
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
      if (param->reg_for_notify.status == ESP_GATT_OK) {
        ESP_LOGD(TAG, "Notification registration successful");
        uint32_t now = millis();
        this->last_poll_time_ = now;
        this->last_settings_request_time_ = now;
        this->send_read_request();
        if (this->settings_on_connect_)
          this->set_timeout(500, [this]() { this->send_settings_request(); });
      }
      break;
    }
    
    case ESP_GATTC_NOTIFY_EVT: {
      if (param->notify.handle == this->notify_handle_) {
        ESP_LOGVV(TAG, "Received notification, length=%d", param->notify.value_len);
        this->parse_notification(param->notify.value, param->notify.value_len);
      }
      break;
    }
    
    default:
      break;
  }
}

uint16_t Fbot::calculate_checksum(const uint8_t *data, size_t len) {
  // CRC-16 Modbus (polynomial 0xA001, initial 0xFFFF)
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (int j = 0; j < 8; j++) {
      if (crc & 1) {
        crc = (crc >> 1) ^ 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

void Fbot::generate_command_bytes(uint8_t address, uint16_t reg, uint16_t value, uint8_t *output) {
  output[0] = address;
  output[1] = 0x06;  // Function code for write
  output[2] = (reg >> 8) & 0xFF;
  output[3] = reg & 0xFF;
  output[4] = (value >> 8) & 0xFF;
  output[5] = value & 0xFF;
  
  uint16_t crc = this->calculate_checksum(output, 6);
  output[6] = (crc >> 8) & 0xFF;
  output[7] = crc & 0xFF;
}

void Fbot::send_read_request() {
  if (!this->connected_ || !this->characteristics_discovered_) {
    return;
  }
  
  // Read 80 registers starting from 0: [0x11, 0x04, 0x00, 0x00, 0x00, 0x50]
  uint8_t payload[6] = {0x11, 0x04, 0x00, 0x00, 0x00, 0x50};
  uint16_t crc = this->calculate_checksum(payload, 6);
  uint8_t command[8];
  memcpy(command, payload, 6);
  command[6] = (crc >> 8) & 0xFF;
  command[7] = crc & 0xFF;
  
  auto status = esp_ble_gattc_write_char(this->parent()->get_gattc_if(), this->parent()->get_conn_id(),
                                         this->write_handle_, sizeof(command), command,
                                         ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);
  if (status) {
    ESP_LOGW(TAG, "Error sending read request, status=%d", status);
  } else {
    ESP_LOGVV(TAG, "Sent read request");
  }
}

void Fbot::send_settings_request() {
  if (!this->connected_ || !this->characteristics_discovered_) {
    return;
  }
  
  // Read 80 holding registers starting from 0: [0x11, 0x03, 0x00, 0x00, 0x00, 0x50]
  uint8_t payload[6] = {0x11, 0x03, 0x00, 0x00, 0x00, 0x50};
  uint16_t crc = this->calculate_checksum(payload, 6);
  uint8_t command[8];
  memcpy(command, payload, 6);
  command[6] = (crc >> 8) & 0xFF;
  command[7] = crc & 0xFF;
  
  auto status = esp_ble_gattc_write_char(this->parent()->get_gattc_if(), this->parent()->get_conn_id(),
                                         this->write_handle_, sizeof(command), command,
                                         ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);
  if (status) {
    ESP_LOGW(TAG, "Error sending settings request, status=%d", status);
  } else {
    ESP_LOGD(TAG, "Sent settings request");
  }
}

void Fbot::send_control_command(uint16_t reg, uint16_t value) {
  if (!this->connected_ || !this->characteristics_discovered_) {
    ESP_LOGW(TAG, "Cannot send command: not connected");
    return;
  }
  
  uint8_t command[8];
  this->generate_command_bytes(0x11, reg, value, command);
  
  auto status = esp_ble_gattc_write_char(this->parent()->get_gattc_if(), this->parent()->get_conn_id(),
                                         this->write_handle_, sizeof(command), command,
                                         ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);
  if (status) {
    ESP_LOGW(TAG, "Error sending control command, status=%d", status);
  } else {
    ESP_LOGI(TAG, "Sent control command: reg=%d, value=%d", reg, value);
    this->last_poll_time_ = millis();
  }
}

uint16_t Fbot::get_register(const uint8_t *data, uint16_t length, uint16_t reg_index) {
  uint16_t offset = 6 + (reg_index * 2);
  if (offset + 1 >= length) {
    return 0;
  }
  return (data[offset] << 8) | data[offset + 1];
}

void Fbot::parse_notification(const uint8_t *data, uint16_t length) {
  if (length < 6) {
    ESP_LOGW(TAG, "Notification too short: %d bytes", length);
    return;
  }
  
  this->consecutive_poll_failures_ = 0;
  this->last_successful_poll_ = millis();
  
  if (data[0] != 0x11) {
    ESP_LOGVV(TAG, "Invalid header byte: 0x%02x", data[0]);
    return;
  }
  
  if (data[1] == 0x03) {
    if (length <= SETTINGS_CACHE_MAX) {
      memcpy(rtc_settings_cache.data, data, length);
      rtc_settings_cache.length = length;
      rtc_settings_cache.magic = SETTINGS_CACHE_MAGIC;
    }
    this->parse_settings_notification(data, length);
    return;
  } else if (data[1] == 0x04) {
    // Read Input Registers response (Status)
  } else {
    ESP_LOGVV(TAG, "Unknown function code: 0x%02x", data[1]);
    return;
  }
  
  // 1. Batteries
  float battery_percent = this->get_register(data, length, REG_BATTERY_MAIN_SOC) / 10.0f;
  uint16_t s1_raw = this->get_register(data, length, REG_BATTERY_S1_SOC);
  uint16_t s2_raw = this->get_register(data, length, REG_BATTERY_S2_SOC);
  uint16_t s3_raw = this->get_register(data, length, REG_BATTERY_S3_SOC);
  uint16_t s4_raw = this->get_register(data, length, REG_BATTERY_S4_SOC);
  
  float battery_percent_s1 = (s1_raw > 0) ? ((s1_raw - 10) / 10.0f) : NAN;
  float battery_percent_s2 = (s2_raw > 0) ? ((s2_raw - 10) / 10.0f) : NAN;
  float battery_percent_s3 = (s3_raw > 0) ? ((s3_raw - 10) / 10.0f) : NAN;
  float battery_percent_s4 = (s4_raw > 0) ? ((s4_raw - 10) / 10.0f) : NAN;
  
  bool battery_s1_connected = (s1_raw > 0);
  bool battery_s2_connected = (s2_raw > 0);
  bool battery_s3_connected = (s3_raw > 0);
  bool battery_s4_connected = (s4_raw > 0);
  
  // Capacity & Battery Voltage
  // Capacity: 0.1 Ah (e.g. 762 = 76.2 Ah)
  // Battery Voltage: Reg 22 is in 0.01 V (e.g. 4997 = 49.97 V for 16S LiFePO4 pack)
  float battery_capacity = this->get_register(data, length, REG_BATTERY_CAPACITY) * 0.1f; // Ah
  float battery_voltage = this->get_register(data, length, REG_BATTERY_VOLTAGE) / 100.0f;  // V
  
  // 2. Power & Watts
  uint16_t ac_input_watts = this->get_register(data, length, REG_AC_INPUT_WATTS);
  uint16_t dc_input_watts = this->get_register(data, length, REG_DC_INPUT_WATTS);
  uint16_t input_watts = this->get_register(data, length, REG_TOTAL_INPUT_WATTS);
  int16_t ac_grid_power = static_cast<int16_t>(this->get_register(data, length, REG_AC_GRID_POWER));
  uint16_t output_watts = this->get_register(data, length, REG_TOTAL_OUTPUT_WATTS);
  uint16_t total_watts = this->get_register(data, length, REG_TOTAL_OUTPUT_WATTS_20);
  
  // Charge level: Reg 2, 1-5 maps to 300W-1100W
  uint16_t charge_level_raw = this->get_register(data, length, REG_CHARGE_LEVEL_STATUS);
  uint16_t charge_level_watts = 0;
  if (charge_level_raw >= 1 && charge_level_raw <= 5) {
    charge_level_watts = 300 + ((charge_level_raw - 1) * 200);
  }
  
  // Inverter & AC lines
  float ac_out_voltage = this->get_register(data, length, REG_AC_OUT_VOLTAGE) * 0.1f;
  float ac_out_frequency = this->get_register(data, length, REG_AC_OUT_FREQUENCY) * 0.1f;
  
  // Reg 21: AC Input Voltage (x10) or cold protection code 15
  uint16_t reg21_raw = this->get_register(data, length, REG_AC_IN_VOLTAGE);
  float ac_in_voltage = reg21_raw * 0.1f;
  uint16_t system_watts = reg21_raw; // Legacy compatibility
  
  // USB ports power
  float usb_a1_power = this->get_register(data, length, REG_USB_A1_OUT) * 0.1f;
  float usb_a2_power = this->get_register(data, length, REG_USB_A2_OUT) * 0.1f;
  float usb_c1_power = this->get_register(data, length, REG_USB_C1_OUT) * 0.1f;
  float usb_c2_power = this->get_register(data, length, REG_USB_C2_OUT) * 0.1f;
  float usb_c3_power = this->get_register(data, length, REG_USB_C3_OUT) * 0.1f;
  float usb_c4_power = this->get_register(data, length, REG_USB_C4_OUT) * 0.1f;
  
  // 3. Flags and Errors
  uint16_t port_flags = this->get_register(data, length, REG_PORT_FLAGS);
  uint16_t protection_flags = this->get_register(data, length, REG_PROTECTION_FLAGS);
  uint16_t status_flags = this->get_register(data, length, REG_STATUS_FLAGS);
  uint16_t error_code = this->get_register(data, length, REG_ERROR_CODE);
  
  // Timers & Counters
  uint16_t booking_charge_delay = this->get_register(data, length, REG_BOOKING_DELAY);
  uint16_t time_to_full = this->get_register(data, length, REG_TIME_TO_FULL);
  uint16_t time_to_empty = this->get_register(data, length, REG_TIME_TO_EMPTY);
  uint16_t ac_standby_counter = this->get_register(data, length, REG_AC_STANDBY_COUNTER);
  uint16_t dc_standby_counter = this->get_register(data, length, REG_DC_STANDBY_COUNTER);
  uint16_t shutdown_timer = this->get_register(data, length, REG_SHUTDOWN_TIMER_STATUS);
  uint16_t fan_level = this->get_register(data, length, REG_FAN_LEVEL);
  
  // Publish numeric sensors
  if (this->battery_percent_sensor_ != nullptr) this->battery_percent_sensor_->publish_state(battery_percent);
  if (this->battery_percent_s1_sensor_ != nullptr) this->battery_percent_s1_sensor_->publish_state(battery_percent_s1);
  if (this->battery_percent_s2_sensor_ != nullptr) this->battery_percent_s2_sensor_->publish_state(battery_percent_s2);
  if (this->battery_percent_s3_sensor_ != nullptr) this->battery_percent_s3_sensor_->publish_state(battery_percent_s3);
  if (this->battery_percent_s4_sensor_ != nullptr) this->battery_percent_s4_sensor_->publish_state(battery_percent_s4);
  if (this->battery_capacity_sensor_ != nullptr) this->battery_capacity_sensor_->publish_state(battery_capacity);
  if (this->battery_voltage_sensor_ != nullptr) this->battery_voltage_sensor_->publish_state(battery_voltage);
  if (this->ac_input_power_sensor_ != nullptr) this->ac_input_power_sensor_->publish_state(ac_input_watts);
  if (this->dc_input_power_sensor_ != nullptr) this->dc_input_power_sensor_->publish_state(dc_input_watts);
  if (this->input_power_sensor_ != nullptr) this->input_power_sensor_->publish_state(input_watts);
  if (this->ac_grid_power_sensor_ != nullptr) this->ac_grid_power_sensor_->publish_state(ac_grid_power);
  if (this->output_power_sensor_ != nullptr) this->output_power_sensor_->publish_state(output_watts);
  if (this->system_power_sensor_ != nullptr) this->system_power_sensor_->publish_state(system_watts);
  if (this->total_power_sensor_ != nullptr) this->total_power_sensor_->publish_state(total_watts);
  if (this->charge_level_sensor_ != nullptr) this->charge_level_sensor_->publish_state(charge_level_watts);
  if (this->ac_out_voltage_sensor_ != nullptr) this->ac_out_voltage_sensor_->publish_state(ac_out_voltage);
  if (this->ac_out_frequency_sensor_ != nullptr) this->ac_out_frequency_sensor_->publish_state(ac_out_frequency);
  if (this->ac_in_voltage_sensor_ != nullptr) this->ac_in_voltage_sensor_->publish_state(ac_in_voltage);
  if (this->time_to_full_sensor_ != nullptr) this->time_to_full_sensor_->publish_state(time_to_full);
  if (this->remaining_time_sensor_ != nullptr) this->remaining_time_sensor_->publish_state(time_to_empty);
  if (this->time_to_empty_sensor_ != nullptr) this->time_to_empty_sensor_->publish_state(time_to_empty);
  if (this->booking_charge_countdown_sensor_ != nullptr) this->booking_charge_countdown_sensor_->publish_state(booking_charge_delay);
  if (this->ac_standby_counter_sensor_ != nullptr) this->ac_standby_counter_sensor_->publish_state(ac_standby_counter);
  if (this->dc_standby_counter_sensor_ != nullptr) this->dc_standby_counter_sensor_->publish_state(dc_standby_counter);
  if (this->shutdown_timer_countdown_sensor_ != nullptr) this->shutdown_timer_countdown_sensor_->publish_state(shutdown_timer);
  if (this->fan_level_sensor_ != nullptr) this->fan_level_sensor_->publish_state(fan_level);
  if (this->error_code_sensor_ != nullptr) this->error_code_sensor_->publish_state(error_code);
  if (this->protection_flags_sensor_ != nullptr) this->protection_flags_sensor_->publish_state(protection_flags);
  if (this->status_flags_sensor_ != nullptr) this->status_flags_sensor_->publish_state(status_flags);
  
  if (this->usb_a1_power_sensor_ != nullptr) this->usb_a1_power_sensor_->publish_state(usb_a1_power);
  if (this->usb_a2_power_sensor_ != nullptr) this->usb_a2_power_sensor_->publish_state(usb_a2_power);
  if (this->usb_c1_power_sensor_ != nullptr) this->usb_c1_power_sensor_->publish_state(usb_c1_power);
  if (this->usb_c2_power_sensor_ != nullptr) this->usb_c2_power_sensor_->publish_state(usb_c2_power);
  if (this->usb_c3_power_sensor_ != nullptr) this->usb_c3_power_sensor_->publish_state(usb_c3_power);
  if (this->usb_c4_power_sensor_ != nullptr) this->usb_c4_power_sensor_->publish_state(usb_c4_power);

  // Binary sensors for battery connections
  if (this->battery_connected_s1_binary_sensor_ != nullptr) this->battery_connected_s1_binary_sensor_->publish_state(battery_s1_connected);
  if (this->battery_connected_s2_binary_sensor_ != nullptr) this->battery_connected_s2_binary_sensor_->publish_state(battery_s2_connected);
  if (this->battery_connected_s3_binary_sensor_ != nullptr) this->battery_connected_s3_binary_sensor_->publish_state(battery_s3_connected);
  if (this->battery_connected_s4_binary_sensor_ != nullptr) this->battery_connected_s4_binary_sensor_->publish_state(battery_s4_connected);
  
  // Output states from port flags
  bool usb_state = (port_flags & STATE_USB_BIT) != 0;
  bool dc_state = (port_flags & STATE_DC_BIT) != 0;
  bool ac_state = (port_flags & STATE_AC_BIT) != 0;
  bool light_state = (port_flags & STATE_LIGHT_BIT) != 0;
  
  if (this->usb_active_binary_sensor_ != nullptr) this->usb_active_binary_sensor_->publish_state(usb_state);
  if (this->dc_active_binary_sensor_ != nullptr) this->dc_active_binary_sensor_->publish_state(dc_state);
  if (this->ac_active_binary_sensor_ != nullptr) this->ac_active_binary_sensor_->publish_state(ac_state);
  if (this->light_active_binary_sensor_ != nullptr) this->light_active_binary_sensor_->publish_state(light_state);
  
  // Detailed subsystem flags (Reg 41)
  bool ac_out_active = (port_flags & PORT_FLAG_AC_OUT_ACTIVE) != 0;
  bool ac_in_present = (port_flags & PORT_FLAG_AC_IN_PRESENT) != 0;
  bool ac_in_charging = (port_flags & PORT_FLAG_AC_IN_CHARGING) != 0;
  bool low_pv_present = (port_flags & PORT_FLAG_LOW_PV_PRESENT) != 0;
  bool low_pv_charging = (port_flags & PORT_FLAG_LOW_PV_CHARGING) != 0;
  bool dc_out_active = (port_flags & PORT_FLAG_DC_OUT_ACTIVE) != 0;
  bool car_in_present = (port_flags & PORT_FLAG_CAR_IN_PRESENT) != 0;
  bool car_charging = (port_flags & PORT_FLAG_CAR_CHARGING) != 0;
  bool high_pv_present = (port_flags & PORT_FLAG_HIGH_PV_PRESENT) != 0;
  bool high_pv_charging = (port_flags & PORT_FLAG_HIGH_PV_CHARGING) != 0;
  bool ups_bypass_active = ((port_flags & 0x000C) == 0x000C);
  
  if (this->ac_out_active_binary_sensor_ != nullptr) this->ac_out_active_binary_sensor_->publish_state(ac_out_active);
  if (this->ac_in_present_binary_sensor_ != nullptr) this->ac_in_present_binary_sensor_->publish_state(ac_in_present);
  if (this->ac_in_charging_binary_sensor_ != nullptr) this->ac_in_charging_binary_sensor_->publish_state(ac_in_charging);
  if (this->low_pv_in_present_binary_sensor_ != nullptr) this->low_pv_in_present_binary_sensor_->publish_state(low_pv_present);
  if (this->low_pv_charging_binary_sensor_ != nullptr) this->low_pv_charging_binary_sensor_->publish_state(low_pv_charging);
  if (this->dc_out_active_binary_sensor_ != nullptr) this->dc_out_active_binary_sensor_->publish_state(dc_out_active);
  if (this->car_in_present_binary_sensor_ != nullptr) this->car_in_present_binary_sensor_->publish_state(car_in_present);
  if (this->car_charging_binary_sensor_ != nullptr) this->car_charging_binary_sensor_->publish_state(car_charging);
  if (this->high_pv_in_present_binary_sensor_ != nullptr) this->high_pv_in_present_binary_sensor_->publish_state(high_pv_present);
  if (this->high_pv_charging_binary_sensor_ != nullptr) this->high_pv_charging_binary_sensor_->publish_state(high_pv_charging);
  if (this->ups_bypass_active_binary_sensor_ != nullptr) this->ups_bypass_active_binary_sensor_->publish_state(ups_bypass_active);
  
  // Status flags (Reg 48)
  bool is_charging = (status_flags & STATUS_FLAG_AC_CHARGING) != 0;
  bool is_standby = (status_flags & STATUS_FLAG_INVERTER_STANDBY) != 0;
  if (this->charging_active_binary_sensor_ != nullptr) this->charging_active_binary_sensor_->publish_state(is_charging);
  if (this->inverter_standby_binary_sensor_ != nullptr) this->inverter_standby_binary_sensor_->publish_state(is_standby);
  
  // Fault & Safety Protections (Reg 8 + Reg 42)
  bool is_crit_fault = (error_code == 79 && (protection_flags & PROT_FLAG_CRITICAL_MASK) > 0);
  bool is_temp_prot = ((error_code == 79 && (protection_flags & PROT_FLAG_CRITICAL_MASK) == 0) || reg21_raw == 15);
  bool is_inverter_fault = (error_code == 78);
  
  if (this->critical_fault_binary_sensor_ != nullptr) this->critical_fault_binary_sensor_->publish_state(is_crit_fault);
  if (this->temp_protection_binary_sensor_ != nullptr) this->temp_protection_binary_sensor_->publish_state(is_temp_prot);
  if (this->inverter_fault_binary_sensor_ != nullptr) this->inverter_fault_binary_sensor_->publish_state(is_inverter_fault);

#ifdef USE_TEXT_SENSOR
  if (this->system_status_text_sensor_ != nullptr) {
    std::string sys_status = "Normal";
    if (is_crit_fault) {
      sys_status = "Critical Hardware Fault (Error 79)";
    } else if (is_temp_prot) {
      sys_status = "Temperature Protection Active";
    } else if (is_inverter_fault) {
      sys_status = "Inverter Fault (Error 78)";
    } else if (is_charging) {
      sys_status = "Charging";
    } else if (is_standby) {
      sys_status = "Standby";
    }
    this->system_status_text_sensor_->publish_state(sys_status);
  }
#endif
  
  // Sync switches
  if (this->usb_switch_ != nullptr) this->usb_switch_->publish_state(usb_state);
  if (this->dc_switch_ != nullptr) this->dc_switch_->publish_state(dc_state);
  if (this->ac_switch_ != nullptr) this->ac_switch_->publish_state(ac_state);
  if (this->light_switch_ != nullptr) this->light_switch_->publish_state(light_state);
  
  ESP_LOGD(TAG, "SOC: %.1f%% (S1:%.1f%% S2:%.1f%% S3:%.1f%% S4:%.1f%%), In: %dW, Out: %dW, Grid: %dW, Fan: %d, Err: %d", 
           battery_percent, battery_percent_s1, battery_percent_s2, battery_percent_s3, battery_percent_s4,
           input_watts, output_watts, ac_grid_power, fan_level, error_code);
}

void Fbot::parse_settings_notification(const uint8_t *data, uint16_t length) {
  ESP_LOGD(TAG, "Received settings notification");
  
  if (!this->settings_received_) {
    this->settings_received_ = true;
    ESP_LOGI(TAG, "First settings response received");
  }
  
  // Hardware ID & Capabilities
  uint16_t hardware_id = this->get_register(data, length, REG_HARDWARE_ID);
  uint16_t max_ac_input = this->get_register(data, length, REG_MAX_AC_INPUT);
  uint16_t max_charge_current = this->get_register(data, length, REG_MAX_CHARGE_CURRENT);
  uint16_t charge_current_setting = this->get_register(data, length, REG_CHARGE_CURRENT_SET);
  
  if (this->hardware_id_sensor_ != nullptr) this->hardware_id_sensor_->publish_state(hardware_id);
  if (this->max_ac_input_sensor_ != nullptr) this->max_ac_input_sensor_->publish_state(max_ac_input);
  if (this->max_charge_current_sensor_ != nullptr) this->max_charge_current_sensor_->publish_state(max_charge_current);
  
  // MCU Firmware Versions ((val & 0xFF) / 10.0f)
  float ac_mcu_ver = roundf(((this->get_register(data, length, REG_AC_MCU_VERSION) & 0xFF) / 10.0f) * 10.0f) / 10.0f;
  float bms_mcu_ver = roundf(((this->get_register(data, length, REG_BMS_MCU_VERSION) & 0xFF) / 10.0f) * 10.0f) / 10.0f;
  float pv_mcu_ver = roundf(((this->get_register(data, length, REG_PV_MCU_VERSION) & 0xFF) / 10.0f) * 10.0f) / 10.0f;
  float dc_mcu_ver = roundf(((this->get_register(data, length, REG_DC_MCU_VERSION) & 0xFF) / 10.0f) * 10.0f) / 10.0f;
  
  if (this->ac_mcu_version_sensor_ != nullptr) this->ac_mcu_version_sensor_->publish_state(ac_mcu_ver);
  if (this->bms_mcu_version_sensor_ != nullptr) this->bms_mcu_version_sensor_->publish_state(bms_mcu_ver);
  if (this->pv_mcu_version_sensor_ != nullptr) this->pv_mcu_version_sensor_->publish_state(pv_mcu_ver);
  if (this->dc_mcu_version_sensor_ != nullptr) this->dc_mcu_version_sensor_->publish_state(dc_mcu_ver);

  // AC Silent Mode
  bool ac_silent_state = (this->get_register(data, length, REG_AC_SILENT_CONTROL) == 1);
  if (this->ac_silent_switch_ != nullptr) {
    this->ac_silent_switch_->publish_state(ac_silent_state);
  }
  
  // Key Sound
  bool key_sound_state = (this->get_register(data, length, REG_KEY_SOUND) == 1);
  if (this->key_sound_switch_ != nullptr) {
    this->key_sound_switch_->publish_state(key_sound_state);
  }
  
  // Light Mode: 0=Off, 1=On, 2=Flash, 3=SOS (Matches PROTOCOL.md and Golden Packets)
  uint16_t light_mode_value = this->get_register(data, length, REG_LIGHT_CONTROL);
  
#ifdef USE_SELECT
  if (this->light_mode_select_ != nullptr) {
    std::string light_mode;
    switch (light_mode_value) {
      case 0: light_mode = "Off"; break;
      case 1: light_mode = "On"; break;
      case 2: light_mode = "Flash"; break;
      case 3: light_mode = "SOS"; break;
      default: light_mode = "Off"; break;
    }
    this->light_mode_select_->publish_state(light_mode);
  }
  
  // AC Charge Limit select (register 13: values 1-5)
  uint16_t ac_charge_limit_value = this->get_register(data, length, REG_AC_CHARGE_LIMIT);
  if (this->ac_charge_limit_select_ != nullptr && ac_charge_limit_value >= 1 && ac_charge_limit_value <= 5) {
    const auto &options = this->ac_charge_limit_select_->traits.get_options();
    size_t index = ac_charge_limit_value - 1;
    if (index < options.size()) {
      this->ac_charge_limit_select_->publish_state(options[index]);
    }
  }

  // Machine Shutdown select
  uint16_t shutdown_val = this->get_register(data, length, REG_SLEEP_TIMER);
  if (this->machine_shutdown_select_ != nullptr) {
    std::string opt;
    if (shutdown_val == 5) opt = "5 min";
    else if (shutdown_val == 10) opt = "10 min";
    else if (shutdown_val == 30) opt = "30 min";
    else if (shutdown_val == 60) opt = "60 min";
    else if (shutdown_val == 480) opt = "480 min (8h)";
    if (!opt.empty()) {
      this->machine_shutdown_select_->publish_state(opt);
    }
  }
#endif
  
  // Thresholds (Registers 66 and 67, values in permille / 10 for percentage)
  float threshold_discharge = this->get_register(data, length, REG_THRESHOLD_DISCHARGE) / 10.0f;
  float threshold_charge = this->get_register(data, length, REG_THRESHOLD_CHARGE) / 10.0f;
  
  if (this->threshold_discharge_sensor_ != nullptr) this->threshold_discharge_sensor_->publish_state(threshold_discharge);
  if (this->threshold_charge_sensor_ != nullptr) this->threshold_charge_sensor_->publish_state(threshold_charge);
  
#ifdef USE_NUMBER
  if (this->threshold_discharge_number_ != nullptr) this->threshold_discharge_number_->publish_state(threshold_discharge);
  if (this->threshold_charge_number_ != nullptr) this->threshold_charge_number_->publish_state(threshold_charge);

  uint16_t sleep_timer_raw = this->get_register(data, length, REG_SLEEP_TIMER);
  if (this->sleep_timer_number_ != nullptr) {
    this->sleep_timer_number_->publish_state(static_cast<float>(sleep_timer_raw));
  }
  
  if (this->charge_current_number_ != nullptr) {
    this->charge_current_number_->publish_state(static_cast<float>(charge_current_setting));
  }
  
  uint16_t screen_timeout = this->get_register(data, length, REG_SCREEN_TIMEOUT);
  if (this->screen_timeout_number_ != nullptr) {
    this->screen_timeout_number_->publish_state(static_cast<float>(screen_timeout));
  }
  
  uint16_t booking_charge = this->get_register(data, length, REG_BOOKING_CHARGE);
  if (this->booking_charge_number_ != nullptr) {
    this->booking_charge_number_->publish_state(static_cast<float>(booking_charge));
  }
#endif

  ESP_LOGD(TAG, "Settings: Discharge: %.1f%%, Charge: %.1f%%, AC Silent: %d, Key Sound: %d, Sleep: %d min",
           threshold_discharge, threshold_charge, ac_silent_state, key_sound_state, shutdown_val);
}

void Fbot::update_connected_state(bool state) {
  this->connected_ = state;
  if (this->connected_binary_sensor_ != nullptr) {
    this->connected_binary_sensor_->publish_state(state);
  }
}

// Control methods
void Fbot::control_usb(bool state) {
  this->send_control_command(REG_USB_CONTROL, state ? 1 : 0);
}

void Fbot::control_dc(bool state) {
  this->send_control_command(REG_DC_CONTROL, state ? 1 : 0);
}

void Fbot::control_ac(bool state) {
  this->send_control_command(REG_AC_CONTROL, state ? 1 : 0);
}

void Fbot::control_light(bool state) {
  this->send_control_command(REG_LIGHT_CONTROL, state ? 1 : 0);
}

void Fbot::control_light_mode(const std::string &value) {
  uint16_t mode_value = 0;
  if (value == "Off") {
    mode_value = 0;
  } else if (value == "On") {
    mode_value = 1;
  } else if (value == "Flash" || value == "Flashing") {
    mode_value = 2;
  } else if (value == "SOS") {
    mode_value = 3;
  } else {
    ESP_LOGW(TAG, "Unknown light mode: %s", value.c_str());
    return;
  }
  
  ESP_LOGI(TAG, "Setting light mode to: %s (value: %d)", value.c_str(), mode_value);
  this->send_control_command(REG_LIGHT_CONTROL, mode_value);
  
  if (this->light_switch_ != nullptr) {
    this->light_switch_->publish_state(mode_value != 0);
  }
}

void Fbot::control_ac_charge_limit(const std::string &value) {
#ifdef USE_SELECT
  if (this->ac_charge_limit_select_ == nullptr) {
    ESP_LOGW(TAG, "AC Charge Limit select not configured");
    return;
  }
  
  const auto &options = this->ac_charge_limit_select_->traits.get_options();
  for (size_t i = 0; i < options.size(); i++) {
    if (options[i] == value) {
      uint16_t reg_value = i + 1;
      if (reg_value < 1 || reg_value > 5) {
        ESP_LOGW(TAG, "Invalid AC Charge Limit value: %d", reg_value);
        return;
      }
      
      ESP_LOGI(TAG, "Setting AC Charge Limit to: %s (value: %d)", value.c_str(), reg_value);
      this->send_control_command(REG_AC_CHARGE_LIMIT, reg_value);
      this->set_timeout(500, [this]() { this->send_settings_request(); });
      return;
    }
  }
  
  ESP_LOGW(TAG, "Unknown AC Charge Limit option: %s", value.c_str());
#endif
}

void Fbot::control_machine_shutdown_select(const std::string &value) {
#ifdef USE_SELECT
  uint16_t minutes = 0;
  if (value == "5 min") minutes = 5;
  else if (value == "10 min") minutes = 10;
  else if (value == "30 min") minutes = 30;
  else if (value == "60 min") minutes = 60;
  else if (value == "480 min (8h)") minutes = 480;
  else {
    ESP_LOGW(TAG, "Unknown machine shutdown option: %s", value.c_str());
    return;
  }
  
  this->set_sleep_timer(static_cast<float>(minutes));
#endif
}

void Fbot::control_ac_silent(bool state) {
  this->send_control_command(REG_AC_SILENT_CONTROL, state ? 1 : 0);
  this->set_timeout(500, [this]() { this->send_settings_request(); });
}

void Fbot::control_key_sound(bool state) {
  this->send_control_command(REG_KEY_SOUND, state ? 1 : 0);
  this->set_timeout(500, [this]() { this->send_settings_request(); });
}

void Fbot::set_threshold_charge(float percent) {
  uint16_t value = static_cast<uint16_t>(percent * 10);
  if (value < 100) { value = 100; }
  if (value > 1000) { value = 1000; }
  this->send_control_command(REG_THRESHOLD_CHARGE, value);
}

void Fbot::set_threshold_discharge(float percent) {
  uint16_t value = static_cast<uint16_t>(percent * 10);
  if (value < 0) { value = 0; }
  if (value > 500) { value = 500; }
  this->send_control_command(REG_THRESHOLD_DISCHARGE, value);
}

void Fbot::set_sleep_timer(float minutes) {
  uint16_t value = static_cast<uint16_t>(minutes);
  
  // CRITICAL BRICK PREVENTION: Reg 68 = 0 permanently bricks Fossibot hardware!
  if (value == 0) {
    ESP_LOGE(TAG, "BLOCKED WRITE: Reg 68 (Machine Shutdown) = 0 is forbidden to protect hardware against permanent bricking!");
    return;
  }
  
  if (value < 5) {
    ESP_LOGW(TAG, "Value %d min below minimum allowed 5 min, adjusting to 5 min", value);
    value = 5;
  }
  
  this->send_control_command(REG_SLEEP_TIMER, value);
  this->set_timeout(500, [this]() { this->send_settings_request(); });
}

void Fbot::set_charge_current(float amps) {
  uint16_t value = static_cast<uint16_t>(amps);
  if (value < 1) value = 1;
  if (value > 20) value = 20;
  this->send_control_command(REG_CHARGE_CURRENT_SET, value);
  this->set_timeout(500, [this]() { this->send_settings_request(); });
}

void Fbot::set_screen_timeout(float seconds) {
  uint16_t value = static_cast<uint16_t>(seconds);
  this->send_control_command(REG_SCREEN_TIMEOUT, value);
  this->set_timeout(500, [this]() { this->send_settings_request(); });
}

void Fbot::set_booking_charge(float minutes) {
  uint16_t value = static_cast<uint16_t>(minutes);
  if (value > 1440) value = 1440;
  this->send_control_command(REG_BOOKING_CHARGE, value);
  this->set_timeout(500, [this]() { this->send_settings_request(); });
}

void Fbot::control_power_off() {
  ESP_LOGW(TAG, "Executing soft power off command (Reg 64 = 1)");
  this->send_control_command(REG_POWER_OFF, 1);
}

void Fbot::control_factory_reset() {
  ESP_LOGW(TAG, "Executing factory reset / unbind command (Reg 0 = 1)");
  this->send_control_command(REG_FACTORY_RESET, 1);
}

void Fbot::set_wifi_credentials(const std::string &ssid, const std::string &password) {
  if (!this->connected_ || !this->characteristics_discovered_) {
    ESP_LOGW(TAG, "Cannot set WiFi credentials: not connected");
    return;
  }
  
  if (ssid.length() == 0 || ssid.length() > 255) {
    ESP_LOGW(TAG, "Invalid SSID length: %d", ssid.length());
    return;
  }
  if (password.length() == 0 || password.length() > 255) {
    ESP_LOGW(TAG, "Invalid password length: %d", password.length());
    return;
  }
  
  uint8_t ssid_len = static_cast<uint8_t>(ssid.length());
  uint8_t pass_len = static_cast<uint8_t>(password.length());
  size_t packet_size = 4 + ssid_len + pass_len + 2;
  uint8_t *packet = new uint8_t[packet_size];
  
  packet[0] = 0x11;  // Header
  packet[1] = 0x07;  // OpCode for WiFi configuration
  packet[2] = ssid_len;
  packet[3] = pass_len;
  
  memcpy(&packet[4], ssid.c_str(), ssid_len);
  memcpy(&packet[4 + ssid_len], password.c_str(), pass_len);
  
  uint16_t crc = this->calculate_checksum(packet, packet_size - 2);
  packet[packet_size - 2] = (crc >> 8) & 0xFF;
  packet[packet_size - 1] = crc & 0xFF;
  
  auto status = esp_ble_gattc_write_char(this->parent()->get_gattc_if(), this->parent()->get_conn_id(),
                                         this->write_handle_, packet_size, packet,
                                         ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);
  
  if (status) {
    ESP_LOGW(TAG, "Error sending WiFi credentials, status=%d", status);
  } else {
    ESP_LOGI(TAG, "Sent WiFi credentials: SSID='%s' (len=%d), Password length=%d", 
             ssid.c_str(), ssid_len, pass_len);
  }
  
  delete[] packet;
}

void Fbot::check_poll_timeout() {
  if (this->last_poll_time_ == 0) {
    return;
  }
  
  if (this->consecutive_poll_failures_ >= this->max_poll_failures_) {
    return;
  }
  
  uint32_t now = millis();
  uint32_t time_since_success = (this->last_successful_poll_ > 0) ? (now - this->last_successful_poll_) : 0;
  
  if (this->last_successful_poll_ > 0 && time_since_success > this->poll_timeout_ms_) {
    uint32_t time_since_poll = now - this->last_poll_time_;
    if (time_since_poll >= this->polling_interval_) {
      this->consecutive_poll_failures_++;
      ESP_LOGW(TAG, "Poll timeout detected (failure %d/%d)", this->consecutive_poll_failures_, this->max_poll_failures_);
      this->last_successful_poll_ = now;
      
      if (this->consecutive_poll_failures_ >= this->max_poll_failures_) {
        // Link is up but the power station stopped answering: drop it so the BLE
        // client reconnects. Sensor values are kept for the grace period (see loop()).
        ESP_LOGW(TAG, "Max poll failures reached - forcing reconnect");
        this->consecutive_poll_failures_ = 0;
        this->parent()->disconnect();
      }
    }
  }
}

void Fbot::reset_sensors_to_unknown() {
  if (this->battery_percent_sensor_ != nullptr) this->battery_percent_sensor_->publish_state(NAN);
  if (this->battery_percent_s1_sensor_ != nullptr) this->battery_percent_s1_sensor_->publish_state(NAN);
  if (this->battery_percent_s2_sensor_ != nullptr) this->battery_percent_s2_sensor_->publish_state(NAN);
  if (this->battery_percent_s3_sensor_ != nullptr) this->battery_percent_s3_sensor_->publish_state(NAN);
  if (this->battery_percent_s4_sensor_ != nullptr) this->battery_percent_s4_sensor_->publish_state(NAN);
  if (this->battery_capacity_sensor_ != nullptr) this->battery_capacity_sensor_->publish_state(NAN);
  if (this->battery_voltage_sensor_ != nullptr) this->battery_voltage_sensor_->publish_state(NAN);
  if (this->ac_input_power_sensor_ != nullptr) this->ac_input_power_sensor_->publish_state(NAN);
  if (this->dc_input_power_sensor_ != nullptr) this->dc_input_power_sensor_->publish_state(NAN);
  if (this->input_power_sensor_ != nullptr) this->input_power_sensor_->publish_state(NAN);
  if (this->ac_grid_power_sensor_ != nullptr) this->ac_grid_power_sensor_->publish_state(NAN);
  if (this->output_power_sensor_ != nullptr) this->output_power_sensor_->publish_state(NAN);
  if (this->system_power_sensor_ != nullptr) this->system_power_sensor_->publish_state(NAN);
  if (this->total_power_sensor_ != nullptr) this->total_power_sensor_->publish_state(NAN);
  if (this->remaining_time_sensor_ != nullptr) this->remaining_time_sensor_->publish_state(NAN);
  if (this->time_to_empty_sensor_ != nullptr) this->time_to_empty_sensor_->publish_state(NAN);
  if (this->charge_level_sensor_ != nullptr) this->charge_level_sensor_->publish_state(NAN);
  if (this->ac_out_voltage_sensor_ != nullptr) this->ac_out_voltage_sensor_->publish_state(NAN);
  if (this->ac_out_frequency_sensor_ != nullptr) this->ac_out_frequency_sensor_->publish_state(NAN);
  if (this->ac_in_voltage_sensor_ != nullptr) this->ac_in_voltage_sensor_->publish_state(NAN);
  if (this->ac_in_frequency_sensor_ != nullptr) this->ac_in_frequency_sensor_->publish_state(NAN);
  if (this->time_to_full_sensor_ != nullptr) this->time_to_full_sensor_->publish_state(NAN);
  if (this->booking_charge_countdown_sensor_ != nullptr) this->booking_charge_countdown_sensor_->publish_state(NAN);
  if (this->ac_standby_counter_sensor_ != nullptr) this->ac_standby_counter_sensor_->publish_state(NAN);
  if (this->dc_standby_counter_sensor_ != nullptr) this->dc_standby_counter_sensor_->publish_state(NAN);
  if (this->shutdown_timer_countdown_sensor_ != nullptr) this->shutdown_timer_countdown_sensor_->publish_state(NAN);
  if (this->fan_level_sensor_ != nullptr) this->fan_level_sensor_->publish_state(NAN);
  if (this->error_code_sensor_ != nullptr) this->error_code_sensor_->publish_state(NAN);
  if (this->protection_flags_sensor_ != nullptr) this->protection_flags_sensor_->publish_state(NAN);
  if (this->status_flags_sensor_ != nullptr) this->status_flags_sensor_->publish_state(NAN);
  if (this->hardware_id_sensor_ != nullptr) this->hardware_id_sensor_->publish_state(NAN);
  if (this->max_ac_input_sensor_ != nullptr) this->max_ac_input_sensor_->publish_state(NAN);
  if (this->max_charge_current_sensor_ != nullptr) this->max_charge_current_sensor_->publish_state(NAN);
  if (this->ac_mcu_version_sensor_ != nullptr) this->ac_mcu_version_sensor_->publish_state(NAN);
  if (this->bms_mcu_version_sensor_ != nullptr) this->bms_mcu_version_sensor_->publish_state(NAN);
  if (this->pv_mcu_version_sensor_ != nullptr) this->pv_mcu_version_sensor_->publish_state(NAN);
  if (this->dc_mcu_version_sensor_ != nullptr) this->dc_mcu_version_sensor_->publish_state(NAN);

  if (this->usb_a1_power_sensor_ != nullptr) this->usb_a1_power_sensor_->publish_state(NAN);
  if (this->usb_a2_power_sensor_ != nullptr) this->usb_a2_power_sensor_->publish_state(NAN);
  if (this->usb_c1_power_sensor_ != nullptr) this->usb_c1_power_sensor_->publish_state(NAN);
  if (this->usb_c2_power_sensor_ != nullptr) this->usb_c2_power_sensor_->publish_state(NAN);
  if (this->usb_c3_power_sensor_ != nullptr) this->usb_c3_power_sensor_->publish_state(NAN);
  if (this->usb_c4_power_sensor_ != nullptr) this->usb_c4_power_sensor_->publish_state(NAN);
  
  if (this->usb_active_binary_sensor_ != nullptr) this->usb_active_binary_sensor_->publish_state(false);
  if (this->dc_active_binary_sensor_ != nullptr) this->dc_active_binary_sensor_->publish_state(false);
  if (this->ac_active_binary_sensor_ != nullptr) this->ac_active_binary_sensor_->publish_state(false);
  if (this->light_active_binary_sensor_ != nullptr) this->light_active_binary_sensor_->publish_state(false);
  if (this->ac_out_active_binary_sensor_ != nullptr) this->ac_out_active_binary_sensor_->publish_state(false);
  if (this->ac_in_present_binary_sensor_ != nullptr) this->ac_in_present_binary_sensor_->publish_state(false);
  if (this->ac_in_charging_binary_sensor_ != nullptr) this->ac_in_charging_binary_sensor_->publish_state(false);
  if (this->low_pv_in_present_binary_sensor_ != nullptr) this->low_pv_in_present_binary_sensor_->publish_state(false);
  if (this->low_pv_charging_binary_sensor_ != nullptr) this->low_pv_charging_binary_sensor_->publish_state(false);
  if (this->dc_out_active_binary_sensor_ != nullptr) this->dc_out_active_binary_sensor_->publish_state(false);
  if (this->car_in_present_binary_sensor_ != nullptr) this->car_in_present_binary_sensor_->publish_state(false);
  if (this->car_charging_binary_sensor_ != nullptr) this->car_charging_binary_sensor_->publish_state(false);
  if (this->high_pv_in_present_binary_sensor_ != nullptr) this->high_pv_in_present_binary_sensor_->publish_state(false);
  if (this->high_pv_charging_binary_sensor_ != nullptr) this->high_pv_charging_binary_sensor_->publish_state(false);
  if (this->ups_bypass_active_binary_sensor_ != nullptr) this->ups_bypass_active_binary_sensor_->publish_state(false);
  if (this->charging_active_binary_sensor_ != nullptr) this->charging_active_binary_sensor_->publish_state(false);
  if (this->inverter_standby_binary_sensor_ != nullptr) this->inverter_standby_binary_sensor_->publish_state(false);
  if (this->critical_fault_binary_sensor_ != nullptr) this->critical_fault_binary_sensor_->publish_state(false);
  if (this->temp_protection_binary_sensor_ != nullptr) this->temp_protection_binary_sensor_->publish_state(false);
  if (this->inverter_fault_binary_sensor_ != nullptr) this->inverter_fault_binary_sensor_->publish_state(false);

#ifdef USE_TEXT_SENSOR
  if (this->system_status_text_sensor_ != nullptr) {
    this->system_status_text_sensor_->publish_state("Unavailable");
  }
#endif
}

}  // namespace fbot
}  // namespace esphome

#endif  // USE_ESP32
