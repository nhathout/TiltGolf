#include "PhysicsEngine.h"
#include <iostream>
#include <cmath>

PhysicsEngine::PhysicsEngine() : world(nullptr), ballBody(nullptr) {
    // Initialize IMU
    if (!imu.begin()) {
        std::cerr << "PhysicsEngine: Failed to initialize IMU!" << std::endl;
    } else {
        std::cout << "PhysicsEngine: IMU Initialized." << std::endl;
    }
}

PhysicsEngine::~PhysicsEngine() {
    if (world) delete world;
}

void PhysicsEngine::loadLevel(const LevelConfig& level) {
    if (world) delete world;

    currentLevel = level;

    // Zero gravity because we are looking down at the table
    // Gravity/Force is applied manually via IMU
    b2Vec2 gravity(0.0f, 0.0f);
    world = new b2World(gravity);

    // 1. Create Walls (uniform restitution)
    b2BodyDef wallDef;
    wallDef.type = b2_staticBody;
    
    for (const auto& w : level.walls) {
        wallDef.position = w.position;
        b2Body* wall = world->CreateBody(&wallDef);
        
        b2PolygonShape box;
        box.SetAsBox(w.size.x, w.size.y);

        b2FixtureDef fixture;
        fixture.shape = &box;
        fixture.density = 0.0f;
        fixture.friction = 0.8f;
        fixture.restitution = 0.0f;
        wall->CreateFixture(&fixture);
    }

    // 2. Create Ball
    b2BodyDef ballDef;
    ballDef.type = b2_dynamicBody;
    ballDef.position = level.ballStartPos;
    ballDef.linearDamping = 1.0f; // Friction/Air resistance
    ballDef.angularDamping = 0.8f;
    
    ballBody = world->CreateBody(&ballDef);

    b2CircleShape circle;
    circle.m_radius = 0.5f; // 0.5m radius

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.6f; // Bounciness

    ballBody->CreateFixture(&fixtureDef);

    // Reset previous filter
    prev_fx = prev_fy = 0.0f;
}

void PhysicsEngine::reset() {
    if (ballBody) {
        ballBody->SetTransform(currentLevel.ballStartPos, 0.0f);
        ballBody->SetLinearVelocity(b2Vec2(0,0));
        ballBody->SetAngularVelocity(0);
        ballBody->SetAwake(true);

        // reset smoothing state so the ball responds immediately after reset
        prev_fx = prev_fy = 0.0f;
    }
}

void PhysicsEngine::step() {
    if (!world || !ballBody) return;

    // 1. Read IMU
    imu.update();

    // 2. Read calibrated sensor values (raw - bias)
    float sx = static_cast<float>(imu.getX());
    float sy = static_cast<float>(imu.getY());

    // 3. Axis swap if needed (depends on board mounting)
    float sensorX = swapXY ? sy : sx;
    float sensorY = swapXY ? sx : sy;

    // 4. Apply sign inversion if needed (tweak so clockwise tilt -> right on screen)
    if (invertX)
        sensorX = -sensorX;
    if (invertY)
        sensorY = -sensorY;

    // 5. Convert sensor reading into world force applying global scaling
    //    (k_Force chosen in header tunables)
    float raw_fx = sensorX * k_Force;
    float raw_fy = sensorY * k_Force;

    // 6. Deadzone to avoid tiny jitter
    if (std::fabs(raw_fx) < deadzone)
        raw_fx = 0.0f;
    if (std::fabs(raw_fy) < deadzone)
        raw_fy = 0.0f;

    // 7. Simple low-pass filter (IIR): smoothed = prev + alpha*(raw - prev)
    float fx = prev_fx + smoothAlpha * (raw_fx - prev_fx);
    float fy = prev_fy + smoothAlpha * (raw_fy - prev_fy);
    prev_fx = fx;
    prev_fy = fy;

    // 8. Apply Force to center of ball
    //    Note: positive fx moves ball in +X (right) direction; positive fy moves ball in +Y (down)
    ballBody->ApplyForceToCenter(b2Vec2(fx, fy), true);

    // 9. Step Box2D
    world->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);

    // 10. Update moving water positions (level 3)
    if (!currentLevel.movingWater.empty()) {
        for (auto &mw : currentLevel.movingWater) {
            mw.phase += mw.speed * TIME_STEP;
            float offset = mw.amplitude * std::sin(mw.phase) * (mw.direction >= 0.0f ? 1.0f : -1.0f);
            mw.position = b2Vec2(mw.basePosition.x, mw.basePosition.y + offset);
        }
    }

    // 11. Water hazards: check after step. If ball center is inside any water rect (static or moving), reset.
    b2Vec2 pos = ballBody->GetPosition();
    auto inside = [&](float cx, float cy, float hx, float hy) {
        float minX = cx - hx;
        float maxX = cx + hx;
        float minY = cy - hy;
        float maxY = cy + hy;
        return (pos.x >= minX && pos.x <= maxX && pos.y >= minY && pos.y <= maxY);
    };

    for (const auto &w : currentLevel.water) {
        if (inside(w.position.x, w.position.y, w.size.x, w.size.y)) {
            reset();
            return;
        }
    }
    for (const auto &mw : currentLevel.movingWater) {
        if (inside(mw.position.x, mw.position.y, mw.size.x, mw.size.y)) {
            reset();
            return;
        }
    }
}

b2Vec2 PhysicsEngine::getBallPosition() const {
    if (ballBody) return ballBody->GetPosition();
    return b2Vec2(0,0);
}

float PhysicsEngine::getBallAngle() const {
    if (ballBody) return ballBody->GetAngle();
    return 0.0f;
}

LevelConfig PhysicsEngine::getLevelConfig() const {
    return currentLevel;
}

bool PhysicsEngine::calibrateIMU()
{
    // Ask IMU to treat current reading as saved bias (in-memory)
    if (imu.calibrateNow())
    {
        std::cout << "PhysicsEngine: IMU calibration saved in-memory." << std::endl;
        return true;
    }
    else
    {
        std::cerr << "PhysicsEngine: IMU calibration failed." << std::endl;
        return false;
    }
}

void PhysicsEngine::startCalibrationPreview(bool resetBallToStart)
{
    // Set temporary bias to the current reading so the current pose becomes "zero"
    imu.setTempBiasFromCurrentReading();

    if (resetBallToStart && ballBody)
    {
        // Move ball to start position and stop motion so user can align easily
        ballBody->SetTransform(currentLevel.ballStartPos, 0.0f);
        ballBody->SetLinearVelocity(b2Vec2(0, 0));
        ballBody->SetAngularVelocity(0);
        ballBody->SetAwake(true);
    }

    std::cout << "PhysicsEngine: Calibration preview started." << std::endl;
}

bool PhysicsEngine::commitCalibrationPreview()
{
    // Copy the temp bias into saved bias (in-memory only)
    if (imu.commitTempBiasToSaved())
    {
        std::cout << "PhysicsEngine: Calibration preview committed." << std::endl;
        return true;
    }
    else
    {
        std::cerr << "PhysicsEngine: Failed to commit calibration preview." << std::endl;
        return false;
    }
}

void PhysicsEngine::cancelCalibrationPreview()
{
    imu.clearTempBias();
    std::cout << "PhysicsEngine: Calibration preview canceled." << std::endl;
}

void PhysicsEngine::setBallPosition(const b2Vec2 &pos, float angle)
{
    if (!ballBody)
        return;
    ballBody->SetTransform(pos, angle);
    ballBody->SetLinearVelocity(b2Vec2(0, 0));
    ballBody->SetAngularVelocity(0);
    ballBody->SetAwake(true);
}