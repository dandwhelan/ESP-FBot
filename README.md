# ESP-FBot: ESPHome Component for Fossibot Power Stations

A custom ESPHome component for controlling and monitoring Fossibot F2400 (and compatible) portable power stations via Bluetooth Low Energy (BLE).

## Features

### 📊 Sensors (Read-Only Monitoring)
| Sensor | Description |
|--------|-------------|
| Battery | Main battery percentage (0-100%) |
| Battery S1/S2 | Extra battery pack percentages (if connected) |
| AC/DC Input Power | Power from AC and DC charging sources (W) |
| Total Input Power | Combined input power (W) |
| Output Power | Power being drawn by loads (W) |
| System Power | Internal system consumption (W) |
| Total Output Power | Combined output power (W) |
| Remaining Minutes | Estimated runtime remaining |
| Time to Full Charge | Minutes until fully charged |
| AC Output Voltage/Frequency | Inverter output specs |
| Charge/Discharge Threshold | Current charge limit settings |
| Screen Timeout | Current screen timeout value |
| AC/DC/USB Standby | Current standby timer values |

### 🔌 Switches (Controls)
| Switch | Description |
|--------|-------------|
| USB Output | Toggle USB ports on/off |
| DC Output | Toggle DC outputs on/off |
| AC Inverter | Toggle AC inverter on/off |
| Light | Toggle built-in light on/off |
| AC Silent Charging | Enable/disable silent charging mode |
| System Beeps | Enable/disable key sounds |

### ⚙️ Configuration (Settings)
| Setting | Description |
|---------|-------------|
| Polling Interval | How often to poll battery status (2-60s) |
| Settings Polling Interval | How often to poll device settings (2-60s) |
| Set Screen Timeout | Screen auto-off timer (0-60 min) |
| Set AC Standby | AC auto-off when idle (0-1440 min) |
| Set DC Standby | DC auto-off when idle (0-1440 min) |
| Set USB Standby | USB auto-off when idle (0-3600 sec) |
| Set Start Charging | Delayed charge start timer |
| Charge Max | Maximum charge percentage (10-100%) |
| Discharge Min | Minimum discharge percentage (0-50%) |

### 🔘 Buttons (One-Shot Commands)
| Button | Description |
|--------|-------------|
| Power Off | Shuts down the power station |

### 💡 Binary Sensors (Status Indicators)
| Sensor | Description |
|--------|-------------|
| Connected | BLE connection status |
| Connected S1/S2 | Extra battery connection status |
| USB/DC/AC Active | Output status indicators |
| Light Active | Light status |
| AC Silent Charging Active | Silent mode status |

## Installation

### Prerequisites
- ESP32 development board
- ESPHome 2024.1.0 or later
- Home Assistant (optional, for dashboard integration)

### Setup

1. **Clone this repository** into your ESPHome directory:
   ```bash
   git clone https://github.com/dandwhelan/ESP-FBot.git
   ```

2. **Create your configuration file** based on `controller-example.yaml`:
   ```bash
   cp controller-example.yaml fossibot-controller.yaml
   ```

3. **Create a `secrets.yaml`** file with your WiFi credentials:
   ```yaml
   wifi_ssid: "YourWiFiName"
   wifi_password: "YourWiFiPassword"
   ```

4. **Update the MAC address** in your config file to match your Fossibot:
   ```yaml
   ble_client:
     - mac_address: "XX:XX:XX:XX:XX:XX"
   ```

5. **Compile and upload**:
   ```bash
   esphome run fossibot-controller.yaml
   ```

## Finding Your Fossibot MAC Address

Use a BLE scanner app (like "nRF Connect" on Android/iOS) to find your device. It will typically appear as "FBOT_XXXX" where XXXX is the last 4 characters of the MAC address.

## Protocol Details

This component communicates with the Fossibot using a Modbus-like protocol over BLE:

- **Service UUID**: `0000a002-0000-1000-8000-00805f9b34fb`
- **Write Characteristic**: `0000c304-0000-1000-8000-00805f9b34fb`
- **Notify Characteristic**: `0000c305-0000-1000-8000-00805f9b34fb`

### Register Types
- **Input Registers (0x1104)**: Real-time status data (battery, power, etc.)
- **Holding Registers (0x1103)**: Configuration settings
- **Control Registers**: Write-only commands for switches

## Troubleshooting

### "Guru Meditation Error" on boot
This is usually a transient BLE stack issue. Try:
1. Power cycle the ESP32
2. Run `esphome clean` then recompile
3. Ensure only one device is connecting to the Fossibot at a time

### Entities showing in wrong category
After updating, you may need to:
1. Reload the ESPHome integration in Home Assistant
2. Clear your browser cache (Ctrl+F5)

### Settings not appearing in Configuration section
The `entity_category: config` setting controls this. Make sure it's set in your YAML and reupload the firmware.

## Optional: LED Battery Indicator

The component includes an optional LED that shows battery level via brightness:
- Connect an LED with resistor to GPIO32
- 0% battery = LED off
- 100% battery = LED full brightness

## Contributing

Pull requests welcome! Please ensure any new features follow the existing code patterns.

## License

This project is licensed under the Apache 2.0 License - see the LICENSE file for details.

## Acknowledgments

- Protocol reverse-engineered from the official Fossibot app
- Built with [ESPHome](https://esphome.io/)
