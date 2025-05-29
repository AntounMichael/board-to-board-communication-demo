#pragma once

#include "DataMessage.h"
#include "ControlMessage.h"
#ifdef abs
#undef abs
#endif
#include <random>
#include <Arduino.h>

struct ParticleState {
    float position = 0.0f;
    float velocity = 0.0f;
};

class Plant {
public:
    // Simulation parameters
    float externalForce = 1.0f;
    float mass = 1.0f;
    float damping = 0.1f;
    float noiseScale = 0.0f;  // Set to 0 to disable noise
    
    // Simulation state
    ParticleState state;
    float input = 0.0f;
    const unsigned long targetDelay = 20;  // Target delay between updates in milliseconds
    unsigned long lastUpdateTime = 0;      // Last update timestamp
    
    // Data buffer for communication
    DataMessage dataBuffer;

    // Random number generation for noise
    std::mt19937 rng;
    std::normal_distribution<float> normalDist;

    Plant() : rng(std::random_device{}()), normalDist(0.0f, 1.0f) {
        lastUpdateTime = millis();
    }

    void updateSimulation();
    void reset();
    void handleControlMessage(const ControlMessage& msg);
    
private:
    float generateNoise() {
        return noiseScale * normalDist(rng);
    }
};
