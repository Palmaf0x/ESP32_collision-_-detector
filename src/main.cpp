#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "BELL721";
const char* password = "766654CEA1A9";
String serverURL = "http://192.168.2.78:5000/api/sensor"; // Fixed port colon separator

const int triggPin = 26;
const int echoPin = 27;
const int ledPin = 32;
const int buzzerPin = 33;

unsigned long ledStartTime = 0;
const unsigned long duration = 2000;
bool active = false;

void connectWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Connecting..");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
}

void activateConfirmation() {
    digitalWrite(ledPin, HIGH);
    tone(buzzerPin, 2000);
    ledStartTime = millis(); // Record activation timestamp
    active = true;
    Serial.println("LED and Buzzer Activated!");
}

void updateBlinkState() {
    // Non-blocking timer check to turn off LED and Buzzer after duration
    if (active && (millis() - ledStartTime >= duration)) {
        digitalWrite(ledPin, LOW);
        noTone(buzzerPin);
        active = false;
        Serial.println("LED and Buzzer Turned Off.");
    }
}

void sendData(int value) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverURL);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"Distance\": " + String(value) + "}";
    int httpResponse = http.POST(payload);

    if (httpResponse > 0) {
        String response = http.getString();
        Serial.println("Response: " + response);
        activateConfirmation();
    } else {
        Serial.println("Error on sending POST: " + String(httpResponse));
    }

    http.end();
}

int getDistance() {
    digitalWrite(triggPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggPin, LOW);

    long pulseDuration = pulseIn(echoPin, HIGH, 30000); // Added timeout fallback
    int distance = (pulseDuration * 0.0343) / 2;

    return distance;
}

void setup() {
    Serial.begin(115200); // Standard ESP32 baud rate

    pinMode(triggPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);

    connectWiFi();
}

void loop() {
    updateBlinkState(); // Continuously evaluate LED/Buzzer shutoff timer

    static unsigned long lastSendTime = 0;
    if (millis() - lastSendTime >= 5000) {
        lastSendTime = millis();
        int sensorDistance = getDistance();
        sendData(sensorDistance);
    }
}