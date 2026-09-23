#include "sensors.h"
#include "config.h"
#include "state.h"

DHT dht(DHT_PIN, DHT_TYPE);

static unsigned long lastBtnPluginMs = 0;
static unsigned long lastBtnPlugoutMs = 0;
static const unsigned long DEBOUNCE_DELAY_MS = 200;

void initSensors() {
  pinMode(VOLTAGE_PIN, INPUT);
  pinMode(CURRENT_PIN, INPUT);
  pinMode(BTN_PLUGIN, INPUT_PULLUP);
  pinMode(BTN_PLUGOUT, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  dht.begin();
}

static float mapFloat(long x, long inMin, long inMax, float outMin, float outMax) {
  return (x - inMin) * (outMax - outMin) / (float)(inMax - inMin) + outMin;
}

void readSensors() {
  int rawVoltage = analogRead(VOLTAGE_PIN);
  int rawCurrent = analogRead(CURRENT_PIN);

  // Calibrate 0-4095 ADC to 0-250V and 0-32A
  voltage = mapFloat(rawVoltage, 0, 4095, 0.0f, 250.0f);
  
  if (bayStatus == "CHARGING") {
    current = mapFloat(rawCurrent, 0, 4095, 0.0f, 32.0f);
  } else {
    // If not charging, current draw is negligible / 0
    current = 0.0f;
  }

  power = voltage * current;

  float t = dht.readTemperature();
  if (!isnan(t)) {
    temperature = t;
  }

  recordCurrentSample(current);
}

void handleButtons() {
  unsigned long now = millis();

  // Active LOW buttons (connected to GND when pressed)
  if (digitalRead(BTN_PLUGIN) == LOW && (now - lastBtnPluginMs > DEBOUNCE_DELAY_MS)) {
    lastBtnPluginMs = now;
    if (bayStatus != "CHARGING") {
      bayStatus = "CHARGING";
      sessionStartMs = now;
      energyWh = 0.0f;
      manualOverrideActive = false;
      Serial.println("[EVENT] EV Plugged In -> Bay Status: CHARGING");
    }
  }

  if (digitalRead(BTN_PLUGOUT) == LOW && (now - lastBtnPlugoutMs > DEBOUNCE_DELAY_MS)) {
    lastBtnPlugoutMs = now;
    if (bayStatus == "CHARGING") {
      bayStatus = "FREE";
      sessionStartMs = 0;
      manualOverrideActive = false;
      throttleLevel = 0;
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("[EVENT] EV Unplugged -> Bay Status: FREE");
    }
  }
}

void updateEnergy(float dtSeconds) {
  if (bayStatus == "CHARGING" && dtSeconds > 0.0f) {
    // energyWh += power (W) * (dt (seconds) / 3600.0)
    energyWh += power * (dtSeconds / 3600.0f);
  }
}
