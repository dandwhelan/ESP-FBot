import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_CONNECTIVITY,
    DEVICE_CLASS_PROBLEM,
    DEVICE_CLASS_BATTERY_CHARGING,
    CONF_ICON,
)
from . import fbot_ns, Fbot, CONF_FBOT_ID

DEPENDENCIES = ["fbot"]

CONF_CONNECTED = "connected"
CONF_BATTERY_CONNECTED_S1 = "battery_connected_s1"
CONF_BATTERY_CONNECTED_S2 = "battery_connected_s2"
CONF_BATTERY_CONNECTED_S3 = "battery_connected_s3"
CONF_BATTERY_CONNECTED_S4 = "battery_connected_s4"
CONF_USB_ACTIVE = "usb_active"
CONF_DC_ACTIVE = "dc_active"
CONF_AC_ACTIVE = "ac_active"
CONF_LIGHT_ACTIVE = "light_active"
CONF_AC_OUT_ACTIVE = "ac_out_active"
CONF_AC_IN_PRESENT = "ac_in_present"
CONF_AC_IN_CHARGING = "ac_in_charging"
CONF_LOW_PV_IN_PRESENT = "low_pv_in_present"
CONF_LOW_PV_CHARGING = "low_pv_charging"
CONF_DC_OUT_ACTIVE = "dc_out_active"
CONF_CAR_IN_PRESENT = "car_in_present"
CONF_CAR_CHARGING = "car_charging"
CONF_HIGH_PV_IN_PRESENT = "high_pv_in_present"
CONF_HIGH_PV_CHARGING = "high_pv_charging"
CONF_UPS_BYPASS_ACTIVE = "ups_bypass_active"
CONF_CHARGING_ACTIVE = "charging_active"
CONF_INVERTER_STANDBY = "inverter_standby"
CONF_CRITICAL_FAULT = "critical_fault"
CONF_TEMP_PROTECTION = "temp_protection"
CONF_INVERTER_FAULT = "inverter_fault"

DEVICE_CLASS_OUTLET = "plug"
DEVICE_CLASS_LIGHT = "light"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_FBOT_ID): cv.use_id(Fbot),
        cv.Optional(CONF_CONNECTED): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
        ),
        cv.Optional(CONF_BATTERY_CONNECTED_S1): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
            icon="mdi:battery-plus",
        ),
        cv.Optional(CONF_BATTERY_CONNECTED_S2): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
            icon="mdi:battery-plus",
        ),
        cv.Optional(CONF_BATTERY_CONNECTED_S3): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
            icon="mdi:battery-plus",
        ),
        cv.Optional(CONF_BATTERY_CONNECTED_S4): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
            icon="mdi:battery-plus",
        ),
        cv.Optional(CONF_USB_ACTIVE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_OUTLET,
        ),
        cv.Optional(CONF_DC_ACTIVE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_OUTLET,
        ),
        cv.Optional(CONF_AC_ACTIVE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_OUTLET,
        ),
        cv.Optional(CONF_LIGHT_ACTIVE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_LIGHT,
            icon="mdi:lightbulb",
        ),
        cv.Optional(CONF_AC_OUT_ACTIVE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_OUTLET,
            icon="mdi:power-socket-eu",
        ),
        cv.Optional(CONF_AC_IN_PRESENT): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
            icon="mdi:transmission-tower",
        ),
        cv.Optional(CONF_AC_IN_CHARGING): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_BATTERY_CHARGING,
            icon="mdi:battery-charging-wireless",
        ),
        cv.Optional(CONF_LOW_PV_IN_PRESENT): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
            icon="mdi:solar-panel",
        ),
        cv.Optional(CONF_LOW_PV_CHARGING): binary_sensor.binary_sensor_schema(
            icon="mdi:solar-power",
        ),
        cv.Optional(CONF_DC_OUT_ACTIVE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_OUTLET,
            icon="mdi:car-power",
        ),
        cv.Optional(CONF_CAR_IN_PRESENT): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
            icon="mdi:car-battery",
        ),
        cv.Optional(CONF_CAR_CHARGING): binary_sensor.binary_sensor_schema(
            icon="mdi:car-electric",
        ),
        cv.Optional(CONF_HIGH_PV_IN_PRESENT): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
            icon="mdi:solar-panel-large",
        ),
        cv.Optional(CONF_HIGH_PV_CHARGING): binary_sensor.binary_sensor_schema(
            icon="mdi:solar-power-variant",
        ),
        cv.Optional(CONF_UPS_BYPASS_ACTIVE): binary_sensor.binary_sensor_schema(
            icon="mdi:ray-vertex",
        ),
        cv.Optional(CONF_CHARGING_ACTIVE): binary_sensor.binary_sensor_schema(
            icon="mdi:battery-charging",
        ),
        cv.Optional(CONF_INVERTER_STANDBY): binary_sensor.binary_sensor_schema(
            icon="mdi:sleep",
        ),
        cv.Optional(CONF_CRITICAL_FAULT): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_PROBLEM,
            icon="mdi:alert-decagram",
        ),
        cv.Optional(CONF_TEMP_PROTECTION): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_PROBLEM,
            icon="mdi:thermometer-alert",
        ),
        cv.Optional(CONF_INVERTER_FAULT): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_PROBLEM,
            icon="mdi:alert-octagon",
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_FBOT_ID])
    
    mapping = [
        (CONF_CONNECTED, parent.set_connected_binary_sensor),
        (CONF_BATTERY_CONNECTED_S1, parent.set_battery_connected_s1_binary_sensor),
        (CONF_BATTERY_CONNECTED_S2, parent.set_battery_connected_s2_binary_sensor),
        (CONF_BATTERY_CONNECTED_S3, parent.set_battery_connected_s3_binary_sensor),
        (CONF_BATTERY_CONNECTED_S4, parent.set_battery_connected_s4_binary_sensor),
        (CONF_USB_ACTIVE, parent.set_usb_active_binary_sensor),
        (CONF_DC_ACTIVE, parent.set_dc_active_binary_sensor),
        (CONF_AC_ACTIVE, parent.set_ac_active_binary_sensor),
        (CONF_LIGHT_ACTIVE, parent.set_light_active_binary_sensor),
        (CONF_AC_OUT_ACTIVE, parent.set_ac_out_active_binary_sensor),
        (CONF_AC_IN_PRESENT, parent.set_ac_in_present_binary_sensor),
        (CONF_AC_IN_CHARGING, parent.set_ac_in_charging_binary_sensor),
        (CONF_LOW_PV_IN_PRESENT, parent.set_low_pv_in_present_binary_sensor),
        (CONF_LOW_PV_CHARGING, parent.set_low_pv_charging_binary_sensor),
        (CONF_DC_OUT_ACTIVE, parent.set_dc_out_active_binary_sensor),
        (CONF_CAR_IN_PRESENT, parent.set_car_in_present_binary_sensor),
        (CONF_CAR_CHARGING, parent.set_car_charging_binary_sensor),
        (CONF_HIGH_PV_IN_PRESENT, parent.set_high_pv_in_present_binary_sensor),
        (CONF_HIGH_PV_CHARGING, parent.set_high_pv_charging_binary_sensor),
        (CONF_UPS_BYPASS_ACTIVE, parent.set_ups_bypass_active_binary_sensor),
        (CONF_CHARGING_ACTIVE, parent.set_charging_active_binary_sensor),
        (CONF_INVERTER_STANDBY, parent.set_inverter_standby_binary_sensor),
        (CONF_CRITICAL_FAULT, parent.set_critical_fault_binary_sensor),
        (CONF_TEMP_PROTECTION, parent.set_temp_protection_binary_sensor),
        (CONF_INVERTER_FAULT, parent.set_inverter_fault_binary_sensor),
    ]

    for key, setter in mapping:
        if key in config:
            sens = await binary_sensor.new_binary_sensor(config[key])
            cg.add(setter(sens))
