#include <WiFi.h>
#include <ArduinoJson.h>
#include "telemetry.h"
#include "network.h"
#include "state.h"
#include "config.h"

void publishTelemetry() {
  if (!mqtt.connected()) return;

  StaticJsonDocument<384> doc;
  doc["bayId"]                = BAY_ID;
  doc["voltage"]              = round(voltage * 10.0f) / 10.0f;
  doc["current"]              = round(current * 10.0f) / 10.0f;
  doc["power"]                = round(power * 10.0f) / 10.0f;
  doc["energyWh"]             = round(energyWh * 100.0f) / 100.0f;
  doc["temperature"]          = round(temperature * 10.0f) / 10.0f;
  doc["bayStatus"]            = bayStatus;
  doc["predictedArrivalProb"] = round(predictedArrivalProb * 1000.0f) / 1000.0f;
  doc["predictedDurationMin"] = predictedDurationMin;
  doc["loadDecision"]         = loadDecision;
  doc["throttleLevel"]        = throttleLevel;
  doc["overloadActive"]       = overloadActive;
  doc["manualOverrideActive"] = manualOverrideActive;

  char buffer[384];
  size_t n = serializeJson(doc, buffer, sizeof(buffer));

  bool ok = mqtt.publish("v1/devices/me/telemetry", buffer, n);
  if (ok) {
    Serial.print("[MQTT >> Telemetry] ");
    Serial.println(buffer);
  } else {
    Serial.println("[MQTT !!] Telemetry publish failed!");
  }
}

void publishClientAttributes() {
  if (!mqtt.connected()) return;

  StaticJsonDocument<256> doc;
  doc["bayId"]           = BAY_ID;
  doc["firmwareVersion"] = FIRMWARE_VERSION;
  doc["ipAddress"]       = WiFi.localIP().toString();

  char buffer[256];
  size_t n = serializeJson(doc, buffer, sizeof(buffer));

  mqtt.publish("v1/devices/me/attributes", buffer, n);
  Serial.print("[MQTT >> Client Attributes] ");
  Serial.println(buffer);
}
