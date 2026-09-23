#include "state.h"

// ---------------------------------------------------------------------
// Live Bay Telemetry & State
// ---------------------------------------------------------------------
String bayStatus = "FREE";
float voltage = 0.0f;
float current = 0.0f;
float power = 0.0f;
float energyWh = 0.0f;
float temperature = 25.0f;

// ---------------------------------------------------------------------
// Edge AI Inference Outputs
// ---------------------------------------------------------------------
float predictedArrivalProb = 0.0f;
int   predictedDurationMin = 0;

// ---------------------------------------------------------------------
// Optimization Decisions & Control State
// ---------------------------------------------------------------------
String loadDecision = "ALLOW";
int    throttleLevel = 100;
bool   overloadActive = false;
bool   manualOverrideActive = false;

// ---------------------------------------------------------------------
// Session & Timing State
// ---------------------------------------------------------------------
unsigned long sessionStartMs = 0;
int lastHourOfDay = 12;
int lastDayOfWeek = 1;

// ---------------------------------------------------------------------
// Shared Attributes Defaults (SRS Section 8.4)
// ---------------------------------------------------------------------
float maxStationLoadW = 6000.0f;
float overloadCurrentA = 16.0f;
int   peakTariffStartHr = 18;
int   peakTariffEndHr = 22;
float predictionThreshold = 0.5f;

// ---------------------------------------------------------------------
// Current History Buffer Implementation
// ---------------------------------------------------------------------
static float currentHistory[CURRENT_HISTORY_SIZE] = {0};
static int historyIndex = 0;
static int historyCount = 0;

void recordCurrentSample(float currentVal) {
  currentHistory[historyIndex] = currentVal;
  historyIndex = (historyIndex + 1) % CURRENT_HISTORY_SIZE;
  if (historyCount < CURRENT_HISTORY_SIZE) {
    historyCount++;
  }
}

float recentAvgCurrent() {
  if (historyCount == 0) return 0.0f;
  float sum = 0.0f;
  for (int i = 0; i < historyCount; i++) {
    sum += currentHistory[i];
  }
  return sum / (float)historyCount;
}
