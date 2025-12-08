#include "GameController.h"

#include <iostream>

GameController::GameController(QObject *parent) : QObject(parent), isWon(false) {
    physics = new PhysicsEngine();
    
    // Run game loop at ~60 FPS
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameController::gameLoop);
}

GameController::~GameController() {
    delete physics;
}

void GameController::loadLevel(int levelId) {
    currentLevel = LevelData::getLevel(levelId);
    physics->loadLevel(currentLevel);
    isWon = false;
    gameTimer->start(16); // ~60 FPS
}

void GameController::resetGame() {
    physics->reset();
    isWon = false;
    gameTimer->start();
    emit gameStateUpdated();
}

void GameController::pauseGame() {
    gameTimer->stop();
}

void GameController::resumeGame() {
    if (!isWon) gameTimer->start();
}

void GameController::gameLoop() {
    if (isWon) return;

    // 1. Step Physics
    physics->step();
    // Pull latest level state (for moving water)
    currentLevel = physics->getLevelConfig();

    // 2. Check Win Condition
    // Calculate distance between ball center and hole center
    b2Vec2 ballPos = physics->getBallPosition();
    b2Vec2 distVec = ballPos - currentLevel.holePos;
    
    // If distance is less than hole radius (ball center is inside hole)
    // You can adjust tolerance (e.g. radius * 0.5) to make it harder
    if (distVec.Length() < (currentLevel.holeRadius * 0.5f)) {
        isWon = true;
        gameTimer->stop();
        emit gameWon();
        std::cout << "HOLE IN ONE!" << std::endl;
    }

    // 3. Notify View
    emit gameStateUpdated();
}

b2Vec2 GameController::getBallPos() const {
    return physics->getBallPosition();
}

LevelConfig GameController::getCurrentLevel() const {
    return currentLevel;
}

void GameController::calibrateIMU()
{
    if (physics->calibrateIMU())
    {
        std::cout << "GameController: IMU calibrated via PhysicsEngine." << std::endl;
    }
    else
    {
        std::cerr << "GameController: IMU calibration failed." << std::endl;
    }
}

void GameController::startCalibrationPreview(bool resetBallToStart)
{
    physics->startCalibrationPreview(resetBallToStart);
}

void GameController::acceptCalibrationPreview()
{
    if (physics->commitCalibrationPreview())
    {
        std::cout << "GameController: Calibration preview accepted and saved." << std::endl;
    }
    else
    {
        std::cerr << "GameController: Failed to save calibration preview." << std::endl;
    }
}

void GameController::cancelCalibrationPreview()
{
    physics->cancelCalibrationPreview();
    std::cout << "GameController: Calibration preview canceled." << std::endl;
}