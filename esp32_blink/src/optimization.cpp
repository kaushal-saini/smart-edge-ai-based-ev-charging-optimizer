#include "optimization.h"
#include "config.h"
#include "state.h"

// ---------------------------------------------------------------------
// SRS Section 8.7: Local Charger Allocation / Optimization Algorithm
// ---------------------------------------------------------------------
void runOptimization() {
  if (manualOverrideActive) {
    // If operator has overridden relay state or throttle via RPC, retain it
    return;
  }

  overloadActive = false;

  if (bayStatus != "CHARGING") {
    bool isPeak = (lastHourOfDay >= peakTariffStartHr && lastHourOfDay <= peakTariffEndHr);

    if (isPeak && predictedArrivalProb >= predictionThreshold) {
      loadDecision = "STANDBY_EXPECTING";
      throttleLevel = 0;
    } else if (isPeak) {
      loadDecision = "DEFER";
      throttleLevel = 0;
    } else {
      loadDecision = "ALLOW";
      throttleLevel = 100;
    }
    return;
  }

  // Active Charging Optimization
  if (current > overloadCurrentA) {
    // Overcurrent condition -> safety throttle to 50% & activate red alarm
    loadDecision = "THROTTLE";
    throttleLevel = 50;
    overloadActive = true;
    Serial.println("!! [OPTIMIZER] Overcurrent detected! Throttling to 50% & raising alarm.");
  } else if (power > maxStationLoadW) {
    // Station power cap exceeded -> prioritize sessions closer to completion
    if (predictedDurationMin > 10) {
      loadDecision = "THROTTLE";
      throttleLevel = 70;
      Serial.println("!! [OPTIMIZER] Max station power cap exceeded -> Throttling bay to 70%.");
    } else {
      loadDecision = "ALLOW";
      throttleLevel = 100;
      Serial.println(">> [OPTIMIZER] Station power high, but session near completion -> Maintaining 100%.");
    }
  } else {
    // Normal charging condition
    loadDecision = "ALLOW";
    throttleLevel = 100;
  }
}

// ---------------------------------------------------------------------
// FR-6: Apply throttleLevel to relay via time-sliced duty cycling
// ---------------------------------------------------------------------
void applyRelayDutyCycle() {
  if (bayStatus != "CHARGING") {
    digitalWrite(RELAY_PIN, LOW);
    return;
  }

  if (throttleLevel >= 100) {
    digitalWrite(RELAY_PIN, HIGH);
    return;
  }

  if (throttleLevel <= 0) {
    digitalWrite(RELAY_PIN, LOW);
    return;
  }

  unsigned long phase = millis() % DUTY_CYCLE_WINDOW_MS;
  unsigned long onTime = (DUTY_CYCLE_WINDOW_MS * (unsigned long)throttleLevel) / 100UL;
  digitalWrite(RELAY_PIN, phase < onTime ? HIGH : LOW);
}

// ---------------------------------------------------------------------
// Status LEDs indication (SRS Section 8.1)
// Green: Free | Yellow: Charging | Red: Fault / Overload
// ---------------------------------------------------------------------
void updateLeds() {
  if (overloadActive) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, LOW);
  } else if (bayStatus == "CHARGING") {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_GREEN, LOW);
  } else { // FREE
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, HIGH);
  }
}
