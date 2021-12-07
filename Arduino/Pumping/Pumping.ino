#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiClient.h>
#include "SuperSecretSettings.h" // File containing const char*'s for SSID & PASSWORD & KEY

// What it's all about :)
#include "opentherm.h"
#include "mqtt_opentherm.h"

#define TX 1
#define RX 3

#define THERMOSTAT_IN RX  // Pin 17 on OTGW PIC ic
#define THERMOSTAT_OUT 2  // Pin 3 on OTGW PIC ic
#define BOILER_IN TX      // Pin 18 on OTGW PIC ic
#define BOILER_OUT 0      // Pin 2 on OTGW PIC ic

#define TESTMODE 1

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

#if TESTMODE
WiFiServer telnet_server(999);
WiFiClient client;
#endif TESTMODE

void setup()
{

  // Safety delay to ensure that input pins are no longer high.
  delay(10 * 1000);

  //********** CHANGE PIN FUNCTION  TO GPIO **********
  //GPIO 1 (TX) swap the pin to a GPIO.
  pinMode(TX, FUNCTION_3);
  //  //GPIO 3 (RX) swap the pin to a GPIO.
  pinMode(RX, FUNCTION_3);
  //  //**************************************************
  delay(200);
  // setup_digitalRead();
  setup_pins();

  // from mqtt_opentherm.h
  mqtt_setup();

  WiFi.mode(WIFI_STA);
  connectToWifi();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  //Updater
  httpUpdater.setup(&server, OTAUSER, OTAPASSWORD);

  server.begin();
#if TESTMODE
  telnet_server.begin();
#endif TESTMODE

  mqttClient.publish("iot/boiler/version_loaded", 1, true, __DATE__ " " __TIME__);

}


#define MODE_LISTEN_MASTER 0
#define MODE_LISTEN_SLAVE 1
uint_least8_t mode = MODE_LISTEN_MASTER;
OpenthermData message;

void loop()
{
  server.handleClient();
#if TESTMODE == 1
  client = telnet_server.available();
  if (client) {
    while (client.connected()) {
      server.handleClient();
      test_loop();
    }
  }
#else
  no_client_connected_loop();
#endif TESTMODE
}

void setup_pins() {
  pinMode(THERMOSTAT_IN, INPUT);
  digitalWrite(THERMOSTAT_IN, HIGH); // pull up
  digitalWrite(THERMOSTAT_OUT, HIGH);
  pinMode(THERMOSTAT_OUT, OUTPUT); // low output = high current, high output = low current
  pinMode(BOILER_IN, INPUT);
  digitalWrite(BOILER_IN, HIGH); // pull up
  digitalWrite(BOILER_OUT, HIGH);
  pinMode(BOILER_OUT, OUTPUT); // low output = high voltage, high output = low voltage

  OPENTHERM::setuppp_digitalRead();
}

void no_client_connected_loop() {
  if (mode == MODE_LISTEN_MASTER) {
    if (OPENTHERM::isSent() || OPENTHERM::isIdle() || OPENTHERM::isError()) {
      OPENTHERM::listen(THERMOSTAT_IN);
    }
    else if (OPENTHERM::getMessage(message)) {
      OPENTHERM::send(BOILER_OUT, message); // forward message to boiler
      mode = MODE_LISTEN_SLAVE;
      mqttClient.publish("iot/boiler/OTMessage", 0, true, OPENTHERM::toFormattedString(message).c_str());
    }
  }
  else if (mode == MODE_LISTEN_SLAVE) {
    if (OPENTHERM::isSent()) {
      OPENTHERM::listen(BOILER_IN, 800); // response need to be send back by boiler within 800ms
    }
    else if (OPENTHERM::getMessage(message)) {
      OPENTHERM::send(THERMOSTAT_OUT, message); // send message back to thermostat
      mode = MODE_LISTEN_MASTER;
      mqttClient.publish("iot/boiler/OTMessage", 0, true, OPENTHERM::toFormattedString(message).c_str());
    }
    else if (OPENTHERM::isError()) {
      mode = MODE_LISTEN_MASTER;
    }
  }
}

#if TESTMODE
void test_loop() {

  client.println(F("--- START OF TEST CYCLE---"));

  client.print(F("Boiler inbound, thermostat outbound .. "));
  digitalWrite(THERMOSTAT_OUT, HIGH);
  digitalWrite(BOILER_OUT, HIGH);
  delay(10);
  // print_test_stats();
  // delay(2000);

  if (OPENTHERM::my_digitalRead(THERMOSTAT_IN) == 0 && OPENTHERM::my_digitalRead(BOILER_IN) == 0) { // ok
    client.print(F("line above if clause succes:  "));
    client.println(__LINE__);
    // thermostat out low -> boiler in high
    digitalWrite(THERMOSTAT_OUT, LOW);

    client.println(F("Setting THERMOSTAT_OUT, LOW - BOILER_IN should now be HIGH"));
    print_test_stats();
    server.handleClient();
    delay(1000);
    server.handleClient();

    if (OPENTHERM::my_digitalRead(THERMOSTAT_IN) == 0 && OPENTHERM::my_digitalRead(BOILER_IN) == 1) { // ok
      client.print(F("line above if clause succes:  "));
      client.println(__LINE__);
      client.println(F("OK"));
    }
    else {
      client.print(F("line above else clause triggered:  "));
      client.println(__LINE__);
      client.println(F("Failed"));
      client.println(F("Boiler is not registering signal or thermostat is not sending properly"));
    }
  }
  else {
    client.print(F("line above else clause triggered:  "));
    client.println(__LINE__);
    client.println(F("Failed"));
    client.println(F("Boiler is high even if no signal is being sent"));
  }

  client.print(F("Boiler outbound, thermostat inbound .. "));
  digitalWrite(THERMOSTAT_OUT, HIGH);
  digitalWrite(BOILER_OUT, HIGH);
  delay(10);

  //   print_test_stats();
  // delay(2000);

  if (OPENTHERM::my_digitalRead(THERMOSTAT_IN) == 0 && OPENTHERM::my_digitalRead(BOILER_IN) == 0) { // ok
    client.print(F("line above if clause succes:  "));
    client.println(__LINE__);
    // boiler out low -> thermostat in high
    digitalWrite(BOILER_OUT, LOW);

    client.println(F("Setting BOILER_OUT, LOW - THERMOSTAT_IN should now be HIGH"));
    print_test_stats();
    server.handleClient();
    delay(1000);
    server.handleClient();
    delay(10);

    if (OPENTHERM::my_digitalRead(THERMOSTAT_IN) == 1 && OPENTHERM::my_digitalRead(BOILER_IN) == 0) { // ok
      client.print(F("line above if clause succes:  "));
      client.println(__LINE__);
      client.println(F("OK"));
    }
    else {
      client.print(F("line above else clause triggered:  "));
      client.println(__LINE__);
      client.println(F("Failed"));
      client.println(F("Thermostat is not registering signal or boiler is not sending properly"));
    }
  }
  else {
    client.print(F("line above else clause triggered:  "));
    client.println(__LINE__);
    client.println(F("Failed"));
    client.println(F("Thermostat is high even if no signal is being sent"));
  }

  print_test_stats();

  client.println(F("--- END OF TEST CYCLE---"));
  server.handleClient();
  delay(5000);
  server.handleClient();
}

void print_test_stats() {

  client.println(F("THERMOSTAT_IN:  "));
  client.println(OPENTHERM::my_digitalRead(THERMOSTAT_IN));
  client.print(F("THERMOSTAT_OUT: "));
  client.println(OPENTHERM::my_digitalRead(THERMOSTAT_OUT));
  client.print(F("BOILER_IN:  "));
  client.println(OPENTHERM::my_digitalRead(BOILER_IN));
  client.print(F("BOILER_OUT: "));
  client.println(OPENTHERM::my_digitalRead(BOILER_OUT));

}

#endif
