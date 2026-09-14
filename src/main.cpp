#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Hardware Pin Configuration
constexpr uint8_t TRIG_PIN   = 26;
constexpr uint8_t ECHO_PIN   = 34; // Input-only pin (Requires 5V to 3.3V resistor divider)
constexpr uint8_t BUZZER_PIN = 25; // Passive Buzzer (PWM)
constexpr uint8_t LED_PIN    = 33; // Status LED

// Network Configuration
const char* WIFI_SSID  = "BELL721";
const char* WIFI_PASS  = "766654CEA1A9";
const char* SERVER_URL = "http://192.168.2.1:3000/api/sensor"; // Replace with your Node.js Host IP

// Global State & Non-blocking Timers
unsigned long lastSensorReadMs = 0;
unsigned long lastHttpSendMs   = 0;
unsigned long lastBlinkMs      = 0;

constexpr unsigned long SENSOR_INTERVAL_MS = 60;  // Poll ultrasonic sensor at ~16Hz
constexpr unsigned long HTTP_INTERVAL_MS   = 250; // Stream telemetry to Node.js at 4Hz

int currentDistanceCm = 300;
bool ledState = false;

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
    pinMode(BUZZER_PIN, OUTPUT);

    digitalWrite(TRIG_PIN, LOW);
    digitalWrite(LED_PIN, LOW);

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
    unsigned long currentMs = millis();

    // 1. Non-blocking Ultrasonic Sensor Polling
    if (currentMs - lastSensorReadMs >= SENSOR_INTERVAL_MS) {
        lastSensorReadMs = currentMs;
        currentDistanceCm = readUltrasonicDistance();
    }

    // 2. Non-blocking Dynamic LED and Buzzer Modulation
    updateAlertFeedback(currentDistanceCm);

    // 3. Non-blocking HTTP Telemetry Transmission
    if (currentMs - lastHttpSendMs >= HTTP_INTERVAL_MS) {
        lastHttpSendMs = currentMs;
        sendTelemetry(currentDistanceCm);
    }
}

int readUltrasonicDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Read pulse width with a 30ms timeout (~5m max measurement range)
    unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);

    if (duration == 0) {
        return 300; // Out of range or timeout fallback
    }

    // Distance calculation: (duration_us * 0.0343 cm/us) / 2
    int distance = duration * 0.0343 / 2;
    return constrain(distance, 2, 300);
}

void updateAlertFeedback(int distance) {
    unsigned long currentMs = millis();

    // SAFE ZONE (> 150 cm): Silence buzzer and turn off LED
    if (distance > 150) {
        digitalWrite(LED_PIN, LOW);
        noTone(BUZZER_PIN);
        ledState = false;
        return;
    }

    // CRITICAL ZONE (<= 15 cm): Solid LED and continuous high-pitch alert tone
    if (distance <= 15) {
        digitalWrite(LED_PIN, HIGH);
        tone(BUZZER_PIN, 2500); // Continuous 2.5 kHz frequency
        return;
    }

    // WARNING ZONE (15 cm < distance <= 150 cm): Dynamic pulsing
    // Map distance to blink/beep interval (50ms fast pulse near 15cm; 600ms slow pulse near 150cm)
    int blinkInterval = map(distance, 15, 150, 50, 600);

    if (currentMs - lastBlinkMs >= static_cast<unsigned long>(blinkInterval)) {
        lastBlinkMs = currentMs;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);

        if (ledState) {
            // Tone frequency increases dynamically as proximity decreases
            int toneFreq = map(distance, 15, 150, 2200, 800);
            tone(BUZZER_PIN, toneFreq);
        } else {
            noTone(BUZZER_PIN);
        }
    }
}

void sendTelemetry(int distance) {
    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    // Stack-allocated JSON buffer
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