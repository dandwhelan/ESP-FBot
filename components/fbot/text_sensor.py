import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from . import fbot_ns, Fbot, CONF_FBOT_ID

DEPENDENCIES = ["fbot"]

CONF_SYSTEM_STATUS = "system_status"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_FBOT_ID): cv.use_id(Fbot),
        cv.Optional(CONF_SYSTEM_STATUS): text_sensor.text_sensor_schema(
            icon="mdi:information-outline",
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_FBOT_ID])
    
    if CONF_SYSTEM_STATUS in config:
        sens = await text_sensor.new_text_sensor(config[CONF_SYSTEM_STATUS])
        cg.add(parent.set_system_status_text_sensor(sens))
