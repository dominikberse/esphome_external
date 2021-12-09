import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import binary_sensor
from esphome.const import CONF_ID, CONF_PIN_A, CONF_PIN_B, CONF_FREQUENCY

ACBinarySensor = cg.global_ns.class_(
    'ACBinarySensor', binary_sensor.BinarySensor, cg.Component)

CONFIG_SCHEMA = binary_sensor.BINARY_SENSOR_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(ACBinarySensor),
        cv.Required(CONF_PIN_A): pins.gpio_input_pin_schema,
        cv.Required(CONF_PIN_B): pins.gpio_input_pin_schema,
        cv.Optional(CONF_FREQUENCY): cv.uint8_t,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await binary_sensor.register_binary_sensor(var, config)

    pin_a = await cg.gpio_pin_expression(config[CONF_PIN_A])
    cg.add(var.add_pin(pin_a))

    pin_b = await cg.gpio_pin_expression(config[CONF_PIN_B])
    cg.add(var.add_pin(pin_b))

    if CONF_FREQUENCY in config:
        freq = await cg.get_variable(config[CONF_FREQUENCY])
        cg.add(var.set_freq(freq))

