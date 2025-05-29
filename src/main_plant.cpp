#include <Wire.h>
#include "Plant.h"

#define CONTROLLER_ADDR 0x09
#define SEND_INTERVAL_MIN 3
#define SEND_INTERVAL_MAX 5
#define MAX_MESSAGE_SIZE 164  // 1 byte type + 1 byte length + (20 points * 2 floats * 4 bytes) + 2 bytes CRC
#define CONTROL_MESSAGE_SIZE 9  // 1 byte type + 1 byte computationFailed + 1 byte resetRequested + 4 bytes controlValue + 2 bytes CRC
#define REQUEST_TIMEOUT 500  // Timeout in milliseconds for waiting for response

Plant plant;
int updateCounter = 0;
int sendCounter = 0;
int sendInterval = SEND_INTERVAL_MIN;

void setup() {
    Wire.begin();  // Initialize as I2C leader
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    delay(15000); // so i have time to set up monitor
    Serial.println("Plant initialized");
}

void loop() {
    // Update simulation
    plant.updateSimulation();
    updateCounter++;
    sendCounter++;
    
    // Log state every 10 updates
    if (updateCounter >= 10) {
        Serial.print("Plant state - ");
        Serial.print("Position: "); Serial.print(plant.state.position);
        Serial.print(", Velocity: "); Serial.print(plant.state.velocity);
        Serial.print(", Input: "); Serial.println(plant.input);
        updateCounter = 0;
    }
    
    // Check if it's time to send data
    if (sendCounter >= sendInterval) {
        Serial.print("Sending data (interval: "); Serial.print(sendInterval); Serial.println(")");
        
        // Send data to controller
        uint8_t buffer[MAX_MESSAGE_SIZE];
        size_t length;
        plant.dataBuffer.serialize(buffer, length);
        
        Wire.beginTransmission(CONTROLLER_ADDR);
        Wire.write(buffer, length);
        Wire.endTransmission();
        Serial.println("Sent data message");
        
        // Clear buffer after sending
        plant.dataBuffer.numPoints = 0;
        
        // Small delay to allow I2C bus to settle
        delay(25);
        
        // Request response from controller
        Serial.println("Requesting response from controller");
        Wire.requestFrom(CONTROLLER_ADDR, CONTROL_MESSAGE_SIZE);
        Serial.println("Requested response from controller");
        
        // Wait for response with timeout
        unsigned long startTime = millis();
        while (Wire.available() < CONTROL_MESSAGE_SIZE && (millis() - startTime) < REQUEST_TIMEOUT) {
            delay(10);
            Serial.println("Waiting for response from controller");
        }
        Serial.println("Checked for response from controller");
        Serial.print("Available bytes: "); Serial.println(Wire.available());
        if (Wire.available() >= CONTROL_MESSAGE_SIZE) {
            Serial.println("Received response from controller");
            // Read response into buffer
            uint8_t response[CONTROL_MESSAGE_SIZE];
            for (int i = 0; i < CONTROL_MESSAGE_SIZE; i++) {
                response[i] = Wire.read();
            }
            
            // Process the response
            ControlMessage controlMsg;
            if (controlMsg.deserialize(response, CONTROL_MESSAGE_SIZE)) {
                plant.handleControlMessage(controlMsg);
                Serial.print("Received control value: "); Serial.println(controlMsg.controlValue);
                if (controlMsg.resetRequested) {
                    Serial.println("Reset requested by controller!");
                }
            } else {
                Serial.println("Failed to deserialize control message");
            }
        } else {
            Serial.println("Timeout waiting for controller response");
        }
        
        // Reset counter and set new random interval
        sendCounter = 0;
        sendInterval = random(SEND_INTERVAL_MIN, SEND_INTERVAL_MAX + 1);
        
        // Blink LED to indicate transmission
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1);
        digitalWrite(LED_BUILTIN, LOW);
    }
    
    // Small delay to maintain consistent timing
    delay(20);  // 50Hz update rate
} 