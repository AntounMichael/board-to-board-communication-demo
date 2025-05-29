#pragma once

#include "Message.h"

const uint8_t MAX_POINTS = 20;

class DataMessage : public Message {
public:
    struct DataPoint {
        float timestamp;
        float value;
    };

    static constexpr uint8_t TYPE = 0x01;
    
    DataPoint data[MAX_POINTS];
    uint8_t numPoints = 0;

    uint8_t getType() const override { return TYPE; }

    void serialize(uint8_t* buffer, size_t& outLength) const override;
    bool deserialize(const uint8_t* buffer, size_t length) override;
};
