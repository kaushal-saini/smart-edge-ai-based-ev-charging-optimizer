#include <ArduinoJson.h>
#include "rpc.h"
#include "network.h"
#include "state.h"
#include "config.h"

// ---------------------------------------------------------------------
// SRS Section 8.5: RPC Command Handler
// ---------------------------------------------------------------------
void handleRpc(String requestId, char* payload) {
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    Serial.println("[RPC !!] Failed to parse JSON RPC request");
    return;
  }

  String method = doc["method"] | "";
  JsonObject params = doc["params"];

  StaticJsonDocument<384> response;

  if (method == "setRelayState") {
    bool state = params["state"] | false;
    manualOverrideActive = true;
    throttleLevel = state ? 100 : 0;
    loadDecision = state ? "ALLOW" : "MANUAL_OFF";
    response["success"] = true;
    response["throttleLevel"] = throttleLevel;
    response["manualOverrideActive"] = true;
    Serial.print(">> [RPC] setRelayState("); Serial.print(state ? "ON" : "OFF");
    Serial.println(") - Operator override active.");

  } else if (method == "setThrottle") {
    int level = params["level"] | 100;
    level = constrain(level, 0, 100);
    manualOverrideActive = true;
    throttleLevel = level;
    loadDecision = "MANUAL_THROTTLE";
    response["success"] = true;
    response["throttleLevel"] = throttleLevel;
    response["manualOverrideActive"] = true;
    Serial.print(">> [RPC] setThrottle("); Serial.print(level);
    Serial.println("%) - Operator override active.");

  } else if (method == "clearOverride") {
    manualOverrideActive = false;
    response["success"] = true;
    response["manualOverrideActive"] = false;
    Serial.println(">> [RPC] clearOverride() - Auto-optimization restored.");

  } else if (method == "getStatus") {
    response["bayId"] = BAY_ID;
    response["bayStatus"] = bayStatus;
    response["voltage"] = voltage;
    response["current"] = current;
    response["power"] = power;
    response["energyWh"] = energyWh;
    response["throttleLevel"] = throttleLevel;
    response["loadDecision"] = loadDecision;
    response["manualOverrideActive"] = manualOverrideActive;

  } else {
    response["success"] = false;
    response["error"] = "Unknown RPC method";
    Serial.print("[RPC !!] Unknown method received: ");
    Serial.println(method);
  }

  char buffer[384];
  size_t n = serializeJson(response, buffer, sizeof(buffer));
  String responseTopic = "v1/devices/me/rpc/response/" + requestId;
  mqtt.publish(responseTopic.c_str(), buffer, n);
  Serial.print("[MQTT >> RPC Response] ");
  Serial.println(buffer);
}
