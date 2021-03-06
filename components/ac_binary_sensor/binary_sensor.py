import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import binary_sensor
from esphome.const import CONF_ID, CONF_PIN, CONF_FREQUENCY, CONF_THRESHOLD

ac_binary_sensor_ns = cg.esphome_ns.namespace("ac_binary_sensor")
ACBinarySensor = ac_binary_sensor_ns.class_(
    'AcBinarySensor', binary_sensor.BinarySensor, cg.Component)

CONFIG_SCHEMA = binary_sensor.BINARY_SENSOR_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(ACBinarySensor),
        cv.Required(CONF_PIN): pins.gpio_input_pin_schema,
        cv.Optional(CONF_THRESHOLD): cv.uint8_t,
        cv.Optional(CONF_FREQUENCY): cv.uint8_t,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await binary_sensor.register_binary_sensor(var, config)

    pin = await cg.gpio_pin_expression(config[CONF_PIN])
    cg.add(var.set_pin(pin))

    if CONF_FREQUENCY in config:
        cg.add(var.set_freq(config[CONF_FREQUENCY]))

    if CONF_THRESHOLD in config:
        cg.add(var.set_threshold(config[CONF_THRESHOLD]))
