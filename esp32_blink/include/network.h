#ifndef NETWORK_H
#define NETWORK_H

#include <WiFi.h>
#include <PubSubClient.h>

extern WiFiClient espClient;
extern PubSubClient mqtt;

void initNetwork();
void checkNetwork();
void connectWiFi();
void connectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);

#endif // NETWORK_H
