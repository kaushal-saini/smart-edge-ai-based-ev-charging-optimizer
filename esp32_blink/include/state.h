#ifndef STATE_H
#define STATE_H

#include <Arduino.h>

// ---------------------------------------------------------------------
// Live Bay Telemetry & State (SRS Section 8.3)
// ---------------------------------------------------------------------
extern String bayStatus;
extern float voltage;
extern float current;
extern float power;
extern float energyWh;
extern float temperature;

// ---------------------------------------------------------------------
// Edge AI Inference Outputs (SRS Section 8.6)
// ---------------------------------------------------------------------
extern float predictedArrivalProb;
extern int   predictedDurationMin;

// ---------------------------------------------------------------------
// Optimization Decisions & Control State (SRS Section 8.7)
// ---------------------------------------------------------------------
extern String loadDecision;
extern int    throttleLevel;
extern bool   overloadActive;
extern bool   manualOverrideActive;

// ---------------------------------------------------------------------
// Session & Timing State
// ---------------------------------------------------------------------
extern unsigned long sessionStartMs;
extern int lastHourOfDay;
extern int lastDayOfWeek;

// ---------------------------------------------------------------------
// Shared Attributes (Configurable via ThingsBoard Cloud — SRS Section 8.4)
// ---------------------------------------------------------------------
extern float maxStationLoadW;
extern float overloadCurrentA;
extern int   peakTariffStartHr;
extern int   peakTariffEndHr;
extern float predictionThreshold;

// ---------------------------------------------------------------------
// Current History Buffer for Feature Extraction
// ---------------------------------------------------------------------
#define CURRENT_HISTORY_SIZE 10
void recordCurrentSample(float currentVal);
float recentAvgCurrent();

#endif // STATE_H
