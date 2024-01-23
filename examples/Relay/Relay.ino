#include <MycilaRelay.h>

Mycila::Relay relay1("relay1");
Mycila::Relay relay2("relay2");

void setup() {
  Serial.begin(115200);
  while (!Serial)
    continue;

  relay1.listen([](bool state) {
    JsonDocument doc;
    relay1.toJson(doc.to<JsonObject>());
    Serial.print("RELAY 1");
    Serial.print(state ? " :ON: " : " :OFF: ");
    serializeJson(doc, Serial);
    Serial.println();
  });

  relay2.listen([](bool state) {
    JsonDocument doc;
    relay2.toJson(doc.to<JsonObject>());
    Serial.print("RELAY 2");
    Serial.print(state ? " :ON: " : " :OFF: ");
    serializeJson(doc, Serial);
    Serial.println();
  });

  relay1.begin();
  relay1.on();

  relay2.begin(true); // initially active
}

void loop() {
  delay(1000);
  relay1.setState(!relay1.getState(), random(500, 2000));
  relay2.setState(!relay2.getState(), random(500, 2000));
}

const Mycila::RelayConfig Mycila::Relay::getConfig(const char* name) {
  if (strcmp(name, "relay1") == 0)
    return {true, 13, Mycila::RelayType::NC};

  if (strcmp(name, "relay2") == 0)
    return {true, 12, Mycila::RelayType::NC};

  return {false, static_cast<uint8_t>(GPIO_NUM_NC), Mycila::RelayType::NO};
}
