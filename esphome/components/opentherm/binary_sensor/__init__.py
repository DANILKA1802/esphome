from typing import Any

import esphome.codegen as cg
from esphome.components import binary_sensor, const as component_const
import esphome.config_validation as cv
from esphome.const import DEVICE_CLASS_CONNECTIVITY, ENTITY_CATEGORY_DIAGNOSTIC

from .. import const, generate, schema, validate

DEPENDENCIES = [const.OPENTHERM]
COMPONENT_TYPE = const.BINARY_SENSOR
CONF_CONNECTED = getattr(component_const, "CONF_CONNECTED", "connected")


def get_entity_validation_schema(entity: schema.BinarySensorSchema) -> cv.Schema:
    return binary_sensor.binary_sensor_schema(
        device_class=(entity.device_class or cv.UNDEFINED),
        icon=(entity.icon or cv.UNDEFINED),
    )


CONFIG_SCHEMA = validate.create_component_schema(
    schema.BINARY_SENSORS, get_entity_validation_schema
).extend(
    {
        cv.Optional(CONF_CONNECTED): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        )
    }
)


async def to_code(config: dict[str, Any]) -> None:
    await generate.component_to_code(
        COMPONENT_TYPE,
        schema.BINARY_SENSORS,
        binary_sensor.BinarySensor,
        generate.create_only_conf(binary_sensor.new_binary_sensor),
        {key: value for key, value in config.items() if key != CONF_CONNECTED},
    )
    hub = await cg.get_variable(config[const.CONF_OPENTHERM_ID])
    if hasattr(binary_sensor, "sub_binary_sensors"):
        sensors = binary_sensor.sub_binary_sensors(config)
        await sensors(CONF_CONNECTED, hub.set_connected_binary_sensor)
    elif CONF_CONNECTED in config:
        connected = await binary_sensor.new_binary_sensor(config[CONF_CONNECTED])
        cg.add(hub.set_connected_binary_sensor(connected))
