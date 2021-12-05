#pragma once
#ifndef MQTT_OPENTHERM_H
#define MQTT_OPENTHERM_H

//#include <ESP8266WiFi.h>
//#include <Ticker.h>
#include <AsyncMqttClient.h>
//#include "SuperSecretSettings.h"

void   mqtt_setup();
//void onMqttDisconnect();
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
extern AsyncMqttClient mqttClient;
void connectToWifi();

#endif
