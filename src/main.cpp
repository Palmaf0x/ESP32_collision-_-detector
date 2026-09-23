#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <String>

const char* ssid = "";
const char* password = "";
String serverURL = "";

const int triggPin = 26;
const int echoPin = 27;
const int ledPin = 32;
const int BuzzerPin = 33;

unsigned long ledStrartTime = 0;
const unsigned long Duration = 2000;
bool Active = false;

void connectWIFI () {
        WiFi.begin(ssid, password);
        Serial.print("Connecting ..");
        while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
            }
        Serial.println("");
        Serial.println("Connected to WiFi");
    }

void sendDATA (int value) {
        WiFiClient client;
        HTTPClient http;

        http.begin(client, serverURL);
        http.addHeader("Content-Type", "application/json");
        String payload = "{\"Distance\": " + to_string(value) + "}";
        int httpResponse = http.POST(payload);
        if (httpResponse > 0) {
                String response = http.getString();
                Serial.println("Response : " + response);
                 blinkConfirmation ();

            } else {
                    Serial.println("Error on sending POST : " + String(httpResponse));
                }
        http.end();
    }

int getDistance () {
        digitalWrite(triggPin, LOW);
        delayMicroseconds(2);
        digitalWrite(triggPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(triggPin, LOW);

        long duration = pusleIn(echoPin, HIGH);

        int distance = (duration * 0.0343) / 2;

        return distance;
    }

void blinkConfirmation () {
        digitalWrite(ledPin, HIGH);
        tone(BuzzerPin, 2000);
        Active = true;
        Serial.println("Led and Buzzer Actived !!");
        if (Active && (millis() - ledStrartTime >= Duration)) {
                digitalWrite(ledPin, LOW);
                noTone(BuzzerPin);
                Active = false;
                Serial.println("Led and Buzzer Turn Off .");
            }
    }

void setup () {
        Serial.begin(11500);
        pinMode(triggPin, OUTPUT);
        pinMode(echoPin, INPUT);
        pinMode(ledPin, OUTPUT);
        pinMode(BuzzerPin, OUTPUT);
        connectWIFI ();
    }

void loop {
        int sensor_distance = getDistance ();
        sendDATA(sensor_distance);
        delay(5000); // just for testing :)
    }