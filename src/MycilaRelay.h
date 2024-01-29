// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2024 Mathieu Carbou and others
 */
#pragma once

#include <Ticker.h>
#include <WString.h>
#include <esp32-hal-gpio.h>
#include <functional>

#ifdef MYCILA_RELAY_JSON_SUPPORT
#include <ArduinoJson.h>
#endif

#define MYCILA_RELAY_VERSION "1.1.0"
#define MYCILA_RELAY_VERSION_MAJOR 1
#define MYCILA_RELAY_VERSION_MINOR 1
#define MYCILA_RELAY_VERSION_REVISION 0

namespace Mycila {
  enum class RelayType {
    NO,
    NC
  };

  typedef std::function<void(bool state)> RelayStateCallback;

  typedef struct {
      bool enabled;
      uint8_t pin;
      RelayType type;
  } RelayConfig;

  class Relay {
    public:
      static const RelayConfig getConfig(const char* name);

    public:
      explicit Relay(const char* name) : name(name) {}

      void begin(bool state = false);
      void end();

      void listen(RelayStateCallback callback) { _callback = callback; }

      // set relay on for a duration in milliseconds
      void on(uint32_t duration = 0) { setState(true, duration); }
      // set relay off for a duration in milliseconds
      void off(uint32_t duration = 0) { setState(false, duration); }
      // set relay on or off for a duration in milliseconds
      void setState(bool state, uint32_t duration = 0);
      inline void toggle(uint32_t duration = 0) { setState(!getState(), duration); }

      bool getState() const { return digitalRead(_pin) == HIGH ? _type == RelayType::NO : _type != RelayType::NO; }
      inline bool isOn() const { return getState(); }
      inline bool isOff() const { return !isOn(); }

      gpio_num_t getPin() const { return _pin; }
      RelayType getType() const { return _type; }
      int64_t getSwitchCount() const { return _switchCount; }
      bool isEnabled() const { return _enabled; }

#ifdef MYCILA_RELAY_JSON_SUPPORT
      void toJson(const JsonObject& root) const;
#endif

    public:
      const char* name;

    private:
      bool _enabled = false;
      RelayType _type = RelayType::NO;
      gpio_num_t _pin = GPIO_NUM_NC;
      volatile int64_t _switchCount = 0;
      RelayStateCallback _callback;
      Ticker _ticker;
  };
} // namespace Mycila
