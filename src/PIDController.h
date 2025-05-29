#pragma once

class PIDController {
public:
    // Controller gains
    float kp = 1.0f;  // Proportional gain
    float ki = 0.1f;  // Integral gain
    
    // Target and limits
    float target = 0.0f;
    float maxIntegral = 100.0f;  // Integral windup protection
    float maxOutput = 10.0f;     // Output saturation
    
    // Controller state
    float integral = 0.0f;
    float lastError = 0.0f;
    unsigned long lastUpdateTime = 0;
    
    PIDController() = default;
    
    // Reset controller state
    void reset() {
        integral = 0.0f;
        lastError = 0.0f;
        lastUpdateTime = 0;
    }
    
    // Update controller with a single measurement
    float update(float measurement, unsigned long currentTime) {
        float error = target - measurement;
        
        // Calculate time delta (convert to seconds)
        float dt = 0.0f;
        if (lastUpdateTime != 0) {
            dt = (currentTime - lastUpdateTime) / 1000.0f;
        }
        
        // Update integral with anti-windup
        if (dt > 0) {
            integral += error * dt;
            // Anti-windup: clamp integral
            if (integral > maxIntegral) integral = maxIntegral;
            if (integral < -maxIntegral) integral = -maxIntegral;
        }
        
        // Calculate control output
        float output = kp * error + ki * integral;
        
        // Output saturation
        if (output > maxOutput) output = maxOutput;
        if (output < -maxOutput) output = -maxOutput;
        
        // Update state
        lastError = error;
        lastUpdateTime = currentTime;
        
        return output;
    }
}; 