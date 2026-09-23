#ifndef EDGE_AI_H
#define EDGE_AI_H

#include <Arduino.h>

void initEdgeAI();
float historicalArrivalRate(int hour);
void runEdgeAIInference();

#endif // EDGE_AI_H
