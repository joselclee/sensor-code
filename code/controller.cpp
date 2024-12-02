#include <LoRaWan_APP.h> // Heltec's LoRaWAN library
#include "Arduino.h"

// LoRaWAN credentials - Need to replace after getting new
uint8_t DevEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t AppEui[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x00, 0x00, 0x00 };
uint8_t AppKey[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x1F, 0xF1, 0x60, 0x94, 0x56, 0xC5 };

uint8_t nwkSKey[] = {};
uint8_t appSKey[] = {};
uint32_t devAddr = (uint32_t)0x00000000;

// Replace later after talking to professor on how to connect to gateway
LoRaMacRegion_t loraWanRegion = LORAMAC_REGION_AS923;

// Select activation method: true for OTAA, false for ABP
bool overTheAirActivation = true;
bool loraWanAdr = true;

DeviceClass_t loraWanClass = CLASS_A;
uint32_t appTxDutyCycle = 8640000; // Transmit once a day to save power
bool sendConfirmed = false;
uint8_t appPort = 2;

// Variables to store sensor data
float temperature = 25.0;
float soilMoisture = 60.5;
float soilPH;
float 

// Buffer to store the payload
uint8_t appData[LORAWAN_APP_DATA_MAX_SIZE];
uint8_t appDataSize;

// Function prototypes
void prepareTxFrame(uint8_t port);
void sendSensorData(); // Need to develop once we have a clearer hardware prototype

void setup() {
  boardInitMcu();
  Serial.begin(9600);
  LoRaWAN.init(loraWanClass, loraWanRegion);
  deviceState = DEVICE_STATE_JOIN;
}

void loop() {
  switch (deviceState) {
    case DEVICE_STATE_JOIN:
      if (overTheAirActivation) {
        Serial.println("Joining via OTAA...");
        LoRaWAN.join();
      } else {
        Serial.println("Joining via ABP...");
        LoRaWAN.setDevAddr(devAddr);
        LoRaWAN.setNwkSKey(nwkSKey);
        LoRaWAN.setAppSKey(appSKey);
        deviceState = DEVICE_STATE_SEND;
      }
      break;

    case DEVICE_STATE_SEND:
      Serial.println("Sending data...");
      prepareTxFrame(appPort);
      LoRaWAN.send();
      deviceState = DEVICE_STATE_CYCLE;
      break;

    case DEVICE_STATE_CYCLE:
      LoRaWAN.cycle(appTxDutyCycle);
      deviceState = DEVICE_STATE_SLEEP;
      break;

    case DEVICE_STATE_SLEEP:
      LoRaWAN.sleep();
      break;

    default:
      deviceState = DEVICE_STATE_INIT;
      break;
  }
}

// Prepare the payload to be sent
void prepareTxFrame(uint8_t port) {
  // Encode sensor data
  appDataSize = 4;
  appData[0] = (uint8_t)(temperature * 10); // Scaled by 10
  appData[1] = (uint8_t)(soilMoisture);    // As integer value
  appData[2] = 0x00; // Reserved for future use
  appData[3] = 0x00; // Reserved for future use

  Serial.print("Payload: ");
  for (int i = 0; i < appDataSize; i++) {
    Serial.print(appData[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}
