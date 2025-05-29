#include "ControlMessage.h"
#include <cstring>

// CRC-16-CCITT implementation
uint16_t calculateCRC16(const uint8_t* data, size_t length) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

void ControlMessage::serialize(uint8_t* buffer, size_t& outLength) const {
    buffer[0] = getType();
    buffer[1] = computationFailed ? 1 : 0;
    buffer[2] = resetRequested ? 1 : 0;
    std::memcpy(&buffer[3], &controlValue, sizeof(float));
    size_t dataLength = 3 + sizeof(float);
    uint16_t crc = calculateCRC16(buffer, dataLength);
    std::memcpy(&buffer[dataLength], &crc, sizeof(uint16_t));
    outLength = dataLength + sizeof(uint16_t);
}

bool ControlMessage::deserialize(const uint8_t* buffer, size_t length) {
    // Check minimum length (type + flags + float + CRC)
    if (length < 3 + sizeof(float) + sizeof(uint16_t)) {
        Serial.println("ControlMessage: Message too short");
        return false;
    }

    if (buffer[0] != getType()) {
        Serial.println("ControlMessage: Invalid message type");
        return false;
    }
    
    computationFailed = buffer[1];
    resetRequested = buffer[2];
    std::memcpy(&controlValue, &buffer[3], sizeof(float));
    size_t dataLength = 3 + sizeof(float);
    uint16_t receivedCRC;
    std::memcpy(&receivedCRC, &buffer[dataLength], sizeof(uint16_t));
    uint16_t calculatedCRC = calculateCRC16(buffer, dataLength);
    
    if (receivedCRC != calculatedCRC) {
        Serial.println("ControlMessage: CRC mismatch");
        computationFailed = true;
        resetRequested = false;
        controlValue = 0.0f;
        return false;
    }
    
    Serial.print("ControlMessage received - ");
    Serial.print("Value: "); Serial.print(controlValue);
    Serial.print(", Failed: "); Serial.print(computationFailed);
    Serial.print(", Reset: "); Serial.println(resetRequested);
    return true;
}
