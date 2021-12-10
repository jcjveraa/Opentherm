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

#define THERMOSTAT_IN RX // Pin 17 on OTGW PIC ic
#define THERMOSTAT_OUT 2 // Pin 3 on OTGW PIC ic
#define BOILER_IN TX     // Pin 18 on OTGW PIC ic
#define BOILER_OUT 0     // Pin 2 on OTGW PIC ic

#define TESTMODE 0

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

#define MODE_LISTEN_MASTER 0
#define MODE_LISTEN_SLAVE 1
uint_least8_t mode = MODE_LISTEN_MASTER;
OpenthermData message;

bool openthermActive = true;

byte CH_SETPOINT_HB_OVERRIDE = 0;
const char *CH_SETPOINT_OVERRIDE_TOPIC = "iot/boiler/overrides/CH_SETPOINT";

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
    server.on("/", redirectToUpdate);
    server.on("/stop", stopOpentherm);
    server.on("/start", startOpentherm);

    server.begin();
#if TESTMODE
    telnet_server.begin();
#endif TESTMODE

    mqttClient.publish("iot/boiler/OTGateway/version_loaded", 1, true, __DATE__ " " __TIME__);
    mqttClient.publish("iot/boiler/OTGateway/IP", 1, WiFi.localIP());

    mqttClient.subscribe(CH_SETPOINT_OVERRIDE_TOPIC, 2);
    mqttClient.subscribe("iot/boiler/overrides/start_stop", 2);
    mqttClient.onMessage(onMqttMessage);
    startOpentherm();
}

void loop()
{
    server.handleClient();
#if TESTMODE == 1
    client = telnet_server.available();
    if (client)
    {
        while (client.connected())
        {
            server.handleClient();
            test_loop();
        }
    }
#else
    if (openthermActive)
    {
        no_client_connected_loop();
    }
#endif TESTMODE
}

void setup_pins()
{
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

void no_client_connected_loop()
{
    if (mode == MODE_LISTEN_MASTER)
    {
        if (OPENTHERM::isSent() || OPENTHERM::isIdle() || OPENTHERM::isError())
        {
            OPENTHERM::listen(THERMOSTAT_IN, 10000);
            if (OPENTHERM::isError())
            {
                mqttClient.publish("iot/boiler/error", 0, true, "timeout MODE_LISTEN_MASTER");
            }
        }
        else if (OPENTHERM::getMessage(message))
        {
            if (message.id == OT_MSGID_CH_SETPOINT && CH_SETPOINT_HB_OVERRIDE != 0)
            {
                char buffer[4];
                itoa(message.valueHB, buffer, 10);
                mqttClient.publish("iot/boiler/overrides/CH_SETPOINT_RECEIVED", 0, true, buffer);
                itoa(CH_SETPOINT_HB_OVERRIDE, buffer, 10);
                mqttClient.publish("iot/boiler/overrides/CH_SETPOINT_OVERRIDE", 0, true, buffer);

                // Actual override
                if (message.valueHB > CH_SETPOINT_HB_OVERRIDE)
                {
                    message.valueHB = CH_SETPOINT_HB_OVERRIDE;
                }
            }
            OPENTHERM::send(BOILER_OUT, message); // forward message to boiler
            mode = MODE_LISTEN_SLAVE;
            mqttClient.publish("iot/boiler/OTMessage", 0, true, OPENTHERM::toFormattedString(message).c_str());
        }
    }
    else if (mode == MODE_LISTEN_SLAVE)
    {
        if (OPENTHERM::isSent())
        {
            OPENTHERM::listen(BOILER_IN, 800); // response need to be send back by boiler within 800ms
        }
        else if (OPENTHERM::getMessage(message))
        {
            // Override the max of the boiler
            if (message.id == OT_MSGID_MAX_CH_SETPOINT && CH_SETPOINT_HB_OVERRIDE != 0)
            {
                // Actual override
                if (message.valueHB > CH_SETPOINT_HB_OVERRIDE)
                {
                    message.valueHB = CH_SETPOINT_HB_OVERRIDE;
                }
            }
            OPENTHERM::send(THERMOSTAT_OUT, message); // send message back to thermostat
            mode = MODE_LISTEN_MASTER;
            mqttClient.publish("iot/boiler/OTMessage", 0, true, OPENTHERM::toFormattedString(message).c_str());
        }
        else if (OPENTHERM::isError())
        {
            mode = MODE_LISTEN_MASTER;
            mqttClient.publish("iot/boiler/error", 0, true, "timeout MODE_LISTEN_SLAVE");
        }
    }
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    mqttClient.publish("iot/boiler/overrideTest_2", 0, true, payload);
    if (strcmp(CH_SETPOINT_OVERRIDE_TOPIC, topic) == 0)
    {
        CH_SETPOINT_HB_OVERRIDE = (byte)atoi(payload);
        if (CH_SETPOINT_HB_OVERRIDE == 0)
        {
            mqttClient.publish("iot/boiler/overrides/CH_SETPOINT_RECEIVED", 0, true, "");
            mqttClient.publish("iot/boiler/overrides/CH_SETPOINT_OVERRIDE", 0, true, "");
        }
    }
}

void startOpentherm()
{
    mode = MODE_LISTEN_MASTER;
    openthermActive = true;
    mqttClient.publish("iot/boiler/OTGateway/status", 1, true, "started");
    server.send(200);
}

void stopOpentherm()
{
    OPENTHERM::stop();
    openthermActive = false;
    mqttClient.publish("iot/boiler/OTGateway/status", 1, true, "stopped");
    // server.send(200);
    redirectToUpdate();
}

void redirectToUpdate()
{
    //   TRACE("Redirect...");
    String url = "/update";

    server.sendHeader("Location", url, true);
    server.send(302);
}

#if TESTMODE
void test_loop()
{

    client.println(F("--- START OF TEST CYCLE---"));

    client.print(F("Boiler inbound, thermostat outbound .. "));
    digitalWrite(THERMOSTAT_OUT, HIGH);
    digitalWrite(BOILER_OUT, HIGH);
    delay(10);
    // print_test_stats();
    // delay(2000);

    if (OPENTHERM::my_digitalRead(THERMOSTAT_IN) == 0 && OPENTHERM::my_digitalRead(BOILER_IN) == 0)
    { // ok
        client.print(F("line above if clause succes:  "));
        client.println(__LINE__);
        // thermostat out low -> boiler in high
        digitalWrite(THERMOSTAT_OUT, LOW);

        client.println(F("Setting THERMOSTAT_OUT, LOW - BOILER_IN should now be HIGH"));
        print_test_stats();
        server.handleClient();
        delay(1000);
        server.handleClient();

        if (OPENTHERM::my_digitalRead(THERMOSTAT_IN) == 0 && OPENTHERM::my_digitalRead(BOILER_IN) == 1)
        { // ok
            client.print(F("line above if clause succes:  "));
            client.println(__LINE__);
            client.println(F("OK"));
        }
        else
        {
            client.print(F("line above else clause triggered:  "));
            client.println(__LINE__);
            client.println(F("Failed"));
            client.println(F("Boiler is not registering signal or thermostat is not sending properly"));
        }
    }
    else
    {
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

    if (OPENTHERM::my_digitalRead(THERMOSTAT_IN) == 0 && OPENTHERM::my_digitalRead(BOILER_IN) == 0)
    { // ok
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

        if (OPENTHERM::my_digitalRead(THERMOSTAT_IN) == 1 && OPENTHERM::my_digitalRead(BOILER_IN) == 0)
        { // ok
            client.print(F("line above if clause succes:  "));
            client.println(__LINE__);
            client.println(F("OK"));
        }
        else
        {
            client.print(F("line above else clause triggered:  "));
            client.println(__LINE__);
            client.println(F("Failed"));
            client.println(F("Thermostat is not registering signal or boiler is not sending properly"));
        }
    }
    else
    {
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

void print_test_stats()
{

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
