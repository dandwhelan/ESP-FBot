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
    UNIT_AMPERE,
    UNIT_SECOND,
)
from .. import fbot_ns, Fbot, CONF_FBOT_ID

DEPENDENCIES = ["fbot"]

CONF_THRESHOLD_CHARGE = "threshold_charge"
CONF_THRESHOLD_DISCHARGE = "threshold_discharge"
CONF_SLEEP_TIMER = "sleep_timer"
CONF_CHARGE_CURRENT = "charge_current"
CONF_SCREEN_TIMEOUT = "screen_timeout"
CONF_BOOKING_CHARGE = "booking_charge"

UNIT_MINUTE = "min"

FbotNumber = fbot_ns.class_("FbotNumber", number.Number, cg.Component)

NUMBER_TYPES = {
    CONF_THRESHOLD_CHARGE: "threshold_charge",
    CONF_THRESHOLD_DISCHARGE: "threshold_discharge",
    CONF_SLEEP_TIMER: "sleep_timer",
    CONF_CHARGE_CURRENT: "charge_current",
    CONF_SCREEN_TIMEOUT: "screen_timeout",
    CONF_BOOKING_CHARGE: "booking_charge",
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
                cv.Optional(CONF_MIN_VALUE, default=10): cv.float_,
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
        cv.Optional(CONF_SLEEP_TIMER): number.number_schema(
            FbotNumber,
            icon="mdi:power-sleep",
            unit_of_measurement=UNIT_MINUTE,
        ).extend(
            {
                # Minimum must never be 0 (writing 0 to Reg 68 permanently bricks Fossibot units)
                cv.Optional(CONF_MIN_VALUE, default=5): cv.float_range(min=1, max=480),
                cv.Optional(CONF_MAX_VALUE, default=480): cv.float_range(min=5, max=480),
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),
        cv.Optional(CONF_CHARGE_CURRENT): number.number_schema(
            FbotNumber,
            icon="mdi:current-ac",
            unit_of_measurement=UNIT_AMPERE,
        ).extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1): cv.float_range(min=1, max=20),
                cv.Optional(CONF_MAX_VALUE, default=20): cv.float_range(min=1, max=20),
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),
        cv.Optional(CONF_SCREEN_TIMEOUT): number.number_schema(
            FbotNumber,
            icon="mdi:monitor-off",
            unit_of_measurement=UNIT_SECOND,
        ).extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=1800): cv.float_,
                cv.Optional(CONF_STEP, default=10): cv.float_,
            }
        ),
        cv.Optional(CONF_BOOKING_CHARGE): number.number_schema(
            FbotNumber,
            icon="mdi:clock-start",
            unit_of_measurement=UNIT_MINUTE,
        ).extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=1440): cv.float_,
                cv.Optional(CONF_STEP, default=10): cv.float_,
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
            
            if key == CONF_THRESHOLD_CHARGE:
                cg.add(parent.set_threshold_charge_number(var))
            elif key == CONF_THRESHOLD_DISCHARGE:
                cg.add(parent.set_threshold_discharge_number(var))
            elif key == CONF_SLEEP_TIMER:
                cg.add(parent.set_sleep_timer_number(var))
            elif key == CONF_CHARGE_CURRENT:
                cg.add(parent.set_charge_current_number(var))
            elif key == CONF_SCREEN_TIMEOUT:
                cg.add(parent.set_screen_timeout_number(var))
            elif key == CONF_BOOKING_CHARGE:
                cg.add(parent.set_booking_charge_number(var))
