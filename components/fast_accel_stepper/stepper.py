from esphome import pins
import esphome.codegen as cg
from esphome.components import stepper
import esphome.config_validation as cv
from esphome.const import (
    CONF_ACCELERATION,
    CONF_ID,
    CONF_STEP_PIN,
    CONF_DIR_PIN,
    CONF_ENABLE_PIN,
    CONF_MAX_SPEED,
)

AUTO_LOAD = []
DEPENDENCIES = ["stepper"]
fast_accel_stepper_ns = cg.esphome_ns.namespace("fast_accel_stepper")
FastAccelStepper = fast_accel_stepper_ns.class_(
    "FastAccelStepperComponent", stepper.Stepper, cg.Component
)


CONF_UNLOCK_SPEED = "unlock_speed"


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(FastAccelStepper),
        cv.Required(CONF_MAX_SPEED): stepper.validate_speed,
        cv.Optional(CONF_ACCELERATION, default="inf"): stepper.validate_acceleration,
        cv.Required(CONF_STEP_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_DIR_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_ENABLE_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_UNLOCK_SPEED): cv.boolean,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    # Ensure PlatformIO pulls in the FastAccelStepper library automatically
    cg.add_library("gin66/FastAccelStepper", None)

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await stepper.register_stepper(var, config)

    step_pin = await cg.gpio_pin_expression(config[CONF_STEP_PIN])
    cg.add(var.set_step_pin(config[CONF_STEP_PIN]["number"]))

    if CONF_DIR_PIN in config:
        cg.add(
            var.set_dir_pin(
                config[CONF_DIR_PIN]["number"], config[CONF_DIR_PIN]["inverted"]
            )
        )

    if CONF_ENABLE_PIN in config:
        cg.add(
            var.set_enable_pin(
                config[CONF_ENABLE_PIN]["number"], config[CONF_ENABLE_PIN]["inverted"]
            )
        )
    if CONF_UNLOCK_SPEED in config:
        cg.add(var.set_unlock_speed(config[CONF_UNLOCK_SPEED]))
