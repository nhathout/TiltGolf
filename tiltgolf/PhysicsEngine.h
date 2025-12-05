#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "box2d/box2d.h"
#include "IMU.h"
#include "LevelData.h"

class PhysicsEngine {
public:
    PhysicsEngine();
    ~PhysicsEngine();

    // Initialize the Box2D world with the given level
    void loadLevel(const LevelConfig& level);

    // Advance the simulation by one time step
    void step();

    // Getters for game logic
    b2Vec2 getBallPosition() const;
    float getBallAngle() const;
    
    // Reset ball to start
    void reset();

private:
    b2World* world;
    b2Body* ballBody;
    IMU imu;
    
    LevelConfig currentLevel;
    
    // Constants
    const float TIME_STEP = 1.0f / 60.0f;
    const int32 VELOCITY_ITERATIONS = 6;
    const int32 POSITION_ITERATIONS = 2;
};

#endif