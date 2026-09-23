#include <Arduino.h>
#include "config.h"
#include "state.h"
#include "sensors.h"
#include "edge_ai.h"
#include "optimization.h"
#include "network.h"
#include "telemetry.h"

static unsigned long lastSampleTimeMs = 0;
static unsigned long lastEnergyUpdateMs = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=======================================================");
  Serial.println("  Smart EV Charging Station Optimizer (Edge AI)");
  Serial.println("  ESP32 + MQTT + ThingsBoard + Lightweight ML");
  Serial.println("=======================================================\n");

  initSensors();
  initEdgeAI();
  initNetwork();

  lastSampleTimeMs = millis();
  lastEnergyUpdateMs = millis();
}

void loop() {
  // 1. Maintain Network & MQTT connection
  checkNetwork();

  // 2. Poll push buttons (EV plug-in / plug-out events)
  handleButtons();

  // 3. Fast duty-cycle PWM control on relay & LED update
  applyRelayDutyCycle();
  updateLeds();

  // 4. Fine-grained energy accumulator
  unsigned long now = millis();
  float dtSeconds = (now - lastEnergyUpdateMs) / 1000.0f;
  if (dtSeconds >= 0.5f) {
    updateEnergy(dtSeconds);
    lastEnergyUpdateMs = now;
  }

  // 5. Periodic Sensing, Edge Inference, Optimization & Telemetry Cycle
  if (now - lastSampleTimeMs >= SAMPLE_INTERVAL_MS) {
    lastSampleTimeMs = now;

    // Read analog sensors & temperature
    readSensors();

    // Run on-device Edge AI Inference (Arrival Prob & Duration)
    runEdgeAIInference();

    // Execute local optimization / throttling / deferral algorithm
    runOptimization();

    // Publish telemetry to ThingsBoard
    publishTelemetry();

    // Print summary to serial monitor
    Serial.println("-------------------------------------------------------");
    Serial.printf("[BAY %s STATUS] %s | V: %.1fV | I: %.1fA | P: %.1fW | E: %.2fWh | T: %.1fC\n",
                  BAY_ID, bayStatus.c_str(), voltage, current, power, energyWh, temperature);
    Serial.printf("[EDGE AI PREDICTIONS] ArrProb: %.2f | EstDur: %d min\n",
                  predictedArrivalProb, predictedDurationMin);
    Serial.printf("[OPTIMIZER DECISION] %s | Throttle: %d%% | Overload: %s | ManualOverride: %s\n",
                  loadDecision.c_str(), throttleLevel,
                  overloadActive ? "YES" : "NO",
                  manualOverrideActive ? "YES" : "NO");
    Serial.println("-------------------------------------------------------\n");
  }
}
