#include "DataMessage.h"
#include <cstring>

void DataMessage::serialize(uint8_t* buffer, size_t& outLength) const {
    buffer[0] = getType();

    // Calculate payload length (number of data bytes after header)
    uint8_t payloadLength = numPoints * 2 * sizeof(float);
    buffer[1] = payloadLength;

    size_t index = 2;
    for (uint8_t i = 0; i < numPoints; i++) {
        std::memcpy(&buffer[index], &data[i].timestamp, sizeof(float));
        index += sizeof(float);
        std::memcpy(&buffer[index], &data[i].value, sizeof(float));
        index += sizeof(float);
    }
    outLength = 2 + payloadLength;
    
    Serial.print("DataMessage sending - ");
    Serial.print("Points: "); Serial.print(numPoints);
    Serial.print(", Last timestamp: "); Serial.print(data[numPoints-1].timestamp);
    Serial.print(", Last value: "); Serial.println(data[numPoints-1].value);
}

bool DataMessage::deserialize(const uint8_t* buffer, size_t length) {
    // Check minimum length (type + length byte)
    if (length < 2) {
        Serial.println("DataMessage: Message too short");
        return false;
    }

    if (buffer[0] != getType()) {
        Serial.println("DataMessage: Invalid message type");
        return false;
    }

    uint8_t payloadLength = buffer[1];
    numPoints = payloadLength / (2 * sizeof(float));

    // Validate total message length
    if (length != 2 + payloadLength) {
        Serial.println("DataMessage: Invalid message length");
        return false;
    }

    // Validate number of points
    if (numPoints > MAX_POINTS) {
        Serial.println("DataMessage: Too many points");
        return false;
    }

    size_t index = 2;
    for (uint8_t i = 0; i < numPoints; i++) {
        std::memcpy(&data[i].timestamp, &buffer[index], sizeof(float));
        index += sizeof(float);
        std::memcpy(&data[i].value, &buffer[index], sizeof(float));
        index += sizeof(float);
    }

    Serial.print("DataMessage received - ");
    Serial.print("Points: "); Serial.print(numPoints);
    Serial.print(", Last timestamp: "); Serial.print(data[numPoints-1].timestamp);
    Serial.print(", Last value: "); Serial.println(data[numPoints-1].value);
    return true;
}

