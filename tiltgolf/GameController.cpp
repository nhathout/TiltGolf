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