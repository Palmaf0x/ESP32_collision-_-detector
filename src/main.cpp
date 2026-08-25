#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ; // Wait for serial port connection
    }
    Serial.println("ESP32 Collision Detector Initialized.");
}

void loop() {
    // Phase 1 implementation will go here
}