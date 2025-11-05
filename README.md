<!---------------------------------------------------------------------------------------------------------->
<!---------------------------------------------------------------------------------------------------------->
# BLE ECU Datalogger (v3.0) for 🛩️ Mugin-6000
### Modern BLE + Web Dashboard for ECU Telemetry, Logging, and Real-Time Visualization.
The **DLA-232 aircraft engine**, powering the **Mugin-6000 VTOL UAV**, is a high-performance two-stroke gasoline engine designed for endurance and reliability. The **ECU Datalogger v3.0** enhances its operation by enabling **real-time telemetry, wireless data logging, and live visualization** through Bluetooth Low Energy (BLE) and a responsive web dashboard.

<!---------------------------------------------------------------------------------------------------------->
<!---------------------------------------------------------------------------------------------------------->
## 🚀 Overview
This version introduces a **fully redesigned software architecture** built on the same ESP32-based hardware. It replaces the previous HTTP + Node-RED system with a **BLE-based communication protocol** and a **modern Web Bluetooth Dashboard** for seamless, cable-free access to live and historical ECU data.
The new software provides:
- Real-time data streaming over BLE
- On-device SD card logging
- File management (download/delete) via BLE
- Interactive dashboard for parameter visualization and charting

<!---------------------------------------------------------------------------------------------------------->
<!---------------------------------------------------------------------------------------------------------->
## ⚙️ System Architecture
```
ECU → ESP32 (BLE Server + SD Logger) ↔ Web Dashboard (BLE Client)
```
- **ESP32:** Reads ECU data, logs it to SD card, and streams it via BLE.
- **Web Dashboard:** Connects directly to the ESP32 through the **Web Bluetooth API**, allowing users to view, record, and download ECU telemetry data wirelessly.
- 
<!---------------------------------------------------------------------------------------------------------->
<!---------------------------------------------------------------------------------------------------------->
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

<!---------------------------------------------------------------------------------------------------------->
<!---------------------------------------------------------------------------------------------------------->
## 🧩 Improvements Over v2.0
| **Feature** | **v2.0 (Old)** | **v3.0 (New)** |
|--------------|----------------|----------------|
| **Communication** | Wi-Fi (HTTP + Node-RED) | Bluetooth Low Energy (BLE) |
| **Real-time Display** | Node-RED dashboard | Web Bluetooth dashboard |
| **Connectivity** | Requires Wi-Fi connection | Works on mobile/PC with Chrome/Edge |
| **Data Access** | HTTP file read/download | BLE-based log management |
| **Responsiveness** | Dependent on network | Instant BLE response |
| **User Interface** | Static Node-RED UI | Interactive HTML + Chart.js UI |
| **Data Control** | Basic read/delete | Start/Stop/Delete/Download over BLE |
| **Efficiency** | Dual Serial (Wi-Fi load) | Optimized low-power BLE handling |

<!---------------------------------------------------------------------------------------------------------->
<!---------------------------------------------------------------------------------------------------------->
## 🧱 Block Diagram
![fig](https://github.com/syedmohiuddinzia/ble_ecu_datalogger/blob/main/v3.0/fig/BlockDiagramECU.png)

<!---------------------------------------------------------------------------------------------------------->
<!---------------------------------------------------------------------------------------------------------->
## 🧾 ECU Parameters Logged
| **Parameter** | **Description** |
|----------------|-----------------|
| **Seconds** | Time since logging started |
| **Injection Pulse Width 1 & 2 (µs)** | Fuel injector open duration |
| **RPM** | Engine speed (revolutions per minute) |
| **Ignition Advance Angle (°)** | Spark ignition timing before TDC |
| **Engine Status Flags** | Ready, Crank, Warmup, TPS modes |
| **Air-Fuel Ratio Target 1 & 2** | Desired air-fuel mixture ratios |
| **WBO2 Enable 1 & 2** | Oxygen sensor activity |
| **Barometric Pressure (kPa)** | Atmospheric pressure |
| **Manifold Absolute Pressure (kPa)** | Engine load indicator |
| **Manifold Air Temperature (°C)** | Intake air temperature |
| **Cylinder Temperature (°C)** | Engine temperature monitoring |
| **Throttle Position (%)** | Throttle opening percentage |
| **Battery Voltage (V)** | ECU power supply voltage |

<!---------------------------------------------------------------------------------------------------------->
<!---------------------------------------------------------------------------------------------------------->
## ⚡ Hardware Setup
- Microcontroller: ESP32 (Dual UART + BLE + SD)
- Interfaces:
  - Serial0 → Debugging
  - Serial2 → ECU data
- Storage: microSD card (FAT32)
- Indicators:
  - Wi-Fi/BLE connection
  - Data reception
  - Logging status

 <!---------------------------------------------------------------------------------------------------------->
<!---------------------------------------------------------------------------------------------------------->
## 💻 Software Components
| **Component** | **Description** |
|----------------|-----------------|
| **ESP32 Firmware** | Reads ECU data, manages SD logs, and serves BLE services. |
| **Web Bluetooth App** | Connects to ESP32, visualizes real-time data, and manages logs. |
| **Python Utilities** | CSV decryption and plotting tools for post-flight analysis. |

<!---------------------------------------------------------------------------------------------------------->
<!---------------------------------------------------------------------------------------------------------->
## 📂 Repository Structure
```bash
/v3.0
│
├── /firmware
│   └── ECU_BLE_LOGGER.ino       # ESP32 firmware
│
├── /webapp
│   └── app.html                 # Web Bluetooth dashboard
│
├── /docs
│   ├── BlockDiagram.pptx              # Block Diagram Powerpoint
│   └── CommunicationECU.doc           # ECU Communication Manual
│
└── /fig
    └── BlockDiagramECU.png            # Block Diagram Image
```

<!---------------------------------------------------------------------------------------------------------->
<!---------------------------------------------------------------------------------------------------------->
🧩 How to Use
**1. Flash ESP32 Firmware**
  - Upload ```ECU_BLE_LOGGER.ino``` using Arduino IDE.
**2. Prepare SD Card**
  - Format as FAT32 and insert into the ESP32 module.
**3. Run the Web App**
  - Open ```index.html``` in **Chrome** or **Edge** (supports Web Bluetooth).
  - Click **Connect** → Select your **ECU** device.
**4. Monitor Data**
  - View real-time ECU telemetry.
  - Start/Stop logging as needed.
  - Download or delete logs via the dashboard.

<!---------------------------------------------------------------------------------------------------------->
<!---------------------------------------------------------------------------------------------------------->
## 🧪 Tools & Technologies
- ESP32 (Arduino Framework)
- Bluetooth Low Energy (BLE GATT)
- Chart.js
- Web Bluetooth API
- HTML5 + CSS3
- Python (Data Processing)

<!---------------------------------------------------------------------------------------------------------->
<!---------------------------------------------------------------------------------------------------------->
## 📈 Example Use Cases
- UAV engine diagnostics and health monitoring
- ECU performance analysis and tuning
- Fuel injection optimization
- Sensor validation and test bench setups

<!---------------------------------------------------------------------------------------------------------->
<!---------------------------------------------------------------------------------------------------------->
## 🧾 License
- MIT License © 2025 — Syed Mohiuddin Zia
