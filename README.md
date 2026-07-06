# IoT Gateway on a Raspberry Pi & Domotic Control Central

[![Platform](https://img.shields.io/badge/Platform-Raspberry%20Pi%20%7C%20ESP8266-red.svg)](https://www.raspberrypi.org/)
[![Protocols](https://img.shields.io/badge/Protocols-MQTT%20%7C%20HTTP-blue.svg)](https://mqtt.org/)
[![Software](https://img.shields.io/badge/Software-Node--RED%20%7C%20Mosquitto-green.svg)](https://nodered.org/)

This repository implements a Proof of Concept (PoC) for a local, scalable, and responsive **IoT Gateway & Domotic Control Central** built upon a **Raspberry Pi Zero W** and **ESP8266 Microcontrollers**. 

It analyzes the communication architecture between transducers (sensors & actuators) and user interfaces (UI) using the lightweight **MQTT** messaging protocol.

---

## 📖 Project Documentation

We have built a premium, interactive, and bilingual documentation hub containing all design schematics, academic reports, and setup guides.

👉 **[Open Local Interactive Documentation](docs/index.html)** *(Open in any Web Browser)*

### Key Documentation Features:
*   **Bilingual Content**: Instant toggle between English and Spanish translations.
*   **Dark/Light Themes**: Smooth transition between slate-dark and clean-light modes.
*   **Academic vs. PoC Perspectives**: Switch between the simplified DHT11 PoC (Blog Article) and the broader multi-node Ethernet/WiFi DHT22/LDR system (Academic Report).
*   **Firmware Center**: Interactive side-by-side C++ code viewers with click-to-copy functionality.
*   **Diagram Gallery**: High-resolution lightbox modals for all schematics and flowcharts.

*The original published blog post can be found online at [Jorge Nicolau's WordPress Blog](https://jorgenicolauar.wordpress.com/2020/05/23/iot-gateway-on-a-pi/).*

---

## 🛠️ System Architecture & Components

The system coordinates edge nodes linked over Wi-Fi and Ethernet to a central Gateway coordinating automations and rendering dashboards.

### 1. Gateway & Control Panel (Raspberry Pi Zero W)
*   **Mosquitto MQTT Broker**: Local message router handling publish/subscribe actions.
*   **Node-RED**: Automations rule engine (e.g. triggering humidifier actuators based on DHT telemetry thresholds) and responsive Web Dashboard generator.

### 2. Wireless Edge Nodes (ESP8266 NodeMCU)
*   **Sensors**: DHT11/DHT22 (Temperature & Humidity) or LDR Photoelectric modules.
*   **Actuators**: Relays (e.g., SONGLE 5V relays) or Red LED simulation models.
*   **Firmware**: C++ clients utilizing `PubSubClient.h` and `ESP8266WiFi.h` libraries.

---

## 📁 Repository Structure

```bash
├── docs/                      # Interactive documentation folder
│   ├── index.html             # Main documentation entry point
│   ├── style.css              # External stylesheet
│   └── images/                # Visual schematics & screenshots
│
├── presentacion/              # Academic presentation materials
│   └── Trabajo Final Integrador/
│       ├── TFI Jorge Enrique Nicolau R01 2019.pdf   # Academic thesis report
│       └── _01.temp_sensor_01.ino                  # ESP8266 + DHT22 firmware
│
├── iotdevice.ino              # ESP8266 + DHT11 + LED PoC firmware sketch
├── LICENSE                    # MIT License
└── README.md                  # Project overview (this file)
```

---

## 🚀 Quick Start & Testing

1.  **Deploy MQTT Broker**: Install Mosquitto on your Raspberry Pi and start the service.
2.  **Verify Broker Activity**:
    ```bash
    # Subscribe to test topic
    mosquitto_sub -d -t hello_topic
    
    # Publish from another session
    mosquitto_pub -d -t hello_topic -m "hello from RaspberryPi"
    ```
3.  **Flash ESP8266**: Open `iotdevice.ino` or `_01.temp_sensor_01.ino` in Arduino IDE, adjust Wi-Fi credentials (`WIFISSID`/`WIFIPASS`) and broker IP (`IOTGATEWAY`), and flash to the board.
4.  **Launch Node-RED**: Import the flow configuration and access the visual dashboard at `http://<pi_ip>:1880/ui`.
