#include <Wire.h>
#include "Controller.h"

#define CONTROLLER_ADDR 0x09
#define MAX_DATA_MESSAGE_SIZE 164  // 1 byte type + 1 byte length + (20 points * 2 floats * 4 bytes) + 2 bytes CRC
#define MAX_CONTROL_MESSAGE_SIZE 9  // 1 byte type + 1 byte computationFailed + 1 byte resetRequested + 4 bytes controlValue + 2 bytes CRC

Controller controller;
volatile uint8_t receiveBuffer[MAX_DATA_MESSAGE_SIZE];
volatile uint8_t sendBuffer[MAX_CONTROL_MESSAGE_SIZE];
volatile bool hasValidResponse = false;  // Track if we have a valid response to send
volatile size_t receivedBytes = 0;  // Track how many bytes we received

void receiveEvent(int bytes) {
    Serial.print("Received "); Serial.print(bytes); Serial.println(" bytes");
    
    if (bytes > sizeof(receiveBuffer)) {
        Serial.println("Error: Message too large for buffer");
        return;  // Basic overflow protection
    }
    
    // Read the message
    int i = 0;
    while (Wire.available() && i < bytes) {
        receiveBuffer[i] = Wire.read();
        // Serial.print("Byte "); Serial.print(i); Serial.print(": 0x"); Serial.println(receiveBuffer[i], HEX);
        i++;
    }
    
    receivedBytes = i;
    Serial.print("Read "); Serial.print(i); Serial.println(" bytes");
    
    // Process the message
    if (receiveBuffer[0] == DATA_MESSAGE) {
        Serial.println("Processing data message");
        DataMessage msg;
        if (msg.deserialize((uint8_t*)receiveBuffer, receivedBytes)) {
            Serial.println("Successfully deserialized data message");
            controller.processDataMessage(msg);
            hasValidResponse = true;
        } else {
            Serial.println("Failed to deserialize data message");
            hasValidResponse = false;
        }
    } else {
        Serial.print("Unknown message type: 0x"); Serial.println(receiveBuffer[0], HEX);
        hasValidResponse = false;
    }
}

void requestEvent() {
    if (hasValidResponse) {
        ControlMessage response = controller.generateControlMessage();
        size_t length;
        response.serialize((uint8_t*)sendBuffer, length);
        Wire.write((uint8_t*)sendBuffer, length);
        Serial.println("Sent control response");
    } else {
        // Send default response if we don't have valid data
        ControlMessage defaultResponse;
        size_t length;
        defaultResponse.serialize((uint8_t*)sendBuffer, length);
        Wire.write((uint8_t*)sendBuffer, length);
        Serial.println("Sent default response");
    }
}

void setup() {
    Wire.begin(CONTROLLER_ADDR);  // Initialize as I2C follower
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("Controller initialized");
}

void loop() {
    // Blink LED to show we're alive
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);  // Slow blink to show we're waiting for messages
} 