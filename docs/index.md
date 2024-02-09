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

Then use the relay:

```c++
Mycila::Relay relay1("relay1");

void setup() {
  relay1.begin(12);
  relay1.begin(12, Mycila::RelayType::NO, true);

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
