#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QTimer>
#include "PhysicsEngine.h"
#include "LevelData.h"

class GameController : public QObject {
    Q_OBJECT

public:
    explicit GameController(QObject *parent = nullptr);
    ~GameController();

    void loadLevel(int levelId);
    
    // Getters for View
    b2Vec2 getBallPos() const;
    LevelConfig getCurrentLevel() const;

public slots:
    void resetGame();
    void pauseGame();
    void resumeGame();

    // Expose simple IMU calibrate action to UI
    void calibrateIMU();

    // Live preview calibration control (note: resetBallToStart default true)
    void startCalibrationPreview(bool resetBallToStart = true);
    void acceptCalibrationPreview();
    void cancelCalibrationPreview();

signals:
    void gameStateUpdated(); // Tells view to repaint
    void gameWon();          // Tells GameScreen we finished

private slots:
    void gameLoop();

private:
    PhysicsEngine* physics;
    QTimer* gameTimer;
    LevelConfig currentLevel;
    bool isWon;
};

#endif