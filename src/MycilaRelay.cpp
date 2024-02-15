// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2024 Mathieu Carbou and others
 */
#include <MycilaRelay.h>

#define TAG "RELAY"

void Mycila::Relay::begin(const uint32_t pin, const RelayType type, const bool state) {
  if (_enabled)
    return;

  if (GPIO_IS_VALID_OUTPUT_GPIO(pin)) {
    _pin = (gpio_num_t)pin;
  } else {
    ESP_LOGE(TAG, "Disable Relay: Invalid pin: %u", _pin);
    _pin = GPIO_NUM_NC;
    return;
  }

  _type = type;

  ESP_LOGI(TAG, "Enable %s Relay on pin %u...", (_type == RelayType::NO ? "NO" : "NC"), _pin);
  pinMode(_pin, OUTPUT);
  _enabled = true;

  setState(state);
}

void Mycila::Relay::end() {
  if (_enabled) {
    ESP_LOGI(TAG, "Disable %s Relay on pin %u...", (_type == RelayType::NO ? "NO" : "NC"), _pin);
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
      if (duration > 0)
        ESP_LOGD(TAG, "%s Relay on pin %u => %s for %u ms", (_type == RelayType::NO ? "NO" : "NC"), _pin, state ? "on" : "off", duration);
      else
        ESP_LOGD(TAG, "%s Relay on pin %u => %s", (_type == RelayType::NO ? "NO" : "NC"), _pin, state ? "on" : "off");
      if (_callback)
        _callback(state);
    } else if (duration > 0) {
      ESP_LOGD(TAG, "%s Relay on pin %u stays %s for %u ms", (_type == RelayType::NO ? "NO" : "NC"), _pin, state ? "on" : "off", duration);
    } else if (duration == 0) {
      ESP_LOGD(TAG, "%s Relay on pin %u stays %s", (_type == RelayType::NO ? "NO" : "NC"), _pin, state ? "on" : "off");
    }
  }
}

#ifdef MYCILA_RELAY_JSON_SUPPORT
void Mycila::Relay::toJson(const JsonObject& root) const {
  root["enabled"] = _enabled;
  root["state"] = getState() ? "on" : "off";
  root["switch_count"] = _switchCount;
}
#endif
