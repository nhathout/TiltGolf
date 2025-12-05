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

class GameScreen : public QWidget {
    Q_OBJECT

public:
    explicit GameScreen(QWidget *parent = nullptr);
    ~GameScreen() = default;

    void setLevel(int newLevelId);

signals:
    void exitToMenu();

private slots:
    void restartLevel();
    void updateTimer();
    void openIMUScreen();
    
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
    
    QTimer *timer;
    int timeElapsed;
    int levelId;
};

#endif