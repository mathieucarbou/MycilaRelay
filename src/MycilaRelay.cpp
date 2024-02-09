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
    ESP_LOGE(TAG, "Disable Relay '%s': Invalid pin: %u", name, _pin);
    _pin = GPIO_NUM_NC;
    return;
  }

  _type = type;

  ESP_LOGI(TAG, "Enable Relay '%s'...", name);
  ESP_LOGD(TAG, "- Pin: %u", _pin);
  ESP_LOGD(TAG, "- Type: %s", (_type == RelayType::NO ? "NO" : "NC"));
  pinMode(_pin, OUTPUT);
  _enabled = true;

  setState(state);
}

void Mycila::Relay::end() {
  if (_enabled) {
    ESP_LOGI(TAG, "Disable Relay '%s'...", name);
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
#if defined(ARDUHAL_LOG_LEVEL) && ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_DEBUG
      if (duration > 0)
        ESP_LOGD(TAG, "Relay '%s' => %s for %u ms", name, state ? "on" : "off", duration);
      else
        ESP_LOGD(TAG, "Relay '%s' => %s", name, state ? "on" : "off");
#endif
      if (_callback)
        _callback(state);
    } else if (duration > 0 && esp_log_level_get(TAG) >= ESP_LOG_DEBUG) {
      ESP_LOGD(TAG, "Relay '%s' stays %s for %u ms", name, state ? "on" : "off", duration);
    } else if (duration == 0) {
      ESP_LOGD(TAG, "Relay '%s' stays %s", name, state ? "on" : "off");
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
