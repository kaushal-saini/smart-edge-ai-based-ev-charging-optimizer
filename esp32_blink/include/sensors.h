#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <DHT.h>

extern DHT dht;

void initSensors();
void readSensors();
void handleButtons();
void updateEnergy(float dtSeconds);

#endif // SENSORS_H
