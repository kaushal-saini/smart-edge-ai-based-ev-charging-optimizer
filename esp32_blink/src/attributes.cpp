#include "attributes.h"
#include "network.h"
#include "state.h"

// Request current values of shared attributes from ThingsBoard on boot/reconnect
void requestSharedAttributes() {
  StaticJsonDocument<192> doc;
  doc["sharedKeys"] = "maxStationLoadW,overloadCurrentA,peakTariffStartHr,peakTariffEndHr,predictionThreshold";
  char buffer[192];
  size_t n = serializeJson(doc, buffer, sizeof(buffer));
  mqtt.publish("v1/devices/me/attributes/request/1", buffer, n);
  Serial.println("[MQTT >>] Requested shared attributes from ThingsBoard.");
}

void applySharedAttributes(JsonObject attrs) {
  if (attrs.containsKey("maxStationLoadW")) {
    maxStationLoadW = attrs["maxStationLoadW"].as<float>();
  }
  if (attrs.containsKey("overloadCurrentA")) {
    overloadCurrentA = attrs["overloadCurrentA"].as<float>();
  }
  if (attrs.containsKey("peakTariffStartHr")) {
    peakTariffStartHr = attrs["peakTariffStartHr"].as<int>();
  }
  if (attrs.containsKey("peakTariffEndHr")) {
    peakTariffEndHr = attrs["peakTariffEndHr"].as<int>();
  }
  if (attrs.containsKey("predictionThreshold")) {
    predictionThreshold = attrs["predictionThreshold"].as<float>();
  }

  Serial.println("--------------------------------------------------");
  Serial.println("[ATTRIBUTES UPDATED FROM THINGSBOARD]");
  Serial.print("  maxStationLoadW:     "); Serial.println(maxStationLoadW);
  Serial.print("  overloadCurrentA:    "); Serial.println(overloadCurrentA);
  Serial.print("  peakTariffHours:     "); Serial.print(peakTariffStartHr); Serial.print(" - "); Serial.println(peakTariffEndHr);
  Serial.print("  predictionThreshold: "); Serial.println(predictionThreshold);
  Serial.println("--------------------------------------------------");
}
