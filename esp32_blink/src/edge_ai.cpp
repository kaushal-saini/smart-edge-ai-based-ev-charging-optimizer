#include <time.h>
#include "edge_ai.h"
#include "state.h"
#include "sensors.h"
#include "model.h"

// NTP server for real-time synchronization
static const char* ntpServer = "pool.ntp.org";
static const long  gmtOffset_sec = 19800; // GMT+5:30 (or default)
static const int   daylightOffset_sec = 0;

void initEdgeAI() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

// Commute-hour historical profile (SRS Section 8.6.4 & generate_dataset.py)
float historicalArrivalRate(int hour) {
  if (hour >= 8 && hour <= 10)  return 0.62f; // Morning peak
  if (hour >= 18 && hour <= 21) return 0.58f; // Evening peak
  if (hour >= 0 && hour <= 5)   return 0.06f; // Late night
  if (hour >= 11 && hour <= 17) return 0.30f; // Mid-day
  return 0.18f;
}

void runEdgeAIInference() {
  struct tm timeinfo;
  int hourOfDay = 12;
  int dayOfWeek = 1;

  if (getLocalTime(&timeinfo, 50)) {
    hourOfDay = timeinfo.tm_hour;
    dayOfWeek = timeinfo.tm_wday;
  } else {
    // Fallback simulation clock advance if NTP is offline
    unsigned long sec = millis() / 1000;
    hourOfDay = (12 + (sec / 60)) % 24;
    dayOfWeek = 1;
  }

  lastHourOfDay = hourOfDay;
  lastDayOfWeek = dayOfWeek;

  float bayOccupiedF = (bayStatus == "CHARGING") ? 1.0f : 0.0f;
  float sessionElapsedMin = (bayStatus == "CHARGING" && sessionStartMs > 0)
      ? ((millis() - sessionStartMs) / 60000.0f)
      : 0.0f;
  float histRate = historicalArrivalRate(hourOfDay);
  float avgCurrent = recentAvgCurrent();

  // Run on-device inferences
  predictedArrivalProb = predictArrival(
      (float)hourOfDay, (float)dayOfWeek, bayOccupiedF,
      avgCurrent, sessionElapsedMin, histRate);

  if (bayStatus == "CHARGING") {
    float dur = predictDuration(
        (float)hourOfDay, (float)dayOfWeek, bayOccupiedF,
        avgCurrent, sessionElapsedMin, histRate);
    predictedDurationMin = (dur > 0.0f) ? (int)round(dur) : 0;
  } else {
    predictedDurationMin = 0;
  }
}
