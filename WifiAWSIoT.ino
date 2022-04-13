#if defined(ESP32)
#include <WiFi.h>
#else
#include <WiFiNINA.h>
#endif

#include <PubSubClient.h>
#include <SSLClient.h>

#include "AWS_Root_CA.h"  // This file is created using AmazonRootCA1.pem from https://www.amazontrust.com/repository/AmazonRootCA1.pem

#define THING_NAME       "<Thing_Name>"
#define MQTT_PACKET_SIZE 1024

void MQTTPublish(const char *topic, char *payload);
void updateThing();

const char ssid[] = "";
const char pass[] = "";

// Add device certificate here.
const char my_cert[] =
    "-----BEGIN CERTIFICATE-----\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "-----END CERTIFICATE-----";

// Add private here.
const char my_key[] =
    "-----BEGIN RSA PRIVATE KEY-----\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "-----END RSA PRIVATE KEY-----";

SSLClientParameters mTLS = SSLClientParameters::fromPEM(my_cert, sizeof my_cert,
                                                        my_key, sizeof my_key);

const char *mqttServer = "ac375e2pwz4gz-ats.iot.us-west-2.amazonaws.com";
const char  publishShadowUpdate[] = "$aws/things/" THING_NAME "/shadow/update";
char        publishPayload[MQTT_PACKET_SIZE];
char *subscribeTopic[5] = {"$aws/things/" THING_NAME "/shadow/update/accepted",
                           "$aws/things/" THING_NAME "/shadow/update/rejected",
                           "$aws/things/" THING_NAME "/shadow/update/delta",
                           "$aws/things/" THING_NAME "/shadow/get/accepted",
                           "$aws/things/" THING_NAME "/shadow/get/rejected"};

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

WiFiClient   wifiClient;
SSLClient    wifiClientSSL(wifiClient, TAs, (size_t)TAs_NUM, 15);
PubSubClient mqtt(mqttServer, 8883, callback, wifiClientSSL);

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02};

void reconnect() {
    while (!mqtt.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (mqtt.connect("arduinoClient")) {
            Serial.println("connected");
            for (int i = 0; i < 5; i++) {
                //        Serial.println(subscribeTopic[i]);
                mqtt.subscribe(subscribeTopic[i]);
            }
            Serial.println("Started updateThing ");
            updateThing();
            Serial.println("Done updateThing ");

        } else {
            Serial.print("failed, rc=");
            Serial.print(mqtt.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void connectWiFi() {
    Serial.print("Attempting to connect to SSID: ");
    Serial.print(ssid);
    Serial.print(" ");

    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
        // failed, retry
        Serial.print(".");
        delay(5000);
    }
    Serial.println();

    Serial.println("You're connected to the network");
    Serial.println();
}

void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        ;  // wait for serial port to connect. Needed for native USB port only
    }
    wifiClientSSL.setMutualAuthParams(mTLS);
    mqtt.setBufferSize(MQTT_PACKET_SIZE);

    connectWiFi();
}

void loop() {
    if (!mqtt.connected()) {
        reconnect();
    }
    mqtt.loop();
}

void updateThing() {
    strcpy(publishPayload,
           "{\"state\": {\"reported\": {\"powerState\":\"ON\"}}}");
    MQTTPublish(publishShadowUpdate, publishPayload);
}

void MQTTPublish(const char *topic, char *payload) {
    mqtt.publish(topic, payload);
    Serial.print("Published [");
    Serial.print(topic);
    Serial.print("] ");
    Serial.println(payload);
}
