import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import CONF_ID, CONF_ICON
from .. import fbot_ns, Fbot, CONF_FBOT_ID

DEPENDENCIES = ["fbot"]

CONF_LIGHT_MODE = "light_mode"
CONF_CHARGE_LEVEL = "charge_level"

FbotSelect = fbot_ns.class_("FbotSelect", select.Select, cg.Component)

# Light mode options mapping to register values
LIGHT_MODE_OPTIONS = {
    "Off": 0,
    "On": 1,
    "SOS": 2,
    "Flashing": 3,
}

# Charge level options mapping to register values (register 2)
# 1 = 300W, 2 = 500W, 3 = 700W, 4 = 900W, 5 = 1100W
CHARGE_LEVEL_OPTIONS = {
    "300W": 1,
    "500W": 2,
    "700W": 3,
    "900W": 4,
    "1100W": 5,
}

SELECT_TYPES = {
    CONF_LIGHT_MODE: "light_mode",
    CONF_CHARGE_LEVEL: "charge_level",
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_FBOT_ID): cv.use_id(Fbot),
        cv.Optional(CONF_LIGHT_MODE): select.select_schema(
            FbotSelect,
            icon="mdi:lightbulb-multiple",
        ),
        cv.Optional(CONF_CHARGE_LEVEL): select.select_schema(
            FbotSelect,
            icon="mdi:battery-charging",
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_FBOT_ID])
    
    for key, select_type in SELECT_TYPES.items():
        if key in config:
            # Use the appropriate options based on select type
            if key == CONF_LIGHT_MODE:
                options = list(LIGHT_MODE_OPTIONS.keys())
            elif key == CONF_CHARGE_LEVEL:
                options = list(CHARGE_LEVEL_OPTIONS.keys())
            else:
                options = []
            
            var = await select.new_select(config[key], options=options)
            await cg.register_component(var, config[key])
            cg.add(var.set_parent(parent))
            cg.add(var.set_select_type(select_type))
            
            # Register select with parent to enable state synchronization
            if key == CONF_LIGHT_MODE:
                cg.add(parent.set_light_mode_select(var))
            elif key == CONF_CHARGE_LEVEL:
                cg.add(parent.set_charge_level_select(var))
