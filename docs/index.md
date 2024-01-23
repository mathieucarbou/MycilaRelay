# MycilaRelay

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Continuous Integration](https://github.com/mathieucarbou/MycilaRelay/actions/workflows/ci.yml/badge.svg)](https://github.com/mathieucarbou/MycilaRelay/actions/workflows/ci.yml)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/mathieucarbou/library/MycilaRelay.svg)](https://registry.platformio.org/libraries/mathieucarbou/MycilaRelay)

Arduino / ESP32 library to control Electromagnetic and Solid State Relays

- Support NO and NC relay types
- On / Off
- Timer mode (on for a given duration, then off, or off for a given duration, then on)
- Toggle
- Actuation count
- Callback on state change (remember: never execute long or I/O task on a callback)

## Usage

Implement the `getConfig(const char* name)` function which returns the configuration for a given relay name.

```c++
const Mycila::RelayConfig Mycila::Relay::getConfig(const char* name) {
  if (strcmp(name, "relay1") == 0)
    return {true, 13, Mycila::RelayType::NC};

  if (strcmp(name, "relay2") == 0)
    return {true, 12, Mycila::RelayType::NC};

  return {false, static_cast<uint8_t>(GPIO_NUM_NC), Mycila::RelayType::NO};
}
```

Then use the relay:

```c++
Mycila::Relay relay1("relay1");

void setup() {
  relay1.begin();
  relay1.begin(true); // activated initially

  relay1.listen([](bool state) {
    Serial.printf("Relay 1 is %s\n", state ? "ON" : "OFF");
  });
}

void loop() {
  Mycila::relay1.toggle();
  Mycila::relay1.off();
  Mycila::relay1.on(500);
  // etc.

  delay(1000);
}
```

See example and API for more!
