#include "Controller.h"
#ifdef abs
#undef abs
#endif
#include <random>
#include <cmath>

void Controller::processDataMessage(const DataMessage& msg) {
    if (msg.numPoints == 0) return;

    // Process all points to update integral term
    for (uint8_t i = 0; i < msg.numPoints; i++) {
        // Skip points with invalid timestamps
        if (msg.data[i].timestamp < lastTimestamp) {
            continue;
        }

        // For the first timestamp we see, just store it without updating PID
        if (!hasSeenTimestamp) {
            lastTimestamp = msg.data[i].timestamp;
            hasSeenTimestamp = true;
            continue;
        }

        lastTimestamp = msg.data[i].timestamp;
        pid.update(msg.data[i].value, msg.data[i].timestamp);
    }
}

ControlMessage Controller::generateControlMessage() {
    ControlMessage msg;
    
    // Get the control value from PID controller
    float controlValue = pid.lastError * pid.kp + pid.integral * pid.ki;
    
    // Check if plant position exceeds threshold
    if ((pid.lastError > 0 ? pid.lastError : -pid.lastError) > threshold) {  // lastError = target - position
        msg.resetRequested = true;
        pid.reset();
        lastTimestamp = 0;  // Reset timestamp tracking on reset
        hasSeenTimestamp = false;  // Reset timestamp tracking flag
        controlValue = 0.0f;
    }
    
    // Simulate computation failure
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    if (dis(gen) < p_computationError) {
        msg.computationFailed = true;
        return msg;
    }
    
    msg.controlValue = controlValue;
    return msg;
} 