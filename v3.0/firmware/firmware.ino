#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// ================== PINS ==================
#define NET_LED 15
#define REC_LED 2
#define MMC_LED 4

// ================== BLE UUIDs ==================
#define SERVICE_UUID                "19b10000-e8f2-537e-4f6c-d104768a1214"
#define ECU_CHARACTERISTIC_UUID     "19b10001-e8f2-537e-4f6c-d104768a1214"
#define CONTROL_CHARACTERISTIC_UUID "19b10002-e8f2-537e-4f6c-d104768a1214"
#define STATUS_CHARACTERISTIC_UUID  "19b10003-e8f2-537e-4f6c-d104768a1214"
#define FILE_CHARACTERISTIC_UUID    "19b10004-e8f2-537e-4f6c-d104768a1214"

// ================== VARIABLES ==================
BLEServer* pServer = NULL;
BLECharacteristic* pECUCharacteristic = NULL;
BLECharacteristic* pControlCharacteristic = NULL;
BLECharacteristic* pStatusCharacteristic = NULL;
BLECharacteristic* pFileCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Logging control
bool loggingEnabled = true;
bool sdCardAvailable = false;

String packet;
unsigned long previousMillis = 0;
const long interval = 200;  // ms

// File transfer control
bool fileTransferInProgress = false;
unsigned long fileTransferStartTime = 0;
const unsigned long FILE_TRANSFER_TIMEOUT = 30000; // 30 seconds

// ================== SD Logging ==================
void appendFile(fs::FS &fs, const char *path, const char *message) {
  if (!loggingEnabled || !sdCardAvailable) return;
  
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    sdCardAvailable = false;
    updateStatusCharacteristic();
    return;
  }
  if (file.print(message)) {
    digitalWrite(MMC_LED, HIGH);
    delay(5);
    digitalWrite(MMC_LED, LOW);
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

String readFile(fs::FS &fs, const char *path) {
  File file = fs.open(path);
  if (!file) {
    return "";
  }
  String content = "";
  while (file.available()) {
    content += (char)file.read();
  }
  file.close();
  return content;
}

bool deleteFile(fs::FS &fs, const char *path) {
  Serial.printf("Deleting file: %s\n", path);
  bool result = fs.remove(path);
  updateStatusCharacteristic();
  return result;
}

// ================== Hex Conversion Functions ==================
String asciiHexToBinaryHex(String asciiHex) {
  String binaryHex = "";
  String temp = "";
  
  for (int i = 0; i < asciiHex.length(); i++) {
    char c = asciiHex[i];
    
    // Skip spaces
    if (c == ' ') continue;
    
    temp += c;
    
    // Every 2 ASCII characters = 1 byte
    if (temp.length() == 2) {
      binaryHex += temp + " ";
      temp = "";
    }
  }
  
  return binaryHex;
}

// ================== File Transfer ==================
void sendFileViaBLE() {
  if (!sdCardAvailable || fileTransferInProgress) return;
  
  fileTransferInProgress = true;
  fileTransferStartTime = millis();
  
  String fileContent = readFile(SD, "/ecu_data.txt");
  if (fileContent.length() > 0) {
    Serial.println("Starting file transfer via BLE...");
    
    // Send file content in chunks
    int chunkSize = 400; // Reduced for BLE MTU
    int totalChunks = (fileContent.length() + chunkSize - 1) / chunkSize;
    
    for (int i = 0; i < totalChunks; i++) {
      if (!fileTransferInProgress) break; // Allow cancellation
      
      int start = i * chunkSize;
      int end = start + chunkSize;
      if (end > fileContent.length()) {
        end = fileContent.length();
      }
      
      String chunk = fileContent.substring(start, end);
      
      pFileCharacteristic->setValue(chunk.c_str());
      pFileCharacteristic->notify();
      
      delay(50); // Small delay between chunks
      
      // Check for timeout
      if (millis() - fileTransferStartTime > FILE_TRANSFER_TIMEOUT) {
        Serial.println("File transfer timeout");
        break;
      }
    }
    
    // Send end marker
    pFileCharacteristic->setValue("FILE_END");
    pFileCharacteristic->notify();
    
    Serial.println("File transfer completed");
  } else {
    // Send empty file marker
    pFileCharacteristic->setValue("FILE_EMPTY");
    pFileCharacteristic->notify();
  }
  
  fileTransferInProgress = false;
}

// ================== Status Updates ==================
void updateStatusCharacteristic() {
  if (deviceConnected && pStatusCharacteristic) {
    String status = "LOG:" + String(loggingEnabled ? "1" : "0") + 
                   ",SD:" + String(sdCardAvailable ? "1" : "0");
    pStatusCharacteristic->setValue(status.c_str());
    pStatusCharacteristic->notify();
  }
}

// ================== BLE Callbacks ==================
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    digitalWrite(NET_LED, HIGH);
    Serial.println("Device connected");
    updateStatusCharacteristic();
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    digitalWrite(NET_LED, LOW);
    fileTransferInProgress = false; // Cancel any file transfer
    Serial.println("Device disconnected, restarting advertising...");
    pServer->startAdvertising();
  }
};

class ControlCharacteristicCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();
    if (value.length() > 0) {
      Serial.println("BLE Control Command: " + value);
      
      if (value == "START") {
        loggingEnabled = true;
        Serial.println("Logging STARTED via BLE");
      } else if (value == "STOP") {
        loggingEnabled = false;
        Serial.println("Logging STOPPED via BLE");
      } else if (value == "DELETE") {
        if (deleteFile(SD, "/ecu_data.txt")) {
          Serial.println("File deleted via BLE");
        } else {
          Serial.println("Delete failed via BLE");
        }
      } else if (value == "STATUS") {
        updateStatusCharacteristic();
      } else if (value == "DOWNLOAD") {
        sendFileViaBLE();
      }
      updateStatusCharacteristic();
    }
  }
};

// ================== ECU Data ==================
void ECUdata() {
  digitalWrite(REC_LED, LOW);

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Request ECU data - uncomment if needed
    // Serial2.write("a"); 

    String receivedData = "";
    while (Serial2.available()) {
      char c = Serial2.read();
      receivedData += c;
    }

    if (receivedData.length() > 0) {
      // Check if data is in ASCII hex format (like from Node-RED)
      // If it contains spaces and hex characters, convert it
      if (receivedData.indexOf(' ') != -1) {
        // Data is in ASCII hex format "3E 00 7D 11..."
        packet = asciiHexToBinaryHex(receivedData);
        Serial.println("Converted ECU Response: " + packet);
      } else {
        // Data is already in binary format
        String hexData = "";
        for (int i = 0; i < receivedData.length(); i++) {
          byte data = receivedData[i];
          hexData += (data < 0x10 ? "0" : "") + String(data, HEX) + " ";
        }
        packet = hexData;
        Serial.println("Binary ECU Response: " + packet);
      }
      
      // Log to SD card if enabled (without timestamp)
      if (loggingEnabled) {
        appendFile(SD, "/ecu_data.txt", packet.c_str());
      }

      // Send via BLE if connected (live data only)
      if (deviceConnected) {
        pECUCharacteristic->setValue(packet.c_str());
        pECUCharacteristic->notify();
      }
    }
  }

  digitalWrite(REC_LED, HIGH);
}

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);

  pinMode(NET_LED, OUTPUT);
  pinMode(REC_LED, OUTPUT);
  pinMode(MMC_LED, OUTPUT);

  // Initialize SD Card
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    sdCardAvailable = false;
  } else {
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
      Serial.println("No SD card attached");
      sdCardAvailable = false;
    } else {
      Serial.println("SD card initialized");
      sdCardAvailable = true;
    }
  }

  // BLE Initialization
  BLEDevice::init("ECU");

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  // ECU Data Characteristic (Live data only)
  pECUCharacteristic = pService->createCharacteristic(
                         ECU_CHARACTERISTIC_UUID,
                         BLECharacteristic::PROPERTY_READ |
                         BLECharacteristic::PROPERTY_NOTIFY
                       );
  pECUCharacteristic->addDescriptor(new BLE2902());

  // Control Characteristic
  pControlCharacteristic = pService->createCharacteristic(
                             CONTROL_CHARACTERISTIC_UUID,
                             BLECharacteristic::PROPERTY_WRITE
                           );
  pControlCharacteristic->setCallbacks(new ControlCharacteristicCallbacks());

  // Status Characteristic
  pStatusCharacteristic = pService->createCharacteristic(
                            STATUS_CHARACTERISTIC_UUID,
                            BLECharacteristic::PROPERTY_READ |
                            BLECharacteristic::PROPERTY_NOTIFY
                          );
  pStatusCharacteristic->addDescriptor(new BLE2902());

  // File Data Characteristic (for downloads)
  pFileCharacteristic = pService->createCharacteristic(
                          FILE_CHARACTERISTIC_UUID,
                          BLECharacteristic::PROPERTY_READ |
                          BLECharacteristic::PROPERTY_NOTIFY
                        );
  pFileCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x00);
  BLEDevice::startAdvertising();

  Serial.println("BLE Advertising started as ECU_BLE_Logger...");
  updateStatusCharacteristic();
}

// ================== LOOP ==================
void loop() {
  ECUdata();

  // Handle BLE reconnect logic
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);
    pServer->startAdvertising();
    Serial.println("Restart advertising");
    oldDeviceConnected = deviceConnected;
  }

  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
}
