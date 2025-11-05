# BLE ECU Datalogger (v3.0) for 🛩️ Mugin-6000
### Modern BLE + Web Dashboard for ECU Telemetry, Logging, and Real-Time Visualization.
The **DLA-232 aircraft engine**, powering the **Mugin-6000 VTOL UAV**, is a high-performance two-stroke gasoline engine designed for endurance and reliability. The **ECU Datalogger v3.0** enhances its operation by enabling **real-time telemetry, wireless data logging, and live visualization** through Bluetooth Low Energy (BLE) and a responsive web dashboard.

## 🚀 Overview
This version introduces a **fully redesigned software architecture** built on the same ESP32-based hardware. It replaces the previous HTTP + Node-RED system with a **BLE-based communication protocol** and a **modern Web Bluetooth Dashboard** for seamless, cable-free access to live and historical ECU data.

The new software provides:
- Real-time data streaming over BLE
- On-device SD card logging
- File management (download/delete) via BLE
- Interactive dashboard for parameter visualization and charting

## ⚙️ System Architecture
```
ECU → ESP32 (BLE Server + SD Logger) ↔ Web Dashboard (BLE Client)
```
- **ESP32:** Reads ECU data, logs it to SD card, and streams it via BLE.
- **Web Dashboard:** Connects directly to the ESP32 through the **Web Bluetooth API**, allowing users to view, record, and download ECU telemetry data wirelessly.

## 🧠 Key Features
- ESP32 Firmware
  - Custom **BLE GATT** Service for ECU data communication.
  - Real-time streaming of ECU parameters (RPM, temperature, throttle, etc.).
  - SD card integration for offline log storage.
  - BLE commands for:
    - Start / Stop logging
    - Download log file
    - Delete stored data
  - LED indicators for **connection**, **data reception**, and **logging status**.
  - Efficient binary-to-string conversion and checksum handling.
- Web Bluetooth Dashboard
  - Connects directly via **BLE** — no Wi-Fi or Node-RED required.
  - Displays live **ECU parameters** in a clean, responsive interface.
  - “Quick Charts” section shows mini visualizations of key parameters.
  - Click on any parameter to open a **detailed live graph**.
  - Supports **CSV export** of logged data.
  - Displays system status (connected, logging, SD status) in real-time.
