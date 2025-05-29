#pragma once

#include <Arduino.h>

class Message {
public:
    virtual void serialize(uint8_t* buffer, size_t& length) const = 0;
    virtual bool deserialize(const uint8_t* buffer, size_t length) = 0;
    virtual uint8_t getType() const = 0;
};

enum MessageType : uint8_t {
    DATA_MESSAGE = 1,
    CONTROL_MESSAGE = 2
};
