import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import CONF_ID
from .. import fbot_ns, Fbot, CONF_FBOT_ID

DEPENDENCIES = ["fbot"]

CONF_POWER_OFF = "power_off"

FbotButton = fbot_ns.class_("FbotButton", button.Button, cg.Component)

BUTTON_TYPES = {
    CONF_POWER_OFF: "power_off",
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_FBOT_ID): cv.use_id(Fbot),
        cv.Optional(CONF_POWER_OFF): button.button_schema(
            FbotButton,
            icon="mdi:power",
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_FBOT_ID])
    
    for key, button_type in BUTTON_TYPES.items():
        if key in config:
            var = await button.new_button(config[key])
            await cg.register_component(var, config[key])
            cg.add(var.set_parent(parent))
            cg.add(var.set_button_type(button_type))
