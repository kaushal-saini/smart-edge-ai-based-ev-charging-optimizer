#include "config.h"

// ---------------- WiFi Network Credentials ----------------
// Default Wokwi simulation network (leave password empty)
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";

// ---------------- ThingsBoard Connection Details ----------------
const char* MQTT_SERVER = "mqtt.thingsboard.cloud";
const int   MQTT_PORT   = 1883;

// ThingsBoard Device Access Token
// Replace placeholder with your individual Device Access Token from ThingsBoard
const char* TB_TOKEN = "YOUR_THINGSBOARD_DEVICE_ACCESS_TOKEN";
const char* BAY_ID   = "BAY1";
const char* FIRMWARE_VERSION = "1.0.0";
