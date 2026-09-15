# ESP32_collision-_detector

### Phase 01 Completion Summary: Hardware Driver
**Components Used:**
- ESP32 Development Board
- HC-SR04 Ultrasonic Sensor
- Passive Buzzer

**Wiring and Pinout Configuration:**

**HC-SR04 Ultrasonic Sensor:**
- **VCC:** Connected to the 5V pin on the ESP32.
- **Trigger:** Connected to GPIO 12 on the ESP32.
- **Echo:** Connected to GPIO 35 (G35) on the ESP32 via a 5V to 3.3V voltage divider circuit.
- **GND:** Connected to the common ground (GND) on the ESP32.

**Passive Buzzer:**
- **Positive (+):** Connected to GPIO 12 on the ESP32 (shares the connection with the sensor's Trigger pin for synchronization).
- **Negative (-):** Connected to the common ground (GND) on the ESP32.

**Logic Level Consideration:**
The echo signal from the HC-SR04 operates at 5V logic. A voltage divider is implemented before connecting this signal to the ESP32's GPIO 35 (G35) to step it down to a safe 3.3V level, protecting the input pin.

**Functionality and Drivers:**
- Drivers have been developed to handle input from the HC-SR04, providing a basis for distance calculation.
- An LEDC PWM driver controls the buzzer, allowing it to generate audible tones based on input from the collision project's logic.
