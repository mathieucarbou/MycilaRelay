// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2024 Mathieu Carbou and others
 */
#include <MycilaLogger.h>
#include <MycilaRelay.h>

#define TAG "RELAY"

void Mycila::Relay::begin(bool state) {
  if (_enabled)
    return;

  const RelayConfig config = Mycila::Relay::getConfig(name);

  if (!config.enabled) {
    Logger.warn(TAG, "Disable Relay: '%s'", name);
    return;
  }

  int32_t pin = config.pin;
  if (GPIO_IS_VALID_OUTPUT_GPIO(pin)) {
    _pin = (gpio_num_t)pin;
  } else {
    Logger.error(TAG, "Disable Relay '%s': Invalid pin: %u", name, _pin);
    _pin = GPIO_NUM_NC;
    return;
  }

  _type = config.type;

  Logger.info(TAG, "Enable Relay '%s'...", name);
  Logger.debug(TAG, "- Pin: %u", _pin);
  Logger.debug(TAG, "- Type: %s", (_type == RelayType::NO ? "NO" : "NC"));
  pinMode(_pin, OUTPUT);
  _enabled = true;

  setState(state);
}

void Mycila::Relay::end() {
  if (_enabled) {
    Logger.info(TAG, "Disable Relay '%s'...", name);
    setState(false);
    _enabled = false;
    _pin = GPIO_NUM_NC;
  }
}

void Mycila::Relay::setState(bool state, uint32_t duration) {
  if (_enabled) {
    const bool current = isOn();

    if (state == current && duration == 0 && !_ticker.active())
      return;

    // disable any pending action
    _ticker.detach();

    // update if needed
    if (current != state) {
      digitalWrite(_pin, state ? (_type == RelayType::NO ? HIGH : LOW) : (_type == RelayType::NO ? LOW : HIGH));
      _switchCount++;
    }

    // schedule time if needed
    if (duration > 0) {
      if (state)
        _ticker.once_ms(
          duration, +[](Relay* instance) { instance->off(); },
          this);
      else
        _ticker.once_ms(
          duration, +[](Relay* instance) { instance->on(); },
          this);
    }

    // logging and CB, only if state changed
    if (current != state) {
      if (Logger.isDebugEnabled()) {
        if (duration > 0)
          Logger.debug(TAG, "Relay '%s' => %s for %u ms", name, state ? "on" : "off", duration);
        else
          Logger.debug(TAG, "Relay '%s' => %s", name, state ? "on" : "off");
      }

      if (_callback)
        _callback(state);
    } else if (duration > 0 && Logger.isDebugEnabled()) {
      Logger.debug(TAG, "Relay '%s' stays %s for %u ms", name, state ? "on" : "off", duration);
    } else if (duration == 0) {
      Logger.debug(TAG, "Relay '%s' stays %s", name, state ? "on" : "off");
    }
  }
}

void Mycila::Relay::toJson(const JsonObject& root) const {
  root["enabled"] = _enabled;
  root["state"] = getState() ? "on" : "off";
  root["switch_count"] = _switchCount;
}
