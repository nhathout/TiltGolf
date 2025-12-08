#ifndef GAMESCREEN_H
#define GAMESCREEN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

// Forward declarations to avoid circular includes
class GameController;
class GameView;
class IMUScreen;
class CalibrationDialog;

class GameScreen : public QWidget {
    Q_OBJECT

public:
    explicit GameScreen(QWidget *parent = nullptr);
    ~GameScreen() = default;

    void setLevel(int newLevelId);
    void setFreePlayMode(bool enabled);
    void resetAttemptCounter();
    void setTotalLevels(int total);

signals:
    void exitToMenu();
    void levelComplete(int levelId);

private slots:
    void restartLevel();
    void updateTimer();
    
    // Slot to handle the "Game Won" signal from the controller
    void handleLevelComplete();

private:
    // Core Logic
    GameController* controller;
    
    // UI Elements
    GameView* gameView;

    QVBoxLayout *mainLayout;
    QHBoxLayout *topBar;
    QLabel *levelLabel;
    QLabel *timerLabel;
    QPushButton *restartButton;
    QPushButton *exitButton;
    QPushButton *calibrateButton; // new: button to trigger mag calibration

    QTimer *timer;
    int timeElapsed;
    int levelId;
    int attemptCount = 1;
    bool freePlayActive = false;
    int totalLevels = 6;
};

#endif