#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Hardware Pin Configuration
constexpr uint8_t TRIG_PIN   = 26;
constexpr uint8_t ECHO_PIN   = 34; // Input-only pin (Requires 5V to 3.3V resistor divider)
constexpr uint8_t BUZZER_PIN = 25; // Passive Buzzer (PWM)
constexpr uint8_t LED_PIN    = 33; // Status LED

// LEDC PWM Channel Configuration (Arduino ESP32 Core v2.x)
constexpr uint8_t  BUZZER_CHANNEL    = 0;
constexpr uint32_t BUZZER_BASE_FREQ  = 2000; // 2 kHz base frequency
constexpr uint8_t  BUZZER_RESOLUTION = 8;    // 8-bit resolution (0-255)

// Network Configuration
const char* WIFI_SSID  = "BELL721";
const char* WIFI_PASS  = "766654CEA1A9";
const char* SERVER_URL = "http://192.168.2.78:3000/api/sensor"; // Node.js Host IP

// Global State & Non-blocking Timers
unsigned long lastSensorReadMs = 0;
unsigned long lastHttpSendMs   = 0;
unsigned long lastBlinkMs      = 0;

constexpr unsigned long SENSOR_INTERVAL_MS = 60;  // Poll ultrasonic sensor at ~16Hz
constexpr unsigned long HTTP_INTERVAL_MS   = 250; // Stream telemetry to Node.js at 4Hz

int currentDistanceCm = 300;
bool ledState = false;
int activeToneFreq = 0; // State tracking to avoid log spam

// Function Declarations
int readUltrasonicDistance();
void updateAlertFeedback(int distance);
void sendTelemetry(int distance);

void setup() {
    Serial.begin(115200);

    // Pin Modes
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    digitalWrite(TRIG_PIN, LOW);
    digitalWrite(LED_PIN, LOW);

    // Initialize LEDC PWM Peripheral (Core v2.x API)
    ledcSetup(BUZZER_CHANNEL, BUZZER_BASE_FREQ, BUZZER_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
    ledcWriteTone(BUZZER_CHANNEL, 0); // Start silenced

    // WiFi Setup
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\nWiFi Connected. Local IP: %s\n", WiFi.localIP().toString().c_str());
}

void loop() {

}

int readUltrasonicDistance() {

}

void updateAlertFeedback(int distance) {

}

void sendTelemetry(int distance) {
    if (WiFi.status() != WL_CONNECTED) return;

    WiFiClient client;
    HTTPClient http;
    http.begin(client, SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    // Stack-allocated JSON payload: {"distance": X}
    char jsonPayload[64];
    snprintf(jsonPayload, sizeof(jsonPayload), "{\"distance\":%d}", distance);

    int httpResponseCode = http.POST(reinterpret_cast<uint8_t*>(jsonPayload), strlen(jsonPayload));

    if (httpResponseCode > 0) {
        Serial.printf("Telemetry Posted: %d cm | Server Response: %d\n", distance, httpResponseCode);
    } else {
        Serial.printf("HTTP POST Failed: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
}