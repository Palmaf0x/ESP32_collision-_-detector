# ESP32 Car Collision Detector

An IoT collision detection system that measures real-time distance using an HC-SR04 ultrasonic sensor, provides dynamic visual (LED) and audio (buzzer) alerts based on threat proximity, and streams telemetry over HTTP to a Node.js backend.

---

## 📌 Hardware Pinout

| Component | ESP32 GPIO Pin | Mode | Description |
| :--- | :--- | :--- | :--- |
| **HC-SR04 Trigger (`triggPin`)** | **GPIO 26** | `OUTPUT` | Sends pulse trigger signal |
| **HC-SR04 Echo (`echoPin`)** | **GPIO 27** | `INPUT` | Receives echo return signal |
| **Status LED (`ledPin`)** | **GPIO 32** | `OUTPUT` | Visual warning indicator |
| **Passive Buzzer (`buzzerPin`)** | **GPIO 33** | `OUTPUT` | PWM tone alert generator |

> **Note:** Ensure all components (sensor, LED, buzzer) share a common ground (`GND`) rail connected to one of the ESP32 `GND` pins. Do not connect ground wires to flash strapping pins such as `CMD` or `GPIO 12`.

---

## 🖥️ Node.js Server & Telemetry API

The Node.js server acts as an intermediate ingestion point for telemetry and serves data to web dashboards.

### API Endpoints

* **`POST /api/sensor`**
    * **Description:** Ingests distance readings from the ESP32.
    * **Payload:** `{"Distance": 45}`
    * **Response:** `200 OK` with confirmation message.
* **`GET /api/distance`**
    * **Description:** Exposes the latest distance reading to external web applications.
    * **Response:** `{"distance": 45}`
    * **CORS:** Enabled (`Access-Control-Allow-Origin: *`) for browser fetch requests.

---

## ⚙️ How to Adapt and Run

To deploy this project on your own local network, you must update the Wi-Fi credentials and target server IP address in both the ESP32 source code and Node.js environment.

### 1. Find Your Local IPv4 Address
Open a terminal on the machine hosting the Node.js server and retrieve your local IP address:

* **Windows (PowerShell / CMD):**
  ```cmd
  ipconfig