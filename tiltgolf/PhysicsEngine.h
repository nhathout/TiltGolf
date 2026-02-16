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

    // Simple IMU calibrate wrapper (permanent/in-memory)
    bool calibrateIMU();

    // Live preview calibration
    void startCalibrationPreview(bool resetBallToStart = true);
    bool commitCalibrationPreview();
    void cancelCalibrationPreview();

    // Utility to set ball transform (useful during preview)
    void setBallPosition(const b2Vec2 &pos, float angle = 0.0f);

    // Getters for game logic
    b2Vec2 getBallPosition() const;
    float getBallAngle() const;
    LevelConfig getLevelConfig() const;
    
    // Reset ball to start
    void reset();

private:
    b2World* world;
    b2Body* ballBody;
    IMU imu;
    
    LevelConfig currentLevel;

    // Constants (tune these to synchronize sensor tilt <-> ball movement)
    // ======= TUNABLES =======
    const float k_Force = 0.05f; // sensitivity

    // Change these to correct axis/direction mapping for your hardware:
    const bool swapXY = true;   // <-- SWAP axes (fixes "right tilt affected Y" issue)
    const bool invertX = false; // flip left/right if needed (try false first)
    const bool invertY = false; // <-- INVERT Y so forward/back map the intended up/down
    // ======= END TUNABLES =======

    const float deadzone = 0.02f;
    const float smoothAlpha = 0.25f;

    const float TIME_STEP = 1.0f / 60.0f;
    const int32 VELOCITY_ITERATIONS = 6;
    const int32 POSITION_ITERATIONS = 2;

    float prev_fx = 0.0f;
    float prev_fy = 0.0f;
};

#endif
