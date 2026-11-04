import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_BATTERY_LEVEL,
    CONF_BATTERY_VOLTAGE,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_FREQUENCY,
    STATE_CLASS_MEASUREMENT,
    UNIT_PERCENT,
    UNIT_WATT,
    UNIT_MINUTE,
    UNIT_VOLT,
    UNIT_AMPERE,
    UNIT_HERTZ,
)
from . import fbot_ns, Fbot, CONF_FBOT_ID

DEPENDENCIES = ["fbot"]

CONF_INPUT_POWER = "input_power"
CONF_AC_INPUT_POWER = "ac_input_power"
CONF_DC_INPUT_POWER = "dc_input_power"
CONF_OUTPUT_POWER = "output_power"
CONF_SYSTEM_POWER = "system_power"
CONF_TOTAL_POWER = "total_power"
CONF_AC_GRID_POWER = "ac_grid_power"
CONF_REMAINING_TIME = "remaining_time"
CONF_TIME_TO_EMPTY = "time_to_empty"
CONF_BATTERY_S1_LEVEL = "battery_s1_level"
CONF_BATTERY_S2_LEVEL = "battery_s2_level"
CONF_BATTERY_S3_LEVEL = "battery_s3_level"
CONF_BATTERY_S4_LEVEL = "battery_s4_level"
CONF_BATTERY_CAPACITY = "battery_capacity"
CONF_THRESHOLD_CHARGE = "threshold_charge"
CONF_THRESHOLD_DISCHARGE = "threshold_discharge"
CONF_CHARGE_LEVEL = "charge_level"
CONF_AC_OUT_VOLTAGE = "ac_out_voltage"
CONF_AC_OUT_FREQUENCY = "ac_out_frequency"
CONF_AC_IN_VOLTAGE = "ac_in_voltage"
CONF_AC_IN_FREQUENCY = "ac_in_frequency"
CONF_TIME_TO_FULL = "time_to_full"
CONF_BOOKING_CHARGE_COUNTDOWN = "booking_charge_countdown"
CONF_AC_STANDBY_COUNTER = "ac_standby_counter"
CONF_DC_STANDBY_COUNTER = "dc_standby_counter"
CONF_SHUTDOWN_TIMER_COUNTDOWN = "shutdown_timer_countdown"
CONF_FAN_LEVEL = "fan_level"
CONF_ERROR_CODE = "error_code"
CONF_PROTECTION_FLAGS = "protection_flags"
CONF_STATUS_FLAGS = "status_flags"
CONF_HARDWARE_ID = "hardware_id"
CONF_MAX_AC_INPUT = "max_ac_input"
CONF_MAX_CHARGE_CURRENT = "max_charge_current"
CONF_AC_MCU_VERSION = "ac_mcu_version"
CONF_BMS_MCU_VERSION = "bms_mcu_version"
CONF_PV_MCU_VERSION = "pv_mcu_version"
CONF_DC_MCU_VERSION = "dc_mcu_version"
CONF_USB_A1_POWER = "usb_a1_power"
CONF_USB_A2_POWER = "usb_a2_power"
CONF_USB_C1_POWER = "usb_c1_power"
CONF_USB_C2_POWER = "usb_c2_power"
CONF_USB_C3_POWER = "usb_c3_power"
CONF_USB_C4_POWER = "usb_c4_power"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_FBOT_ID): cv.use_id(Fbot),
        cv.Optional(CONF_BATTERY_LEVEL): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_BATTERY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_S1_LEVEL): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_BATTERY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_S2_LEVEL): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_BATTERY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_S3_LEVEL): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_BATTERY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_S4_LEVEL): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_BATTERY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_CAPACITY): sensor.sensor_schema(
            unit_of_measurement="Ah",
            accuracy_decimals=1,
            icon="mdi:battery-high",
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_INPUT_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_AC_INPUT_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DC_INPUT_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_OUTPUT_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_SYSTEM_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TOTAL_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_AC_GRID_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_REMAINING_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_MINUTE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_DURATION,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TIME_TO_EMPTY): sensor.sensor_schema(
            unit_of_measurement=UNIT_MINUTE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_DURATION,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_THRESHOLD_CHARGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_THRESHOLD_DISCHARGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGE_LEVEL): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_AC_OUT_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_AC_OUT_FREQUENCY): sensor.sensor_schema(
            unit_of_measurement=UNIT_HERTZ,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_FREQUENCY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_AC_IN_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_AC_IN_FREQUENCY): sensor.sensor_schema(
            unit_of_measurement=UNIT_HERTZ,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_FREQUENCY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TIME_TO_FULL): sensor.sensor_schema(
            unit_of_measurement=UNIT_MINUTE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_DURATION,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BOOKING_CHARGE_COUNTDOWN): sensor.sensor_schema(
            unit_of_measurement=UNIT_MINUTE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_DURATION,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:timer-sand",
        ),
        cv.Optional(CONF_AC_STANDBY_COUNTER): sensor.sensor_schema(
            unit_of_measurement=UNIT_MINUTE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_DURATION,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:timer-outline",
        ),
        cv.Optional(CONF_DC_STANDBY_COUNTER): sensor.sensor_schema(
            unit_of_measurement=UNIT_MINUTE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_DURATION,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:timer-outline",
        ),
        cv.Optional(CONF_SHUTDOWN_TIMER_COUNTDOWN): sensor.sensor_schema(
            unit_of_measurement=UNIT_MINUTE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_DURATION,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:power-sleep",
        ),
        cv.Optional(CONF_FAN_LEVEL): sensor.sensor_schema(
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:fan",
        ),
        cv.Optional(CONF_ERROR_CODE): sensor.sensor_schema(
            accuracy_decimals=0,
            icon="mdi:alert-circle-outline",
        ),
        cv.Optional(CONF_PROTECTION_FLAGS): sensor.sensor_schema(
            accuracy_decimals=0,
            icon="mdi:shield-alert-outline",
        ),
        cv.Optional(CONF_STATUS_FLAGS): sensor.sensor_schema(
            accuracy_decimals=0,
            icon="mdi:flag-outline",
        ),
        cv.Optional(CONF_HARDWARE_ID): sensor.sensor_schema(
            accuracy_decimals=0,
            icon="mdi:chip",
        ),
        cv.Optional(CONF_MAX_AC_INPUT): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MAX_CHARGE_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_AC_MCU_VERSION): sensor.sensor_schema(
            accuracy_decimals=1,
            icon="mdi:tag-outline",
        ),
        cv.Optional(CONF_BMS_MCU_VERSION): sensor.sensor_schema(
            accuracy_decimals=1,
            icon="mdi:tag-outline",
        ),
        cv.Optional(CONF_PV_MCU_VERSION): sensor.sensor_schema(
            accuracy_decimals=1,
            icon="mdi:tag-outline",
        ),
        cv.Optional(CONF_DC_MCU_VERSION): sensor.sensor_schema(
            accuracy_decimals=1,
            icon="mdi:tag-outline",
        ),
        cv.Optional(CONF_USB_A1_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_USB_A2_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_USB_C1_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_USB_C2_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_USB_C3_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_USB_C4_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_FBOT_ID])
    
    mapping = [
        (CONF_BATTERY_LEVEL, parent.set_battery_percent_sensor),
        (CONF_BATTERY_S1_LEVEL, parent.set_battery_percent_s1_sensor),
        (CONF_BATTERY_S2_LEVEL, parent.set_battery_percent_s2_sensor),
        (CONF_BATTERY_S3_LEVEL, parent.set_battery_percent_s3_sensor),
        (CONF_BATTERY_S4_LEVEL, parent.set_battery_percent_s4_sensor),
        (CONF_BATTERY_CAPACITY, parent.set_battery_capacity_sensor),
        (CONF_BATTERY_VOLTAGE, parent.set_battery_voltage_sensor),
        (CONF_INPUT_POWER, parent.set_input_power_sensor),
        (CONF_AC_INPUT_POWER, parent.set_ac_input_power_sensor),
        (CONF_DC_INPUT_POWER, parent.set_dc_input_power_sensor),
        (CONF_OUTPUT_POWER, parent.set_output_power_sensor),
        (CONF_SYSTEM_POWER, parent.set_system_power_sensor),
        (CONF_TOTAL_POWER, parent.set_total_power_sensor),
        (CONF_AC_GRID_POWER, parent.set_ac_grid_power_sensor),
        (CONF_REMAINING_TIME, parent.set_remaining_time_sensor),
        (CONF_TIME_TO_EMPTY, parent.set_time_to_empty_sensor),
        (CONF_THRESHOLD_CHARGE, parent.set_threshold_charge_sensor),
        (CONF_THRESHOLD_DISCHARGE, parent.set_threshold_discharge_sensor),
        (CONF_CHARGE_LEVEL, parent.set_charge_level_sensor),
        (CONF_AC_OUT_VOLTAGE, parent.set_ac_out_voltage_sensor),
        (CONF_AC_OUT_FREQUENCY, parent.set_ac_out_frequency_sensor),
        (CONF_AC_IN_VOLTAGE, parent.set_ac_in_voltage_sensor),
        (CONF_AC_IN_FREQUENCY, parent.set_ac_in_frequency_sensor),
        (CONF_TIME_TO_FULL, parent.set_time_to_full_sensor),
        (CONF_BOOKING_CHARGE_COUNTDOWN, parent.set_booking_charge_countdown_sensor),
        (CONF_AC_STANDBY_COUNTER, parent.set_ac_standby_counter_sensor),
        (CONF_DC_STANDBY_COUNTER, parent.set_dc_standby_counter_sensor),
        (CONF_SHUTDOWN_TIMER_COUNTDOWN, parent.set_shutdown_timer_countdown_sensor),
        (CONF_FAN_LEVEL, parent.set_fan_level_sensor),
        (CONF_ERROR_CODE, parent.set_error_code_sensor),
        (CONF_PROTECTION_FLAGS, parent.set_protection_flags_sensor),
        (CONF_STATUS_FLAGS, parent.set_status_flags_sensor),
        (CONF_HARDWARE_ID, parent.set_hardware_id_sensor),
        (CONF_MAX_AC_INPUT, parent.set_max_ac_input_sensor),
        (CONF_MAX_CHARGE_CURRENT, parent.set_max_charge_current_sensor),
        (CONF_AC_MCU_VERSION, parent.set_ac_mcu_version_sensor),
        (CONF_BMS_MCU_VERSION, parent.set_bms_mcu_version_sensor),
        (CONF_PV_MCU_VERSION, parent.set_pv_mcu_version_sensor),
        (CONF_DC_MCU_VERSION, parent.set_dc_mcu_version_sensor),
        (CONF_USB_A1_POWER, parent.set_usb_a1_power_sensor),
        (CONF_USB_A2_POWER, parent.set_usb_a2_power_sensor),
        (CONF_USB_C1_POWER, parent.set_usb_c1_power_sensor),
        (CONF_USB_C2_POWER, parent.set_usb_c2_power_sensor),
        (CONF_USB_C3_POWER, parent.set_usb_c3_power_sensor),
        (CONF_USB_C4_POWER, parent.set_usb_c4_power_sensor),
    ]

    for key, setter in mapping:
        if key in config:
            sens = await sensor.new_sensor(config[key])
            cg.add(setter(sens))
