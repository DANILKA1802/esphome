"""Validate the optional OpenTherm connection monitor."""

from collections.abc import Generator

import pytest

from esphome.components import opentherm
from esphome.components.esp32.const import KEY_BOARD
from esphome.components.opentherm import binary_sensor
from esphome.config import path_context
import esphome.config_validation as cv
from esphome.const import (
    KEY_CORE,
    KEY_ESP32,
    KEY_FRAMEWORK_VERSION,
    KEY_TARGET_FRAMEWORK,
    KEY_TARGET_PLATFORM,
    KEY_VARIANT,
)
from esphome.core import CORE


@pytest.fixture(autouse=True)
def esp32_core() -> Generator[None]:
    CORE.data[KEY_CORE] = {
        KEY_TARGET_PLATFORM: "esp32",
        KEY_TARGET_FRAMEWORK: "esp-idf",
        KEY_FRAMEWORK_VERSION: cv.Version(5, 5, 5),
    }
    CORE.data[KEY_ESP32] = {KEY_VARIANT: "ESP32", KEY_BOARD: "esp32dev"}
    token = path_context.set([])
    yield
    path_context.reset(token)


def test_default_connection_timeout() -> None:
    config = opentherm.CONFIG_SCHEMA({"in_pin": 17, "out_pin": 16})
    assert config["connection_timeout"].total_milliseconds == 10000


@pytest.mark.parametrize("timeout", ["0s", "-1s", "500ms", "never"])
def test_reject_invalid_timeout(timeout: str) -> None:
    with pytest.raises(cv.Invalid):
        opentherm.CONFIG_SCHEMA(
            {"in_pin": 17, "out_pin": 16, "connection_timeout": timeout}
        )


def test_connected_without_boiler_register_sensors() -> None:
    config = binary_sensor.CONFIG_SCHEMA({"connected": {"name": "Boiler Connected"}})
    assert config["connected"]["device_class"] == "connectivity"
    assert config["connected"]["entity_category"] == "diagnostic"


def test_connected_alongside_existing_sensors() -> None:
    config = binary_sensor.CONFIG_SCHEMA(
        {
            "connected": {"name": "Boiler Connected"},
            "fault_indication": {"name": "Boiler Fault"},
            "flame_on": {"name": "Boiler Flame"},
        }
    )
    assert "fault_indication" in config
    assert "flame_on" in config
