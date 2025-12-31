import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    CONF_ID,
    CONF_ICON,
    CONF_MIN_VALUE,
    CONF_MAX_VALUE,
    CONF_STEP,
    UNIT_PERCENT,
)
from .. import fbot_ns, Fbot, CONF_FBOT_ID

DEPENDENCIES = ["fbot"]

CONF_THRESHOLD_CHARGE = "threshold_charge"
CONF_THRESHOLD_DISCHARGE = "threshold_discharge"
CONF_POLLING_INTERVAL = "polling_interval"
CONF_SETTINGS_POLLING_INTERVAL = "settings_polling_interval"
CONF_SCREEN_TIMEOUT = "screen_timeout"
CONF_AC_STANDBY = "ac_standby"
CONF_DC_STANDBY = "dc_standby"
CONF_USB_STANDBY = "usb_standby"
CONF_START_CHARGE_AFTER = "start_charge_after"

FbotNumber = fbot_ns.class_("FbotNumber", number.Number, cg.Component)

NUMBER_TYPES = {
    CONF_THRESHOLD_CHARGE: "threshold_charge",
    CONF_THRESHOLD_DISCHARGE: "threshold_discharge",
    CONF_POLLING_INTERVAL: "polling_interval",
    CONF_SETTINGS_POLLING_INTERVAL: "settings_polling_interval",
    CONF_SCREEN_TIMEOUT: "screen_timeout",
    CONF_AC_STANDBY: "ac_standby",
    CONF_DC_STANDBY: "dc_standby",
    CONF_USB_STANDBY: "usb_standby",
    CONF_START_CHARGE_AFTER: "start_charge_after",
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_FBOT_ID): cv.use_id(Fbot),
        cv.Optional(CONF_THRESHOLD_CHARGE): number.number_schema(
            FbotNumber,
            icon="mdi:battery-charging-100",
            unit_of_measurement=UNIT_PERCENT,
        ).extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=60): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=100): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),
        cv.Optional(CONF_THRESHOLD_DISCHARGE): number.number_schema(
            FbotNumber,
            icon="mdi:battery-outline",
            unit_of_measurement=UNIT_PERCENT,
        ).extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=50): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),
        cv.Optional(CONF_POLLING_INTERVAL): number.number_schema(
            FbotNumber,
            icon="mdi:timer-refresh",
            unit_of_measurement="s",
        ).extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=60): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),
        cv.Optional(CONF_SETTINGS_POLLING_INTERVAL): number.number_schema(
            FbotNumber,
            icon="mdi:timer-cog",
            unit_of_measurement="s",
        ).extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=60): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),
        cv.Optional(CONF_SCREEN_TIMEOUT): number.number_schema(
            FbotNumber,
            icon="mdi:monitor-off",
            unit_of_measurement="min",
        ).extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=60): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),
        cv.Optional(CONF_AC_STANDBY): number.number_schema(
            FbotNumber,
            icon="mdi:power-plug-off",
            unit_of_measurement="min",
        ).extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=1440): cv.float_, # 24 hours? Protocol says 480=8h
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),
        cv.Optional(CONF_DC_STANDBY): number.number_schema(
            FbotNumber,
            icon="mdi:car-battery",
            unit_of_measurement="min",
        ).extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=1440): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),
        cv.Optional(CONF_USB_STANDBY): number.number_schema(
            FbotNumber,
            icon="mdi:usb",
            unit_of_measurement="s",
        ).extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=3600): cv.float_, # Protocol says 1800=30min
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),
        cv.Optional(CONF_START_CHARGE_AFTER): number.number_schema(
            FbotNumber,
            icon="mdi:timer-start",
            unit_of_measurement="min",
        ).extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=1440): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_FBOT_ID])

    for key, number_type in NUMBER_TYPES.items():
        if key in config:
            var = await number.new_number(
                config[key],
                min_value=config[key][CONF_MIN_VALUE],
                max_value=config[key][CONF_MAX_VALUE],
                step=config[key][CONF_STEP],
            )
            await cg.register_component(var, config[key])
            cg.add(var.set_parent(parent))
            cg.add(var.set_number_type(number_type))
            
            # Register the number component with the parent so it can receive updates
            if key == CONF_THRESHOLD_CHARGE:
                cg.add(parent.set_threshold_charge_number(var))
            elif key == CONF_THRESHOLD_DISCHARGE:
                cg.add(parent.set_threshold_discharge_number(var))
            elif key == CONF_POLLING_INTERVAL:
                cg.add(parent.set_polling_interval_number(var))
            elif key == CONF_SETTINGS_POLLING_INTERVAL:
                cg.add(parent.set_settings_polling_interval_number(var))
            elif key == CONF_SCREEN_TIMEOUT:
                cg.add(parent.set_screen_timeout_number(var))
            elif key == CONF_AC_STANDBY:
                cg.add(parent.set_ac_standby_number(var))
            elif key == CONF_DC_STANDBY:
                cg.add(parent.set_dc_standby_number(var))
            elif key == CONF_USB_STANDBY:
                cg.add(parent.set_usb_standby_number(var))
            elif key == CONF_START_CHARGE_AFTER:
                cg.add(parent.set_start_charge_after_number(var))
