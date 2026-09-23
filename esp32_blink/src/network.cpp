#include <ArduinoJson.h>
#include "network.h"
#include "config.h"
#include "attributes.h"
#include "rpc.h"
#include "telemetry.h"

WiFiClient espClient;
PubSubClient mqtt(espClient);

static unsigned long lastMqttRetryMs = 0;

void initNetwork() {
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  mqtt.setBufferSize(512); // Ensure adequate buffer for JSON payloads
  connectWiFi();
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Connecting to WiFi: ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long startMs = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startMs < 10000) {
    delay(300);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" Connected!");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(" Failed! Will retry in background.");
  }
}

void connectMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
    return;
  }

  if (mqtt.connected()) return;

  unsigned long now = millis();
  if (now - lastMqttRetryMs < 3000) return; // Non-blocking retry interval
  lastMqttRetryMs = now;

  Serial.print("Connecting to ThingsBoard MQTT (");
  Serial.print(MQTT_SERVER);
  Serial.print(")...");

  // ThingsBoard uses TB_TOKEN as MQTT username
  if (mqtt.connect(BAY_ID, TB_TOKEN, NULL)) {
    Serial.println(" Connected!");

    // Subscribe to ThingsBoard MQTT endpoints (SRS Section 7.3)
    mqtt.subscribe("v1/devices/me/attributes");             // Shared attributes push
    mqtt.subscribe("v1/devices/me/attributes/response/+");  // Shared attributes query response
    mqtt.subscribe("v1/devices/me/rpc/request/+");          // RPC requests

    // Publish client attributes and request shared attributes
    publishClientAttributes();
    requestSharedAttributes();
  } else {
    Serial.print(" Failed, rc=");
    Serial.println(mqtt.state());
  }
}

void checkNetwork() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  if (!mqtt.connected()) {
    connectMQTT();
  }
  mqtt.loop();
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String topicStr = String(topic);
  char buf[512];
  unsigned int n = length < (sizeof(buf) - 1) ? length : (sizeof(buf) - 1);
  memcpy(buf, payload, n);
  buf[n] = '\0';

  Serial.print("[MQTT <<] ");
  Serial.print(topicStr);
  Serial.print(" -> ");
  Serial.println(buf);

  // 1. Handle RPC Requests
  if (topicStr.startsWith("v1/devices/me/rpc/request/")) {
    String requestId = topicStr.substring(topicStr.lastIndexOf('/') + 1);
    handleRpc(requestId, buf);
    return;
  }

  // 2. Handle Shared Attributes (both Push and Response)
  StaticJsonDocument<384> doc;
  DeserializationError err = deserializeJson(doc, buf);
  if (err) {
    Serial.println("[MQTT !!] Failed to parse attribute JSON payload");
    return;
  }

  JsonObject attrs = doc.containsKey("shared") ? doc["shared"].as<JsonObject>() : doc.as<JsonObject>();
  applySharedAttributes(attrs);
}
