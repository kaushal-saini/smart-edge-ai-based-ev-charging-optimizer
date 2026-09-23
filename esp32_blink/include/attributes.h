#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <Arduino.h>
#include <ArduinoJson.h>

void requestSharedAttributes();
void applySharedAttributes(JsonObject attrs);

#endif // ATTRIBUTES_H
