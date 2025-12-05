#include "PhysicsEngine.h"
#include <iostream>

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

    // 1. Create Walls
    b2BodyDef wallDef;
    wallDef.type = b2_staticBody;
    
    for (const auto& w : level.walls) {
        wallDef.position = w.position;
        b2Body* wall = world->CreateBody(&wallDef);
        
        b2PolygonShape box;
        box.SetAsBox(w.size.x, w.size.y);
        wall->CreateFixture(&box, 0.0f);
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
}

void PhysicsEngine::reset() {
    if (ballBody) {
        ballBody->SetTransform(currentLevel.ballStartPos, 0.0f);
        ballBody->SetLinearVelocity(b2Vec2(0,0));
        ballBody->SetAngularVelocity(0);
        ballBody->SetAwake(true);
    }
}

void PhysicsEngine::step() {
    if (!world || !ballBody) return;

    // 1. Read IMU
    imu.update();
    
    // 2. Convert Magnetometer (Raw int16) to Force
    // Tuning Factor: You will likely need to adjust this depending on sensitivity
    float k_Force = 0.05f; 

    // Note: Orientation depends on how the board is mounted.
    // Assuming standard flat mount:
    float fx = imu.getX() * k_Force;
    float fy = imu.getY() * k_Force; // Might need to be -fy depending on axis

    // 3. Apply Force to center of ball
    ballBody->ApplyForceToCenter(b2Vec2(fx, fy), true);

    // 4. Step Box2D
    world->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
}

b2Vec2 PhysicsEngine::getBallPosition() const {
    if (ballBody) return ballBody->GetPosition();
    return b2Vec2(0,0);
}

float PhysicsEngine::getBallAngle() const {
    if (ballBody) return ballBody->GetAngle();
    return 0.0f;
}