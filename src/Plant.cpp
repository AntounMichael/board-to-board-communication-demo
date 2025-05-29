#include "Plant.h"

void Plant::updateSimulation() {
    unsigned long currentTime = millis();
    unsigned long deltaTime = currentTime - lastUpdateTime;
    
    // Convert deltaTime from milliseconds to seconds for physics calculations
    float dt = deltaTime / 1000.0f;
    
    // Calculate total force (external force + input force - damping)
    float totalForce = externalForce + input - (damping * state.velocity);
    
    // Calculate acceleration (F = ma)
    float acceleration = totalForce / mass;
    
    // Update velocity using Euler integration
    state.velocity += acceleration * dt;
    
    // Update position using Euler integration
    state.position += state.velocity * dt;
    
    // Add noise to both position and velocity
    state.position += generateNoise();
    state.velocity += generateNoise();
    
    // Update data buffer with current state
    if (dataBuffer.numPoints < MAX_POINTS) {
        dataBuffer.data[dataBuffer.numPoints].timestamp = currentTime;
        dataBuffer.data[dataBuffer.numPoints].value = state.position;
        dataBuffer.numPoints++;
    }
    
    // Update last update time
    lastUpdateTime = currentTime;
    
    // If we're running faster than our target delay, wait
    unsigned long elapsed = millis() - currentTime;
    if (elapsed < targetDelay) {
        delay(targetDelay - elapsed);
    }
}

void Plant::reset() {
    state.position = 0.0f;
    state.velocity = 0.0f;
    input = 0.0f;
    lastUpdateTime = millis();
    dataBuffer.numPoints = 0;
}

void Plant::handleControlMessage(const ControlMessage& msg) {
    if (msg.computationFailed) {
        // Keep using the previous input value
        return;
    }
    
    if (msg.resetRequested) {
        reset();
    }
    
    // Update input with new control value
    input = msg.controlValue;
} 