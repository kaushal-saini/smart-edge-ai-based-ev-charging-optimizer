#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <DHT.h>

// ---------------------------------------------------------------------
// Pin Configuration — SRS Section 8.1 & Diagram.json
// ---------------------------------------------------------------------
#define VOLTAGE_PIN         34
#define CURRENT_PIN         35
#define DHT_PIN             15
#define DHT_TYPE            DHT22
#define RELAY_PIN           26
#define BTN_PLUGIN          32
#define BTN_PLUGOUT         33
#define LED_GREEN           18
#define LED_YELLOW          19
#define LED_RED             21

// ---------------------------------------------------------------------
// Network & ThingsBoard Credentials
// ---------------------------------------------------------------------
extern const char* WIFI_SSID;
extern const char* WIFI_PASS;
extern const char* MQTT_SERVER;
extern const int   MQTT_PORT;
extern const char* TB_TOKEN;
extern const char* BAY_ID;
extern const char* FIRMWARE_VERSION;

// ---------------------------------------------------------------------
// Timing & Intervals
// ---------------------------------------------------------------------
#define SAMPLE_INTERVAL_MS      5000   // Telemetry & sensing interval
#define DUTY_CYCLE_WINDOW_MS    2000   // Relay PWM modulation window

#endif // CONFIG_H
