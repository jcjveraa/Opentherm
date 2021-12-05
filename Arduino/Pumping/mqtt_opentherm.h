#pragma once
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include "SuperSecretSettings.h"

void   mqtt_setup();
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
AsyncMqttClient mqttClient;
void connectToWifi();