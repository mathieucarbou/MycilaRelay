#include <Arduino.h>
#include <MycilaRelay.h>

Mycila::Relay relay1;
Mycila::Relay relay2;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    continue;

  relay1.listen([](bool state) {
    Serial.print("RELAY 1");
    Serial.print(state ? " :ON: " : " :OFF: ");
    Serial.println();
  });

  relay2.listen([](bool state) {
    Serial.print("RELAY 2");
    Serial.print(state ? " :ON: " : " :OFF: ");
    Serial.println();
  });

  relay1.begin(13, Mycila::RelayType::NC); // current will pass
  relay1.on();                             // current won't pass

  relay2.begin(12, Mycila::RelayType::NO, true); // current will pass
  relay1.off();                                  // current won't pass
}

void loop() {
  delay(1000);
  relay1.setState(!relay1.getState(), random(500, 2000));
  relay2.setState(!relay2.getState(), random(500, 2000));
}
