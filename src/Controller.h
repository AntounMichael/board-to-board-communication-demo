#pragma once

#include "DataMessage.h"
#include "ControlMessage.h"
#include "PIDController.h"

class Controller {
public:
    PIDController pid;
    float threshold = 1000.0f;
    float p_computationError = 0.05f;
    unsigned long lastTimestamp = 0;  // Track last valid timestamp
    bool hasSeenTimestamp = false;    // Track if we've seen any timestamps yet

    Controller() {
        pid.target = 10.0f;  // Set initial target
    }

    void processDataMessage(const DataMessage& msg);
    ControlMessage generateControlMessage();
};
