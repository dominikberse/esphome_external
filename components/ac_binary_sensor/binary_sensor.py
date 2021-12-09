import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import binary_sensor
from esphome.const import CONF_ID, CONF_PIN, CONF_FREQUENCY, CONF_THRESHOLD

ACBinarySensor = cg.global_ns.class_(
    'ACBinarySensor', binary_sensor.BinarySensor, cg.Component)

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

    pin_a = await cg.gpio_pin_expression(config[CONF_PIN])
    cg.add(var.set_pin(pin_a))

    if CONF_FREQUENCY in config:
        freq = await cg.get_variable(config[CONF_FREQUENCY])
        cg.add(var.set_freq(freq))

    if CONF_THRESHOLD in config:
        freq = await cg.get_variable(config[CONF_THRESHOLD])
        cg.add(var.set_threshold(freq))
