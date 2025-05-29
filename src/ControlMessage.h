#pragma once

#include "Message.h"

class ControlMessage : public Message {
public:
    bool computationFailed = false;
    bool resetRequested = false;
    float controlValue = 0.0f;

    static constexpr size_t MAX_SERIALIZED_SIZE = 3 + sizeof(float) + 2; // 1 byte type, 1 byte computationFailed, 1 byte resetRequested, sizeof(float) controlValue, 2 bytes CRC

    void serialize(uint8_t* buffer, size_t& outLength) const override;
    bool deserialize(const uint8_t* buffer, size_t length) override;
    uint8_t getType() const override { return CONTROL_MESSAGE; }
};
