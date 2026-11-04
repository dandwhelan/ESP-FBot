# Home Assistant Power Station Interface (ESP-FBot)

![ESP-FBot Image](https://raw.githubusercontent.com/Ylianst/ESP-FBot/refs/heads/main/docs/images/main.png)

This is an ESPHome custom component and standalone bridge device for locally monitoring and controlling portable battery systems and power stations via Bluetooth Low Energy (BLE). It is fully compatible with power stations using the Sydpower/Tuya platform (managed via the "BrightEMS" app), including:

- **FOSSiBOT**: F2400, F3600, F3600 Pro
- **SYDPOWER**: N052, N066
- **AFERIY**: P210 (2400W), P310 (3300W)
- **ABOK**: Power Ark3600

Instead of relying on an internet/cloud connection that fails during power outages, ESP-FBot runs locally on an inexpensive ESP32 / ESP32-S3 microcontroller placed near the battery (10–30 ft) and relays telemetry directly to Home Assistant over Wi-Fi.

---

## Hardware Reference: Waveshare ESP32-S3-RLCD-4.2

This repository features built-in support for the **[Waveshare ESP32-S3-RLCD-4.2](https://www.waveshare.com/wiki/ESP32-S3-RLCD-4.2)** development board, creating a dedicated standalone battery monitor and desktop command center:

- **Microcontroller**: ESP32-S3 (240MHz Xtensa dual-core, 16MB Flash, 8MB Octal PSRAM)
- **Display**: 4.2-inch Reflective LCD (RLCD, 400×300 resolution, ST7305 controller over SPI)
  - Ultra-low power consumption, sunlight-readable, zero-refresh glare.
- **Power & Battery**: Onboard 18650 lithium battery slot with battery charging management & ADC voltage/capacity monitoring on `GPIO4`.
- **Physical Controls**: Onboard `KEY` button on `GPIO18` to toggle between live dashboard and settings pages.
- **Wireless**: Integrated BLE client for real-time power station telemetry + 2.4 GHz Wi-Fi (Home Assistant native API + standalone Web UI).

---

## On-Device Display Pages

The Waveshare 4.2" RLCD runs two full-featured interactive pages toggled with the onboard physical `KEY` button (`GPIO18`):

### 1. Live Dashboard (`Page 1`)
- **Real-Time Input Power**: Total input watts, live AC grid voltage, dedicated AC charge watts, active UPS bypass passthrough detection, and true Solar/DC input power.
- **Battery Gauge & Status**: Center battery outline with proportional fill, percentage, pack voltage (`0.01V` precision), capacity (`Ah`), and dynamic time-to-full / time-to-empty countdowns.
- **Output Power**: Total output watts, AC output voltage & frequency (50/60Hz), and fan speed stage (0–5).
- **Subsystem Badges**: Live indicators for active output ports (`USB`, `AC Inverter`, `DC 12V`, `Light`).
- **Safety Status Banner**: Real-time alerts for Active UPS Bypass, Inverter Trip (`Error 78`), Temperature Freeze/Extreme Protection, or Critical Comparator Fault (`Error 79`).
- **Telemetry Bar & Connectivity**: Current charging limit, board 18650 battery voltage/SOC, and local IP/mDNS address.

### 2. Settings & Power Limits (`Page 2`)
- **Charging & Audio Limits**: Max charging current limit (`1–20A`), AC Silent Mode toggle, Key Beep toggle, Max Charge % ceiling, Min Discharge % floor, Screen dimming timeout, and Max AC input ceiling.
- **Timers & Sub-MCU Versions**: Idle auto-shutdown timer, Scheduled booking delay, AC Inverter MCU version, BMS MCU version, Solar MPPT MCU version, Front Display MCU version, and Regional Hardware ID.


---

## What's New & Register Mapping (from `fossibot-bluetooth`)

This component maps the complete reverse-engineered Sydpower/Fossibot Modbus-over-BLE register set:

### Status Registers (`0x1104` Telemetry)
- **Error & Safety Protection (Regs 8 & 42)**: Real-time safety fault classification:
  - `Error 78`: Inverter Stage Fault (AC tripped; solar MPPT and DC ports remain active)
  - `Error 79` + `(Reg 42 & 0x6000) > 0`: **Critical Hardware Failure**
  - `Error 79` + `(Reg 42 & 0x6000) == 0`: **Temperature Protection** (<0°C cold lockout or >55°C high temperature)
  - Code `136`: Normal healthy operational status (filtered so it doesn't trigger false alarms)
- **AC Input Voltage & Multiplexed Status (Reg 21)**: Measures grid voltage when AC is connected; reports status `15` during cold freeze protection (<0°C).
- **AC Grid Power (Reg 7)**: Signed 16-bit integer for grid import (+) and feed-in/export (-).
- **Expansion Batteries (Regs 53, 55, 66, 67)**: Expansion pack SOC for Ext 1, Ext 2, Ext 3, and Ext 4 (`(val - 10) / 10 %`).
- **Battery Full Capacity (Reg 54)**: Pack capacity in 0.1 Ah (e.g., 76.2 Ah) for State of Health (SOH) tracking.
- **Cooling Fan Level (Reg 69)**: Fan speed levels 0 to 5.
- **Port & Subsystem Active Flags (Reg 41)**: Granular bitmask for AC In Present, AC Charging, Solar Low-PV, Solar High-PV, Car DC In, DC Out, and UPS Bypass Mode.

### Settings Registers (`0x1103` Configuration & Control)
- **Max Charging Current (Reg 20)**: User-adjustable AC charging current (1–20A).
- **Screen Timeout (Reg 62)**: Display sleep countdown (seconds).
- **Booking Charge (Reg 63)**: Scheduled charge delay timer (minutes).
- **Sub-MCU Firmware Versions (Regs 47–50)**: Multi-MCU firmware versions for AC Inverter (`Reg 47`), BMS (`Reg 48`), Solar MPPT (`Reg 49`), and DC/Panel (`Reg 50`).
- **System Power Off (Reg 64)**: Soft shutdown trigger.
- **Factory Reset / Unbind (Reg 0)**: Unbind and restore factory settings.

> [!CAUTION]
> **CRITICAL BRICK PREVENTION — Reg 68 (Machine Shutdown):**
> Writing `0` to Register 68 permanently bricks Fossibot power station mainboards in the field (confirmed issue #1).
> ESP-FBot has hardcoded firmware guards that reject any write of `0` to Register 68, enforcing a safe minimum of 5 minutes. The select option offers only `5 min`, `10 min`, `30 min`, `60 min`, and `480 min (8h)`.

---

## Supported Entities

### Binary Sensors
| Entity | Description |
|:---|:---|
| `connected` | BLE connection established to power station |
| `critical_fault` | Critical hardware comparator fault detected (`Error 79` + `0x6000`) |
| `temp_protection` | Cold (<0°C) or thermal extreme safety lockout active |
| `inverter_fault` | AC output overload or inverter stage trip (`Error 78`) |
| `ups_bypass_active` | Grid bypass active (AC in and AC out simultaneous) |
| `charging_active` | Station is actively charging from AC or Solar |
| `inverter_standby` | AC Inverter is in standby mode |
| `ac_in_present` | AC mains grid detected |
| `ac_in_charging` | AC grid charging active |
| `low_pv_in_present` | Low-voltage solar input connected |
| `high_pv_in_present`| High-voltage solar input connected |
| `car_in_present` | Vehicle DC alternator input connected |
| `usb_active` | USB ports power switch state |
| `dc_active` | 12V DC ports power switch state |
| `ac_active` | 120V/230V AC Inverter state |
| `light_active` | LED ambient/work light state |
| `battery_connected_s1`..`s4` | Expansion batteries 1 through 4 connected |

### Sensors
| Entity | Unit | Description |
|:---|:---|:---|
| `battery_level` | `%` | Main battery State of Charge |
| `battery_s1_level`..`s4_level` | `%` | Expansion battery SOC (Ext 1–4) |
| `battery_capacity` | `Ah` | Total battery capacity rating |
| `battery_voltage` | `V` | Pack voltage |
| `input_power` | `W` | Total input wattage (AC + DC) |
| `ac_input_power` | `W` | AC grid input wattage |
| `dc_input_power` | `W` | Solar/Car DC input wattage |
| `output_power` | `W` | Total output load across all ports |
| `ac_grid_power` | `W` | Grid import (+) / export (-) |
| `total_power` | `W` | Inverter total output power |
| `remaining_time` | `min` | Estimated run time remaining on battery |
| `time_to_full` | `min` | Estimated charge time remaining |
| `fan_level` | `0–5` | Internal forced-air fan speed |
| `ac_out_voltage` | `V` | Inverter AC output voltage |
| `ac_out_frequency` | `Hz` | Inverter output frequency (50/60 Hz) |
| `ac_in_voltage` | `V` | Grid AC input voltage (or code `15`) |
| `usb_a1_power`..`a2_power` | `W` | USB-A port 1 & 2 power |
| `usb_c1_power`..`c4_power` | `W` | USB-C port 1 through 4 power |
| `error_code` | raw | Diagnostic active error code |
| `protection_flags` | hex | Raw hardware protection bitmask |
| `status_flags` | hex | System status bitmask |
| `ac_mcu_version` | ver | Inverter firmware version |
| `bms_mcu_version` | ver | BMS firmware version |
| `pv_mcu_version` | ver | MPPT solar firmware version |
| `dc_mcu_version` | ver | Display panel firmware version |

### Controls (Switches, Numbers, Selects & Buttons)
- **Switches**: `usb`, `dc`, `ac`, `light`, `ac_silent` (quiet fan cap), `key_sound` (buzzer beep).
- **Selects**:
  - `light_mode`: `Off`, `On`, `Flash`, `SOS` (corrected register mapping)
  - `ac_charge_limit`: `300W`, `500W`, `700W`, `900W`, `1100W`
  - `machine_shutdown`: `5 min`, `10 min`, `30 min`, `60 min`, `480 min (8h)`
- **Numbers**:
  - `threshold_charge`: Max charge limit ceiling (10–100%)
  - `threshold_discharge`: Min discharge cutoff floor (0–50%)
  - `charge_current`: Max AC charging current (1–20A)
  - `sleep_timer`: Auto-shutdown timer (5–480 min; protected against 0)
  - `screen_timeout`: Display timeout (0–1800 s)
  - `booking_charge`: Stop charge / scheduled charge delay (0–1440 min)
- **Buttons**:
  - `power_off`: Soft power down unit
  - `factory_reset`: Factory reset & unbind
- **Text Sensor**:
  - `system_status`: High-level state (`Normal`, `Charging`, `Standby`, `Temp Protection`, `Critical Hardware Fault`)

---

## Sample ESPHome Configuration

```yaml
esphome:
  name: fossibot-esp32
  friendly_name: Fossibot Controller

esp32:
  board: esp32-s3-devkitc-1   # Use esp32dev for classic ESP32, or esp32-s3-devkitc-1 for ESP32-S3
  framework:
    type: esp-idf

logger:
  hardware_uart: USB_SERIAL_JTAG   # Required for ESP32-S3 native USB
  level: INFO

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

api:
  encryption:
    key: !secret api_key

ota:
  - platform: esphome
    password: !secret ota_password

external_components:
  - source: github://ylianst/esp-fbot
    refresh: 10s

ble_client:
  - mac_address: "AA:BB:CC:DD:EE:FF"   # Replace with your Fossibot BLE MAC
    id: fbot_ble

fbot:
  id: my_fbot
  ble_client_id: fbot_ble
  polling_interval: 2s
  settings_polling_interval: 30s

sensor:
  - platform: fbot
    fbot_id: my_fbot
    battery_level:
      name: "Battery SOC"
    battery_capacity:
      name: "Battery Capacity"
    battery_voltage:
      name: "Battery Voltage"
    input_power:
      name: "Total Input Power"
    ac_input_power:
      name: "AC Input Power"
    dc_input_power:
      name: "DC Input Power"
    output_power:
      name: "Output Power"
    total_power:
      name: "Total Output Power"
    ac_grid_power:
      name: "AC Grid Power"
    remaining_time:
      name: "Remaining Time"
    time_to_full:
      name: "Time to Full"
    ac_out_voltage:
      name: "AC Output Voltage"
    fan_level:
      name: "Fan Speed Level"
    error_code:
      name: "Active Error Code"

binary_sensor:
  - platform: fbot
    fbot_id: my_fbot
    connected:
      name: "Connected"
    critical_fault:
      name: "Critical Fault"
    temp_protection:
      name: "Temperature Protection"
    inverter_fault:
      name: "Inverter Fault"
    ups_bypass_active:
      name: "UPS Bypass Active"
    charging_active:
      name: "Charging Active"
    usb_active:
      name: "USB Active"
    dc_active:
      name: "DC Active"
    ac_active:
      name: "AC Active"

switch:
  - platform: fbot
    fbot_id: my_fbot
    usb:
      name: "USB Output"
    dc:
      name: "DC Output"
    ac:
      name: "AC Inverter"
    light:
      name: "Light"
    ac_silent:
      name: "Silent Charging"
    key_sound:
      name: "Button Beep"

select:
  - platform: fbot
    fbot_id: my_fbot
    light_mode:
      name: "Light Mode"
    ac_charge_limit:
      name: "AC Charge Limit"
    machine_shutdown:
      name: "Machine Shutdown Timer"

number:
  - platform: fbot
    fbot_id: my_fbot
    threshold_charge:
      name: "Max Charge %"
    threshold_discharge:
      name: "Min Discharge %"
    charge_current:
      name: "Charge Current Setting"
    sleep_timer:
      name: "Machine Shutdown Minutes"

button:
  - platform: fbot
    fbot_id: my_fbot
    power_off:
      name: "Soft Power Off"

text_sensor:
  - platform: fbot
    fbot_id: my_fbot
    system_status:
      name: "System Status"
```
