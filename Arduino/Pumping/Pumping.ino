#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiClient.h>
#include "SuperSecretSettings.h" // File containing const char*'s for SSID & PASSWORD & KEY

// What it's all about :)
#include "opentherm.h"

#define TX 1
  #define RX 3

#define THERMOSTAT_IN RX  // Pin 17 on OTGW PIC ic
#define THERMOSTAT_OUT 2  // Pin 3 on OTGW PIC ic
#define BOILER_IN TX      // Pin 18 on OTGW PIC ic
#define BOILER_OUT 0      // Pin 2 on OTGW PIC ic

#define TESTMODE 0

ESP8266WebServer server(80);
WiFiServer telnet_server(999);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient client;

void setup()
{

  // Safety delay to ensure that input pins are no longer high.
  delay(10*1000);

  //********** CHANGE PIN FUNCTION  TO GPIO **********
  //GPIO 1 (TX) swap the pin to a GPIO.
  pinMode(TX, FUNCTION_3); 
  //GPIO 3 (RX) swap the pin to a GPIO.
  pinMode(RX, FUNCTION_3); 
  //**************************************************
  delay(20);
  // setup_digitalRead();
  setup_pins();

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_1, PASSWORD_1);
  while (WiFi.status() != WL_CONNECTED)
  {
      delay(500);
  }

  //Updater
  httpUpdater.setup(&server, OTAUSER, OTAPASSWORD);

  server.begin();
  telnet_server.begin();
  // Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());

}

#define MODE_LISTEN_MASTER 0
#define MODE_LISTEN_SLAVE 1
uint_least8_t mode = MODE_LISTEN_MASTER;
OpenthermData message;

void loop()
{
  server.handleClient();

  if (telnet_server.hasClient()) {
    client = telnet_server.available();
  }
  
  // wait for a client (web browser) to connect
  if (client)
  {
    client.println(F("\n[Client connected]"));

    while (client.connected())
    {
      server.handleClient();

      #if TESTMODE==0
      gateway_loop();
      #endif

      #if TESTMODE
      test_loop();
      #endif

    }

    // close the connection:
    client.stop();
  // Serial.println("[Client disconnected]");
  }
  else {
    no_client_connected_loop();
  }
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
}

void gateway_loop() {
  if (mode == MODE_LISTEN_MASTER) {
    if (OPENTHERM::isSent() || OPENTHERM::isIdle() || OPENTHERM::isError()) {
      OPENTHERM::listen(THERMOSTAT_IN);
    }
    else if (OPENTHERM::getMessage(message)) {
      client.println(OPENTHERM::toOTGWSerialString(message));
      OPENTHERM::send(BOILER_OUT, message); // forward message to boiler
      mode = MODE_LISTEN_SLAVE;
    }
  }
  else if (mode == MODE_LISTEN_SLAVE) {
    if (OPENTHERM::isSent()) {
      OPENTHERM::listen(BOILER_IN, 800); // response need to be send back by boiler within 800ms
    }
    else if (OPENTHERM::getMessage(message)) {
      client.println(OPENTHERM::toOTGWSerialString(message));
      OPENTHERM::send(THERMOSTAT_OUT, message); // send message back to thermostat
      mode = MODE_LISTEN_MASTER;
    }
    else if (OPENTHERM::isError()) {
      mode = MODE_LISTEN_MASTER;
    }
  }
}

void no_client_connected_loop() {
  if (mode == MODE_LISTEN_MASTER) {
    if (OPENTHERM::isSent() || OPENTHERM::isIdle() || OPENTHERM::isError()) {
      OPENTHERM::listen(THERMOSTAT_IN);
    }
    else if (OPENTHERM::getMessage(message)) {
      OPENTHERM::send(BOILER_OUT, message); // forward message to boiler
      mode = MODE_LISTEN_SLAVE;
    }
  }
  else if (mode == MODE_LISTEN_SLAVE) {
    if (OPENTHERM::isSent()) {
      OPENTHERM::listen(BOILER_IN, 800); // response need to be send back by boiler within 800ms
    }
    else if (OPENTHERM::getMessage(message)) {
      OPENTHERM::send(THERMOSTAT_OUT, message); // send message back to thermostat
      mode = MODE_LISTEN_MASTER;
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

  if (digitalRead(THERMOSTAT_IN) == 0 && digitalRead(BOILER_IN) == 0) { // ok
    client.print(F("line above if clause succes:  "));
    client.println(__LINE__);
    // thermostat out low -> boiler in high
    digitalWrite(THERMOSTAT_OUT, LOW);

    client.println(F("Setting THERMOSTAT_OUT, LOW - BOILER_IN should now be HIGH"));
    print_test_stats();
    server.handleClient();
    delay(1000);
    server.handleClient();

    if (digitalRead(THERMOSTAT_IN) == 0 && digitalRead(BOILER_IN) == 1) { // ok
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

  if (digitalRead(THERMOSTAT_IN) == 0 && digitalRead(BOILER_IN) == 0) { // ok
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

    if (digitalRead(THERMOSTAT_IN) == 1 && digitalRead(BOILER_IN) == 0) { // ok
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
  client.println(digitalRead(THERMOSTAT_IN));
  client.print(F("THERMOSTAT_OUT: "));
  client.println(digitalRead(THERMOSTAT_OUT));
  client.print(F("BOILER_IN:  "));
  client.println(digitalRead(BOILER_IN));
  client.print(F("BOILER_OUT: "));
  client.println(digitalRead(BOILER_OUT));
  
  }

#endif
